/*================================================================
Filename: VoxelModelEditorCore.cpp
Date: 2018.3.14
Created by AirGuanZ
================================================================*/
#include <cassert>
#include <cstdint>
#include <fstream>

#include <Utility\HelperFunctions.h>

#include <SkeletonAnimation\SkeletonDataLoader.h>

#include "VoxelModelEditorCore.h"

namespace
{
    constexpr std::uint8_t VMEBINDING_FILE_HEAD_MAGIC_0 = 0x5b;
    constexpr std::uint8_t VMEBINDING_FILE_HEAD_MAGIC_1 = 0xcf;
}

void VMEBindingContent::Clear(void)
{
    bindingPath = "";
    skeletonPath = "";

    skeletonTimeFactor = 1.0f;
    skeletonSizeFactor = 1.0f;

    originSkeleton.Clear();
    scaledSkeleton.Clear();

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

    //magic number检查
    std::uint8_t mg0, mg1;
    if(!Helper::ReadBinary(fin, mg0, mg1) ||
       mg0 != VMEBINDING_FILE_HEAD_MAGIC_0 ||
       mg1 != VMEBINDING_FILE_HEAD_MAGIC_1)
        goto FAILED;

    //骨骼动画参数
    if(!Helper::ReadBinary(fin, skeletonTimeFactor, skeletonSizeFactor))
        goto FAILED;

    //读取骨骼动画文件路径
    if(!Helper::ReadString(fin, skeletonPath))
        goto FAILED;

    //骨骼动画
    if(!Skeleton::SkeletonDataLoader::GetInstance().LoadFromVWFile(
                            Helper::ToWStr(skeletonPath),
                            skeletonTimeFactor, skeletonSizeFactor,
                            originSkeleton, boneMap, errMsg))
        goto FAILED;

    originSkeleton.Scale(skeletonTimeFactor, skeletonSizeFactor, scaledSkeleton);

    std::uint32_t componentCount;
    if(!Helper::ReadBinary(fin, componentCount))
        goto FAILED;

    components.resize(componentCount);
    for(std::uint32_t i = 0; i < componentCount; ++i)
    {
        Component &cpt = components[i];

        if(!Helper::ReadString(fin, cpt.componentName))
            goto FAILED;
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

        cpt.meshNeedUpdating = true;
    }

    return true;

FAILED:
    Clear();
    return false;
}

bool VMEBindingContent::SaveToFile(const std::string &filename) const
{
    std::ofstream fout(filename, std::ios_base::out | std::ios_base::binary | std::ios_base::trunc);
    if(!fout)
        return false;

    using Helper::WriteBinary;
    using Helper::WriteString;

    //文件头部的magic number
    if(!WriteBinary(fout, VMEBINDING_FILE_HEAD_MAGIC_0, VMEBINDING_FILE_HEAD_MAGIC_1))
        return false;

    if(!WriteBinary(fout, skeletonTimeFactor, skeletonSizeFactor))
        return false;

    if(!WriteString(fout, skeletonPath))
        return false;

    if(!WriteBinary(fout, static_cast<std::uint32_t>(components.size())))
        return false;

    for(const Component &cpt : components)
    {
        if(!WriteString(fout, cpt.componentName))
            return false;

        if(!WriteString(fout, cpt.boneName) || !WriteBinary(fout, cpt.boneIndex))
            return false;

        if(!WriteBinary(fout, cpt.translateX, cpt.translateY, cpt.translateZ))
            return false;

        if(!WriteBinary(fout, cpt.rotateX, cpt.rotateY, cpt.rotateZ, cpt.rotateW))
            return false;

        if(!WriteBinary(fout, cpt.scaleX, cpt.scaleY, cpt.scaleZ))
            return false;

        if(!WriteBinary(fout, cpt.voxelSize))
            return false;

        if(!WriteBinary(fout, cpt.boundPosX, cpt.boundNegX,
                              cpt.boundPosY, cpt.boundNegY,
                              cpt.boundPosZ, cpt.boundNegZ))
            return false;

        if(!WriteBinary(fout, static_cast<std::uint32_t>(cpt.voxels.size())))
            return false;

        for(const Component::Voxel &v : cpt.voxels)
        {
            if(!WriteBinary(fout, v.x, v.y, v.z))
                return false;

            if(!WriteBinary(fout, v.r, v.g, v.b))
                return false;
        }
    }

    return true;
}

bool VMEBindingContent::IsAvailable(void) const
{
    return bindingPath.size() != 0;
}

void VMEBindingContent::RefreshScaledSkeleton(void)
{
    if(scaledSkeleton.GetParents().size())
        scaledSkeleton.Clear();
    if(originSkeleton.GetParents().size())
        originSkeleton.Scale(skeletonTimeFactor, skeletonSizeFactor, scaledSkeleton);
}
