/*================================================================
Filename: VoxelModelBinding.h
Date: 2018.3.10
Created by AirGuanZ
================================================================*/
#pragma once

#include <cstdint>
#include <string>

#include <Utility/Math.h>

class VoxelModelBindingDisplay;

class BindingContent
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

    void Clear(void)
    {
        skeletonPath = "";
        components.clear();
    }

    std::string skeletonPath;

    //componentCount以uint32_t存储在这
    std::vector<Component> components;
};

class VoxelModelBinding
{
public:
    static bool CreateEmptyBindingFile(const std::wstring &filename);

    void Clear(void);

    const std::string &GetName(void) const;

    bool LoadFromFile(const std::wstring &filename);

    bool SaveToFile(const std::wstring &filename) const;

    void RefreshDisplay(VoxelModelBindingDisplay &display);

private:
    std::string name_;
    BindingContent content_;
};
