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

using namespace Skeleton;

SINGLETON_CLASS_DEFINITION(SkeletonDataLoader);

bool SkeletonDataLoader::LoadFromFile(const std::wstring &filename,
                                      Skeleton &skeleton,
                                      std::vector<BasicVertex> &vertices,
                                      std::string &errMsg)
{
    errMsg = "";
    skeleton.Clear();
    vertices.clear();

    //文件读取

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
    {
        errMsg = importer.GetErrorString();
        return false;
    }

    //TODO
}
