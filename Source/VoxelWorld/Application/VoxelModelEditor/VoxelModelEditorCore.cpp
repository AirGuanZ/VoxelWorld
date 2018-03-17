/*================================================================
Filename: VoxelModelEditorCore.cpp
Date: 2018.3.14
Created by AirGuanZ
================================================================*/
#include <cassert>
#include <fstream>

#include <Utility\HelperFunctions.h>

#include <SkeletonAnimation\SkeletonDataLoader.h>

#include "VoxelModelEditorCore.h"

void VMEBindingContent::Clear(void)
{
    bindingPath = "";
    skeletonPath = "";

    skeleton.Clear();

    components.clear();
}

bool VMEBindingContent::LoadFromFile(const std::string &filename)
{
    Clear();

    std::map<std::string, int> boneMap;
    std::string errMsg;

    std::ifstream fin(filename, std::ios_base::in | std::ios_base::binary);
    if(!fin)
        goto FAILED;

    bindingPath = filename;

    //读取骨骼动画文件路径
    if(!Helper::ReadString(fin, skeletonPath))
        goto FAILED;

    //骨骼动画
    if(!Skeleton::SkeletonDataLoader::GetInstance().LoadFromVWFile(
        Helper::ToWStr(skeletonPath), 1.0f, 1.0f, skeleton, boneMap, errMsg))
        goto FAILED;

    std::uint32_t componentCount;
    if(!Helper::ReadBinary(fin, componentCount))
        goto FAILED;
    components.resize(componentCount);
    for(std::uint32_t i = 0; i < componentCount; ++i)
    {
        Component &cpt = components[i];

        if(!Helper::ReadString(fin, cpt.boneName))
            goto FAILED;
        if(!Helper::ReadBinary(fin, cpt.boneIndex))
            goto FAILED;
        
        //校验一下binding文件中的骨骼编号个skeleton文件中的boneMap信息是否吻合
        auto it = boneMap.find(cpt.boneName);
        if(it == boneMap.end() || it->second != cpt.boneIndex)
            goto FAILED;

        if(!Helper::ReadBinary(fin, cpt.translateX, cpt.translateY, cpt.translateZ))
            goto FAILED;
        if(!Helper::ReadBinary(fin, cpt.rotateX, cpt.rotateY, cpt.rotateZ, cpt.rotateW))
            goto FAILED;
        if(!Helper::ReadBinary(fin, cpt.scaleX, cpt.scaleY, cpt.scaleZ))
            goto FAILED;

        if(!Helper::ReadBinary(fin, cpt.voxelSize))
            goto FAILED;

        if(!Helper::ReadBinary(fin, cpt.boundPosX, cpt.boundNegX,
                                    cpt.boundPosY, cpt.boundNegY,
                                    cpt.boundPosZ, cpt.boundNegZ))
            goto FAILED;

        std::uint32_t voxelCount;
        if(!Helper::ReadBinary(fin, voxelCount))
            goto FAILED;

        cpt.voxels.resize(voxelCount);
        for(std::uint32_t j = 0; j < voxelCount; ++j)
        {
            Component::Voxel &v = cpt.voxels[j];

            if(!Helper::ReadBinary(fin, v.x, v.y, v.z) ||
               !Helper::ReadBinary(fin, v.r, v.g, v.b))
                goto FAILED;
        }
    }

    return true;

FAILED:
    Clear();
    return false;
}

bool VMEBindingContent::SaveToFile(const std::string &filename)
{
    return true;
}

bool VMEBindingContent::IsAvailable(void) const
{
    return bindingPath.size() != 0;
}
