/*================================================================
Filename: VoxelModelEditorCore.h
Date: 2018.3.14
Created by AirGuanZ
================================================================*/
#pragma once

#include <cstdint>
#include <string>
#include <vector>

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

    std::string bindingName;
    std::string skeletonPath;

    //componentCount以uint32_t存储在这
    std::vector<Component> components;
};

class VMECore
{
public:
    VMECore(void);
    ~VMECore(void);

    void Clear();

    VMEBindingContent bindingContent;
};
