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

/*
    VMEBindingContent文件格式：
        uint8_t magicNumber0, magicNumber1
        float skeletonTimeFactor
        float skeletonSizeFactor
        string skeletonPath
        uint32_t componentCount
        componentCount * {
            string boneName
            uint32_t boneIndex

            float3 translate
            float4 rotate
            float3 scale

            float voxelSize
            int32_t bound[Pos|Neg][X|Y|Z]

            uint32_t voxelCount
            voxelCount * {
                int32_t x, y, z
                uint8_t r, g, b
            }
        }
*/

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
        std::uint32_t boneIndex;

        float translateX, translateY, translateZ;
        float rotateX,    rotateY,    rotateZ,    rotateW;
        float scaleX,     scaleY,     scaleZ;

        float voxelSize;
        std::int32_t boundPosX, boundNegX;
        std::int32_t boundPosY, boundNegY;
        std::int32_t boundPosZ, boundNegZ;

        std::vector<Voxel> voxels;
    };

    VMEBindingContent(void) { Clear(); }

    void Clear(void);

    bool LoadFromFile(const std::string &filename);
    bool SaveToFile(const std::string &filename) const;

    bool IsAvailable(void) const;

    //根据time和size因子，重新从originSkeleton构造scaledSkeleton
    void RefreshScaledSkeleton(void);

    std::string bindingPath;

    float skeletonTimeFactor, skeletonSizeFactor;
    std::string skeletonPath;

    //time factor和size factor都设为1的骨骼
    Skeleton::Skeleton originSkeleton;

    //从originSkeleton根据time和size因子导出的骨骼
    //是模型真正使用的骨骼
    Skeleton::Skeleton scaledSkeleton;

    std::vector<Component> components;
};

class VMECore
{
public:
    VMEBindingContent bindingContent;
};
