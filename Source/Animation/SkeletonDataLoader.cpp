/*================================================================
Filename: SkeletonDataLoader.cpp
Date: 2018.2.9
Created by AirGuanZ
================================================================*/
#include <fstream>
#include <iostream>
#include <memory>
#include <type_traits>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "../Utility/HelperFunctions.h"
#include "SkeletonDataLoader.h"

SINGLETON_CLASS_DEFINITION(Skeleton::SkeletonDataLoader);

namespace
{
    void PrintNodes(aiNode *node)
    {
        if(!node)
            return;

        std::cerr << node->mName.C_Str() << std::endl;

        for(int i = 0; i != node->mNumChildren; ++i)
            PrintNodes(node->mChildren[i]);
    }

    void CopyMatrix(Matrix &dst, const aiMatrix4x4 &src)
    {
        dst = Matrix(src.a1, src.a2, src.a3, src.a4,
                     src.b1, src.b2, src.b3, src.b4, 
                     src.c1, src.c2, src.c3, src.c4, 
                     src.d1, src.d2, src.d3, src.d4).Transpose();
    }

    bool ReadStaticSkeleton(aiNode *node,
                            std::vector<int> &parents,
                            std::vector<Matrix> &offsets,
                            std::map<std::string, int> &boneIdx,
                            int &idx, int directParent,
                            std::string &errMsg)
    {
        assert(node != nullptr);
        static const std::string AUTO_BONE_NAME_PREFIX = "_AutoNamedBone_";

        if(boneIdx.find(node->mName.C_Str()) != boneIdx.end())
        {
            errMsg = "Bone name repeated";
            return false;
        }
        boneIdx[node->mName.C_Str()] = idx;

        assert(parents.size() == idx && offsets.size() == idx);
        Matrix offset; CopyMatrix(offset, node->mTransformation);
        parents.push_back(directParent);
        offsets.push_back(offset);

        int thisIdx = idx++;

        for(unsigned int i = 0; i != node->mNumChildren; ++i)
        {
            if(!ReadStaticSkeleton(node->mChildren[i], parents, offsets,
                                   boneIdx, idx, thisIdx, errMsg))
                return false;
        }

        return true;
    }

    bool InitStaticSkeletonFromAIScene(const aiScene *scene,
                                       Skeleton::Skeleton &skeleton,
                                       std::map<std::string, int> &boneIdx,
                                       std::string &errMsg)
    {
        assert(scene == nullptr);
        aiNode *armature = scene->mRootNode->FindNode("Armature");
        if(!armature)
        {
            errMsg = "'Armature' not found";
            return false;
        }
        
        int idx = 0;
        std::vector<int> parents;
        std::vector<Matrix> offsets;
        for(unsigned int i = 0; i != armature->mNumChildren; ++i)
        {
            if(!ReadStaticSkeleton(armature->mChildren[i], parents, offsets,
                                   boneIdx, idx, -1, errMsg))
                return false;
        }

        return true;
    }
}

bool Skeleton::SkeletonDataLoader::LoadFromFile(const std::wstring &filename,
                                                Skeleton &skeleton,
                                                std::map<std::string, int> &boneIdx,
                                                std::string &errMsg)
{
    skeleton.Clear();
    boneIdx.clear();
    errMsg = "";

    Assimp::Importer importer;
    const aiScene *scene = nullptr;

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
    
    if(!InitStaticSkeletonFromAIScene(scene, skeleton, boneIdx, errMsg))
    {
        errMsg = "Failed to load static skeleton: " + errMsg;
        goto FAILED;
    }

    return true;

FAILED:
    skeleton.Clear();
    boneIdx.clear();
    return false;
}
