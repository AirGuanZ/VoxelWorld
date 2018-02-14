/*================================================================
Filename: SkeletonDataLoader.cpp
Date: 2018.2.9
Created by AirGuanZ
================================================================*/
#define _CRT_SECURE_NO_WARNINGS

#include <cstdio>
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
        递归访问节点数，在parents中构造数组表示的骨骼树
        boneIdx用于记录名字到骨骼下标的映射
        对没有名字的节点，_AutoNamedBone_ + idx将作为自动命名的结果
    */
    bool ReadStaticSkeleton(aiNode *node,
                            std::vector<int> &parents,
                            std::map<std::string, int> &boneIdx,
                            int &idx, int directParent,
                            std::string &errMsg)
    {
        assert(node != nullptr);

        //不允许两个骨骼拥有相同的名字
        if(boneIdx.find(node->mName.C_Str()) != boneIdx.end())
        {
            errMsg = "Bone name repeated";
            return false;
        }

        //如果名字为空，就自动命名
        std::string finalName = GetBoneName(node->mName.C_Str(), idx);
        boneIdx[finalName] = idx;

        assert(parents.size() == idx);
        parents.push_back(directParent);

        int thisIdx = idx++;

        //递归地将子节点加入骨骼树
        for(unsigned int i = 0; i != node->mNumChildren; ++i)
        {
            if(!ReadStaticSkeleton(node->mChildren[i], parents,
                                   boneIdx, idx, thisIdx, errMsg))
                return false;
        }

        return true;
    }

    //从scene中读取armature节点的子树所描述的骨骼树
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
            //对每个子节点，用-1作为direct parent，表示没有父节点
            if(!ReadStaticSkeleton(armature->mChildren[i], parents,
                                   boneIdx, idx, -1, errMsg))
                return false;
            if(armature->mChildren[i]->mName.C_Str()[0] != '\0')
                directChildrenNames.insert(armature->mChildren[i]->mName.C_Str());
        }

        skeleton.Initialize(std::move(parents));

        return true;
    }

    //读取骨骼树中单个节点的关键帧序列
    //要求每个关键帧同时记录scaling, rotation和translation信息
    bool ReadSkeletonAnimationForSingleNode(aiNodeAnim *nodeAni,
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
            kf.translate = Vector3(kPos.mValue.x, kPos.mValue.y, kPos.mValue.z);
            kf.rotate    = Quaternion(kRot.mValue.x, kRot.mValue.y, kRot.mValue.z, kRot.mValue.w);
            kf.scale     = Vector3(kScl.mValue.x, kScl.mValue.y, kScl.mValue.z);
            kf.time      = timeFactor * static_cast<float>(kPos.mTime);

            boneAni.keyframes.push_back(kf);
        }

        return true;
    }

    //从scene中读取各动作中，armature骨骼树的各骨骼关键帧序列
    //要求每个关键帧同时记录scaling, rotation和translation信息
    bool InitSkeletonAnimationFromAIScene(const aiScene *scene,
                                          std::set<std::string> &directBoneNames,
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

                //无名骨骼的动画会被忽略
                if(nodeAni->mNodeName.C_Str()[0] == '\0')
                    continue;

                //不在armature骨架上的骨骼动画会被忽略
                auto it = boneIdx.find(nodeAni->mNodeName.C_Str());
                if(it == boneIdx.end())
                    continue;

                //读取单个骨骼的关键帧序列
                Skeleton::BoneAni &boneAni = aniClip.boneAnis[it->second];
                if(!ReadSkeletonAnimationForSingleNode(ani->mChannels[i], timeFactor, boneAni, it->second))
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
        aiProcess_JoinIdenticalVertices |   //便于使用index buffer
        aiProcess_Triangulate |             //只允许三角形
        aiProcess_GenUVCoords |             //强制使用UV
        aiProcess_FlipUVs |                 //UV原点设置在左上角
        aiProcess_FlipWindingOrder          //以CW为正面
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
    bool ParseVWBoneClip(const ConfigFileSection &section,
                         Skeleton::BoneAni &ani,
                         float timeFactor, float sizeFactor,
                         std::string &errMsg)
    {
        int kfCount = std::stoi(section["KeyframeCount"]);
        if(kfCount < 0)
        {
            errMsg = "Negative keyframe count";
            return false;
        }

        ani.keyframes.resize(kfCount);
        for(int kfIdx = 0; kfIdx < kfCount; ++kfIdx)
        {
            std::string idxStr = std::to_string(kfIdx);
            Skeleton::Keyframe &kf = ani.keyframes[kfIdx];

            kf.time = timeFactor * std::stof(section["Time" + idxStr]);
            
            if(sscanf(section["Pos" + idxStr].c_str(), "%f%f%f",
                    &kf.translate.x, &kf.translate.y, &kf.translate.z) != 3 ||
               sscanf(section["Rot" + idxStr].c_str(), "%f%f%f%f",
                    &kf.rotate.x, &kf.rotate.y, &kf.rotate.z, &kf.rotate.w) != 4 ||
               sscanf(section["Scl" + idxStr].c_str(), "%f%f%f",
                    &kf.scale.x, &kf.scale.y, &kf.scale.z) != 3)
            {
                errMsg = "Invalid keyframe arguments";
                return false;
            }
            kf.translate = sizeFactor * kf.translate;
            kf.scale = sizeFactor * kf.scale;
        }

        return true;
    }
}

