/*================================================================
Filename: SkeletonDataLoader.cpp
Date: 2018.2.9
Created by AirGuanZ
================================================================*/
#include <fstream>
#include <memory>

#include <assimp\Importer.hpp>
#include <assimp\scene.h>
#include <assimp\postprocess.h>

#include "../Utility/HelperFunctions.h"
#include "SkeletonDataLoader.h"

SINGLETON_CLASS_DEFINITION(Skeleton::SkeletonDataLoader);

namespace
{
    std::unique_ptr<const aiScene> ReadModel(const std::wstring &filename, std::string &errMsg)
    {
        std::vector<char> fileBuf;
        if(!Helper::ReadFileBinary(filename, fileBuf))
            return false;

        Assimp::Importer importer;
        std::unique_ptr<const aiScene> scene;
        scene.reset(importer.ReadFileFromMemory(
            fileBuf.data(), fileBuf.size(),
            aiProcess_JoinIdenticalVertices |   //便于使用顶点缓存
            aiProcess_Triangulate |             //只允许三角形
            aiProcess_GenUVCoords |             //强制使用UV
            aiProcess_FlipUVs |                 //UV原点设置在左上角
            aiProcess_FlipWindingOrder          //以CW为正面
        ));

        if(!scene)
            errMsg = importer.GetErrorString();
        return scene;
    }
}

bool Skeleton::SkeletonDataLoader::LoadFromFile(const std::wstring &filename,
                                                Skeleton &skeleton,
                                                std::string &errMsg)
{
    skeleton.Clear();
    errMsg = "";

    std::unique_ptr<const aiScene> scene = ReadModel(filename, errMsg);
    if(!scene)
        return false;

    //TODO
    return true;
}
