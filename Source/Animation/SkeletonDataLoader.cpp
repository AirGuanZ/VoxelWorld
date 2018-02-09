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

    //�ļ���ȡ

    std::vector<char> fileBuf;
    if(!Helper::ReadFileBinary(filename, fileBuf))
        return false;

    Assimp::Importer importer;
    std::unique_ptr<const aiScene> scene;
    scene.reset(importer.ReadFileFromMemory(
        fileBuf.data(), fileBuf.size(),
        aiProcess_JoinIdenticalVertices |   //����ʹ�ö��㻺��
        aiProcess_Triangulate |             //ֻ����������
        aiProcess_GenUVCoords |             //ǿ��ʹ��UV
        aiProcess_FlipUVs |                 //UVԭ�����������Ͻ�
        aiProcess_FlipWindingOrder          //��CWΪ����
    ));
    if(!scene)
    {
        errMsg = importer.GetErrorString();
        return false;
    }

    //TODO
}
