/*================================================================
Filename: VoxelModelEditorCore.h
Date: 2018.3.14
Created by AirGuanZ
================================================================*/
#pragma once

#include <cstdint>
#include <string>
#include <vector>

#include <SkeletonAnimation\SkeletonData.h>

class VMEBindingContent
{
public:
    struct Component
    {
        struct Voxel
        {
            std::int32_t x, y, z;
            std::uint8_t r, g, b;
        };

        std::string boneName;
        int boneIndex;

        float translateX, translateY, translateZ;
        float rotateX, rotateY, rotateZ, rotateW;
        float scaleX, scaleY, scaleZ;

        float voxelSize;
        std::int32_t boundPosX, boundNegX;
        std::int32_t boundPosY, boundNegY;
        std::int32_t boundPosZ, boundNegZ;

        //voxelCount以uint32_t存储在这
        std::vector<Voxel> voxels;
    };

    void Clear(void);

    bool LoadFromFile(const std::string &filename);
    bool SaveToFile(const std::string &filename);

    bool IsAvailable(void) const;

    std::string bindingPath;
    std::string skeletonPath;

    //skeleton并不被保存到文件
    //加载的时候也是根据skeletonPath来读取
    Skeleton::Skeleton skeleton;

    //componentCount以uint32_t存储在这
    std::vector<Component> components;
};

class VMECore
{
public:
    VMEBindingContent bindingContent;
};
