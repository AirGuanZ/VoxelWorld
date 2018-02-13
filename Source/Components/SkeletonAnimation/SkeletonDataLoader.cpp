/*================================================================
Filename: SkeletonDataLoader.cpp
Date: 2018.2.9
Created by AirGuanZ
================================================================*/
#include <fstream>
#include <memory>
#include <set>
#include <type_traits>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <Utility/ConfigFile.h>
#include <Utility/HelperFunctions.h>

#include "SkeletonDataLoader.h"

SINGLETON_CLASS_DEFINITION(Skeleton::SkeletonDataLoader);

namespace
{
    inline std::string GetBoneName(const std::string &srcName, int idx)
    {
        static const std::string AUTO_BONE_NAME_PREFIX = "_AutoNamedBone_";
        return srcName.empty() ? AUTO_BONE_NAME_PREFIX + std::to_string(idx) : srcName;
    }

    inline std::string TrimArmatureNamePrefix(const std::string &str)
    {
        static const std::string PREFIX = "Armature|";
        if(str.substr(0, PREFIX.size()) == PREFIX)
            return str.substr(PREFIX.size(), str.size() - PREFIX.size());
        return str;
    }

    inline void CopyMatrix(Matrix &dst, const aiMatrix4x4 &src)
    {
        dst = Matrix(src.a1, src.a2, src.a3, src.a4,
                     src.b1, src.b2, src.b3, src.b4, 
                     src.c1, src.c2, src.c3, src.c4, 
                     src.d1, src.d2, src.d3, src.d4).Transpose();
    }

    /*
        �ݹ���ʽڵ�������parents�й��������ʾ�Ĺ�����
        boneIdx���ڼ�¼���ֵ������±��ӳ��
        ��û�����ֵĽڵ㣬_AutoNamedBone_ + idx����Ϊ�Զ������Ľ��
    */
    bool ReadStaticSkeleton(aiNode *node,
                            std::vector<int> &parents,
                            std::map<std::string, int> &boneIdx,
                            int &idx, int directParent,
                            std::string &errMsg)
    {
        assert(node != nullptr);

        //��������������ӵ����ͬ������
        if(boneIdx.find(node->mName.C_Str()) != boneIdx.end())
        {
            errMsg = "Bone name repeated";
            return false;
        }

        //�������Ϊ�գ����Զ�����
        std::string finalName = GetBoneName(node->mName.C_Str(), idx);
        boneIdx[finalName] = idx;

        assert(parents.size() == idx);
        parents.push_back(idx);

        int thisIdx = idx++;

        //�ݹ�ؽ��ӽڵ���������
        for(unsigned int i = 0; i != node->mNumChildren; ++i)
        {
            if(!ReadStaticSkeleton(node->mChildren[i], parents,
                                   boneIdx, idx, thisIdx, errMsg))
                return false;
        }

        return true;
    }

    //��scene�ж�ȡarmature�ڵ�������������Ĺ�����
    bool InitStaticSkeletonFromAIScene(const aiScene *scene,
                                       Skeleton::Skeleton &skeleton,
                                       std::map<std::string, int> &boneIdx,
                                       std::set<std::string> &directChildrenNames,
                                       std::string &errMsg)
    {
        assert(scene != nullptr);
        aiNode *armature = scene->mRootNode->FindNode("Armature");
        if(!armature)
        {
            errMsg = "'Armature' not found";
            return false;
        }
        
        int idx = 0;
        std::vector<int> parents;

        for(unsigned int i = 0; i != armature->mNumChildren; ++i)
        {
            //��ÿ���ӽڵ㣬��-1��Ϊdirect parent����ʾû�и��ڵ�
            if(!ReadStaticSkeleton(armature->mChildren[i], parents,
                                   boneIdx, idx, -1, errMsg))
                return false;
            if(armature->mChildren[i]->mName.C_Str()[0] != '\0')
                directChildrenNames.insert(armature->mChildren[i]->mName.C_Str());
        }

        skeleton.Initialize(std::move(parents));

        return true;
    }

    //��ȡ�������е����ڵ�Ĺؼ�֡����
    //Ҫ��ÿ���ؼ�֡ͬʱ��¼scaling, rotation��translation��Ϣ
    bool ReadSkeletonAnimationForSingleNode(aiNodeAnim *nodeAni,
                                            const Quaternion &rotCorrection,
                                            float timeFactor,
                                            Skeleton::BoneAni &boneAni,
                                            int idx)
    {
        if(nodeAni->mNumPositionKeys != nodeAni->mNumRotationKeys ||
           nodeAni->mNumRotationKeys != nodeAni->mNumScalingKeys)
            return false;

        for(unsigned int kfIdx = 0; kfIdx != nodeAni->mNumPositionKeys; ++kfIdx)
        {
            const aiVectorKey &kPos = nodeAni->mPositionKeys[kfIdx];
            const aiQuatKey   &kRot = nodeAni->mRotationKeys[kfIdx];
            const aiVectorKey &kScl = nodeAni->mScalingKeys[kfIdx];

            constexpr double epsilon = 1e-5;
            if(std::abs(kPos.mTime - kRot.mTime) > epsilon ||
               std::abs(kRot.mTime - kScl.mTime) > epsilon)
                return false;

            Skeleton::Keyframe kf;
            kf.translate = Vector3::Transform(Vector3(kPos.mValue.x, kPos.mValue.y, kPos.mValue.z), rotCorrection);
            kf.rotate    = rotCorrection * Quaternion(kRot.mValue.x, kRot.mValue.y, kRot.mValue.z, kRot.mValue.w);
            kf.scale     = Vector3(kScl.mValue.x, kScl.mValue.y, kScl.mValue.z);
            kf.time      = timeFactor * static_cast<float>(kPos.mTime);

            boneAni.keyframes.push_back(kf);
        }

        return true;
    }

