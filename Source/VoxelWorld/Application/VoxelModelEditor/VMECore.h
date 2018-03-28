/*================================================================
Filename: VMECore.h
Date: 2018.3.14
Created by AirGuanZ
================================================================*/
#pragma once

#include <cstdint>
#include <string>
#include <unordered_map>
#include <vector>

#include <SkeletonAnimation\SkeletonData.h>

/*
    VMEBindingContent�ļ���ʽ��
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
        struct VoxelData
        {
            std::int32_t x, y, z;
            std::uint8_t r, g, b;
        };

        std::string componentName;
        std::string boneName;

        //������洢��ÿ�ζ���skeleton�л�ȡ
        std::int32_t boneIndex;

        float translateX = 0.0f, translateY = 0.0f, translateZ = 0.0f;
        float rotateX = 0.0f,    rotateY = 0.0f,    rotateZ = 0.0f,    rotateW = 0.0f;
        float scaleX = 0.0f,     scaleY = 0.0f,     scaleZ = 0.0f;

        float voxelSize = 1.0f;
        std::int32_t boundPosX = 0, boundNegX = 0;
        std::int32_t boundPosY = 0, boundNegY = 0;
        std::int32_t boundPosZ = 0, boundNegZ = 0;

        std::unordered_map<IntVector3, IntVector3, IntVector3Hasher> voxels;

        //����ָ��������ģ���Ƿ���Ҫ����
        mutable bool meshNeedUpdating = true;
    };

    VMEBindingContent(void) { Clear(); }

    void Clear(void);

    bool LoadFromFile(const std::string &filename);
    bool SaveToFile(const std::string &filename) const;

    bool IsAvailable(void) const;

    //����time��size���ӣ����´�originSkeleton����scaledSkeleton
    void RefreshScaledSkeleton(void);

    std::string bindingPath;

    float skeletonTimeFactor, skeletonSizeFactor;
    std::string skeletonPath;

    //time factor��size factor����Ϊ1�Ĺ���
    Skeleton::Skeleton originSkeleton;

    //��originSkeleton����time��size���ӵ����Ĺ���
    //��ģ������ʹ�õĹ���
    Skeleton::Skeleton scaledSkeleton;

    std::vector<Component> components;
};

class VMECore
{
public:
    VMEBindingContent bindingContent;
};