bool Skeleton::SkeletonDataLoader::LoadFromVWFile(const std::wstring &filename,
                                                  float timeFactor,
                                                  float sizeFactor,
                                                  Skeleton &skeleton,
                                                  std::map<std::string, int> &boneMap,
                                                  std::string &errMsg)
{
    skeleton.Clear();
    boneMap.clear();
    errMsg = "";

    ConfigFile file(filename);
    if(!file)
    {
        errMsg = "Failed to open skeleton animation file";
        goto FAILED;
    }

    try
    {
        int boneCount      = std::stoi(file("Head", "BoneCount"));
        int animationCount = std::stoi(file("Head", "AnimationCount"));
        std::vector<int> parents(boneCount);
        std::vector<std::string> aniNames(animationCount);

        if(boneCount <= 0 || animationCount <= 0)
        {
            errMsg = "Non-positive boundCount/animationCount";
            goto FAILED;
        }

        //取得映射：骨骼名字->骨骼编号，骨骼编号->父骨骼编号
        for(int boneIdx = 0; boneIdx < boneCount; ++boneIdx)
        {
            std::string idxStr = std::to_string(boneIdx);
            std::string name = file("Bones", "Name" + idxStr);
            int parentIdx = std::stoi(file("Bones", "Parent" + idxStr));

            if(name.empty())
            {
                errMsg = "Empty bone name";
                goto FAILED;
            }

            if(parentIdx < -1 || parentIdx >= boneCount)
            {
                errMsg = "Parent index out of range";
                goto FAILED;
            }

            boneMap[name] = boneIdx;
            parents[boneIdx] = parentIdx;
        }

        skeleton.Initialize(std::move(parents));

        //骨骼动画名字
        for(int aniIdx = 0; aniIdx < animationCount; ++aniIdx)
        {
            std::string name = file("Animations", "AnimationName" + std::to_string(aniIdx));
            if(name.empty())
            {
                errMsg = "Empty animation clip name";
                goto FAILED;
            }
            aniNames[aniIdx] = name;
        }

        //对每个动画，遍历每个骨骼，尝试读取其关键帧序列
        for(int aniIdx = 0; aniIdx < animationCount; ++aniIdx)
        {
            AniClip aniClip;
            aniClip.boneAnis.resize(boneCount);
            std::string aniIdxStr = std::to_string(aniIdx);

            for(int boneIdx = 0; boneIdx < boneCount; ++boneIdx)
            {
                std::string section = "BoneClip" + aniIdxStr + "_" + std::to_string(boneIdx);
                if(!file.FindSection(section))
                    continue;
                if(!ParseVWBoneClip(file.GetSection(section), aniClip.boneAnis[boneIdx],
                                    timeFactor, sizeFactor, errMsg))
                    goto FAILED;;
            }

            aniClip.UpdateStartEndTime();
            if(!skeleton.AddClip(aniNames[aniIdx], std::move(aniClip)))
            {
                errMsg = "Animation name repeated";
                goto FAILED;
            }
        }
    }
    catch(const std::invalid_argument &err)
    {
        errMsg = std::string("Invalid argument in parsing") + err.what();
        goto FAILED;
    }

    return true;

FAILED:
    skeleton.Clear();
    boneMap.clear();
    return false;
}

bool Skeleton::SkeletonDataLoader::SaveToVWFile(const std::wstring &filename,
                                                const Skeleton &skeleton,
                                                const std::map<std::string, int> &boneMap)
{
    using std::endl;

    std::ofstream out(filename, std::ofstream::out | std::ofstream::trunc);
    if(!out)
        return false;

    //文件头
    out << "[Head]" << endl
        << endl
        << "BoneCount = " << skeleton.parents_.size() << endl
        << "AnimationCount = " << skeleton.aniClips_.size() << endl
        << endl;

    //骨骼名字和层次关系
    out << "[Bones]" << endl
        << endl;
    for(int i = 0; i != skeleton.parents_.size(); ++i)
        out << "Parent" << i << " = " << skeleton.parents_[i] << endl;
    out << endl;
    for(auto it : boneMap)
        out << "Name" << it.second << " = " << it.first << endl;
    out << endl;

    //动画名
    out << "[Animations]" << endl
        << endl;
    std::vector<const AniClip*> anis;
    for(const auto &it : skeleton.aniClips_)
    {
        out << "AnimationName" << anis.size() << " = " << it.first << endl;
        anis.push_back(&it.second);
    }
    out << endl;

    //遍历所有动画和骨骼，输出关键帧
    for(int aniIdx = 0; aniIdx != anis.size(); ++aniIdx)
    {
        const AniClip &aniClip = *anis[aniIdx];

        for(int boneIdx = 0; boneIdx != skeleton.parents_.size(); ++boneIdx)
        {
            const std::vector<Keyframe> &kfs = aniClip.boneAnis[boneIdx].keyframes;
            if(kfs.empty())
                continue;

            out << "[BoneClip" << aniIdx << "_" << boneIdx << "]" << endl
                << endl
                << "KeyframeCount = " << kfs.size() << endl
                << endl;

            for(int kfIdx = 0; kfIdx != kfs.size(); ++kfIdx)
            {
                const Keyframe &kf = kfs[kfIdx];
                out.precision(12);

                out << "Time" << kfIdx << " = " << kf.time << endl
                    << endl;

                out << "Pos" << kfIdx << " = "
                                      << kf.translate.x << " "
                                      << kf.translate.y << " "
                                      << kf.translate.z << endl
                    << "Rot" << kfIdx << " = "
                                      << kf.rotate.x << " "
                                      << kf.rotate.y << " "
                                      << kf.rotate.z << " "
                                      << kf.rotate.w << endl
                    << "Scl" << kfIdx << " = "
                                      << kf.scale.x << " "
                                      << kf.scale.y << " "
                                      << kf.scale.z << endl
                    << endl;
            }
        }
    }

    return true;
}