    //��scene�ж�ȡ�������У�armature�������ĸ������ؼ�֡����
    //Ҫ��ÿ���ؼ�֡ͬʱ��¼scaling, rotation��translation��Ϣ
    bool InitSkeletonAnimationFromAIScene(const aiScene *scene,
                                          std::set<std::string> directBoneNames,
                                          float timeFactor,
                                          Skeleton::Skeleton &skeleton,
                                          const std::map<std::string, int> &boneIdx,
                                          const std::string &defaultClipName,
                                          std::string &errMsg)
    {
        for(unsigned int aniIdx = 0; aniIdx != scene->mNumAnimations; ++aniIdx)
        {
            aiAnimation *ani = scene->mAnimations[aniIdx];

            Skeleton::AniClip aniClip;
            aniClip.boneAnis.resize(boneIdx.size());

            for(unsigned int i = 0; i != ani->mNumChannels; ++i)
            {
                aiNodeAnim *nodeAni = ani->mChannels[i];

                //���������Ķ����ᱻ����
                if(nodeAni->mNodeName.C_Str()[0] == '\0')
                    continue;

                //����armature�Ǽ��ϵĹ��������ᱻ����
                auto it = boneIdx.find(nodeAni->mNodeName.C_Str());
                if(it == boneIdx.end())
                    continue;

                Quaternion rotCorrection = Quaternion::Identity;
                if(directBoneNames.find(nodeAni->mNodeName.C_Str()) != directBoneNames.end())
                    rotCorrection = Quaternion::CreateFromYawPitchRoll(0.0f, -DirectX::XM_PIDIV2, 0.0f);

                //��ȡ���������Ĺؼ�֡����
                Skeleton::BoneAni &boneAni = aniClip.boneAnis[it->second];
                if(!ReadSkeletonAnimationForSingleNode(ani->mChannels[i], rotCorrection, timeFactor, boneAni, it->second))
                    return false;
            }

            aniClip.UpdateStartEndTime();

            std::string aniClipName = TrimArmatureNamePrefix(ani->mName.C_Str());
            if(aniClipName.empty())
                aniClipName = defaultClipName;
            if(!skeleton.AddClip(aniClipName, std::move(aniClip)))
                return false;
        }

        return true;
    }
}

bool Skeleton::SkeletonDataLoader::LoadFromRawFile(const std::wstring &filename,
                                                   float timeFactor,
                                                   Skeleton &skeleton,
                                                   std::map<std::string, int> &boneIdx,
                                                   const std::string &defaultClipName,
                                                   std::string &errMsg)
{
    skeleton.Clear();
    boneIdx.clear();
    errMsg = "";

    Assimp::Importer importer;
    const aiScene *scene = nullptr;

    std::set<std::string> directChildrenNames;

    std::vector<char> fileBuf;
    if(!Helper::ReadFileBinary(filename, fileBuf))
    {
        errMsg = "Failed to open model file";
        goto FAILED;
    }

    scene = importer.ReadFileFromMemory(
        fileBuf.data(), fileBuf.size(),
        aiProcess_JoinIdenticalVertices |   //����ʹ��index buffer
        aiProcess_Triangulate |             //ֻ����������
        aiProcess_GenUVCoords |             //ǿ��ʹ��UV
        aiProcess_FlipUVs |                 //UVԭ�����������Ͻ�
        aiProcess_FlipWindingOrder          //��CWΪ����
    );

    if(!scene)
    {
        errMsg = importer.GetErrorString();
        goto FAILED;
    }
    
    if(!InitStaticSkeletonFromAIScene(scene, skeleton, boneIdx, directChildrenNames, errMsg))
    {
        errMsg = "Failed to load static skeleton: " + errMsg;
        goto FAILED;
    }

    if(!InitSkeletonAnimationFromAIScene(scene, directChildrenNames, timeFactor, skeleton, boneIdx, defaultClipName, errMsg))
    {
        errMsg = "Failed to load animation for skeleton: " + errMsg;
        goto FAILED;
    }

    return true;

FAILED:
    skeleton.Clear();
    boneIdx.clear();
    return false;
}

namespace
{
    const std::string VW_MODEL_HEADER_NAME = "Model";
    const std::string VW_MODEL_SKELETON_NAME_PREFIX = "Name";
    const std::string VW_MODEL_SKELETON_PARENT_PREFIX = "";
}

bool Skeleton::SkeletonDataLoader::LoadFromVWFile(const std::wstring &filename,
                                                  float timeFactor,
                                                  float sizeFactor,
                                                  Skeleton &skeleton,
                                                  std::map<std::string, int> boneMap,
                                                  std::string &errMsg)
{
    skeleton.Clear();
    boneMap.clear();
    errMsg = "";

    ConfigFile file;
    if(!file.LoadFromFile(filename))
        return false;

    //TODO

    return true;
}
