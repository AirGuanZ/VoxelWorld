/*================================================================
Filename: VoxelModelBinding.cpp
Date: 2018.3.10
Created by AirGuanZ
================================================================*/
#include <filesystem>
#include <fstream>
#include <istream>

#include "VoxelModelBinding.h"

namespace filesystem = std::experimental::filesystem::v1;

namespace
{
    template<typename T>
    inline bool ReadBinary(std::istream &in, T &data)
    {
        in.read(reinterpret_cast<char*>(&data), sizeof(T));
        return !!in;
    }

    bool ReadString(std::istream &in, std::string &str)
    {
        str = "";
        while(true)
        {
            if(in.eof())
                return false;
            char ch;
            if(!ReadBinary(in, ch))
                return false;
            if(ch == '\0')
                return true;
            str += ch;
        }
    }

    bool ReadVoxel(std::istream &in, BindingContent::Component::Voxel &vol)
    {
        return ReadBinary(in, vol.x) && ReadBinary(in, vol.y) && ReadBinary(in, vol.z)
            && ReadBinary(in, vol.r) && ReadBinary(in, vol.g) && ReadBinary(in, vol.b);
    }

    bool ReadComponent(std::istream &in, BindingContent::Component &cpt)
    {
        if(!ReadString(in, cpt.boneName))
            return false;

        if(!ReadBinary(in, cpt.translateX) ||
           !ReadBinary(in, cpt.translateY) ||
           !ReadBinary(in, cpt.translateZ))
            return false;

        if(!ReadBinary(in, cpt.rotateX) ||
           !ReadBinary(in, cpt.rotateY) ||
           !ReadBinary(in, cpt.rotateZ) ||
           !ReadBinary(in, cpt.rotateW))
            return false;

        if(!ReadBinary(in, cpt.scaleX) ||
           !ReadBinary(in, cpt.scaleY) ||
           !ReadBinary(in, cpt.scaleZ))
            return false;

        if(!ReadBinary(in, cpt.voxelSize))
            return false;

        if(!ReadBinary(in, cpt.boundPosX) ||
           !ReadBinary(in, cpt.boundNegX) ||
           !ReadBinary(in, cpt.boundPosY) ||
           !ReadBinary(in, cpt.boundNegY) ||
           !ReadBinary(in, cpt.boundPosZ) ||
           !ReadBinary(in, cpt.boundNegZ))
            return false;

        std::uint32_t voxelCount;
        if(!ReadBinary(in, voxelCount))
            return false;

        cpt.voxels.resize(voxelCount);
        for(auto &vol : cpt.voxels)
        {
            if(!ReadVoxel(in, vol))
                return false;
        }

        return true;
    }

    bool ReadBinding(std::istream &in, BindingContent &binding)
    {
        std::uint32_t componentCount;
        binding.Clear();

        if(!ReadString(in, binding.skeletonPath))
            goto FAILED;

        if(!ReadBinary(in, componentCount))
            goto FAILED;

        binding.components.resize(componentCount);
        for(auto &cpt : binding.components)
        {
            if(!ReadComponent(in, cpt))
                goto FAILED;
        }

        return true;

    FAILED:
        binding.Clear();
        return false;
    }

    template<typename T>
    inline bool WriteBinary(std::ostream &out, T &data)
    {
        out.write(reinterpret_cast<char*>(&data), sizeof(T));
        return !!out;
    }

    bool WriteString(std::ostream &out, const std::string &str)
    {
        out.write(str.data(), str.size() + 1);
        return !!out;
    }

    bool WriteBinding(std::ostream &out, BindingContent &content)
    {
        if(!WriteString(out, content.skeletonPath))
            return false;

        std::uint32_t componentCount = static_cast<std::uint32_t>(content.components.size());
        if(!WriteBinary(out, componentCount))
            return false;

        for(auto &cpt : content.components)
        {
            if(!WriteString(out, cpt.boneName))
                return false;

            if(!WriteBinary(out, cpt.translateX) ||
               !WriteBinary(out, cpt.translateY) ||
               !WriteBinary(out, cpt.translateZ) ||
               !WriteBinary(out, cpt.rotateX) ||
               !WriteBinary(out, cpt.rotateY) ||
               !WriteBinary(out, cpt.rotateZ) ||
               !WriteBinary(out, cpt.rotateW) ||
               !WriteBinary(out, cpt.scaleX) ||
               !WriteBinary(out, cpt.scaleY) ||
               !WriteBinary(out, cpt.scaleZ))
                return false;

            if(!WriteBinary(out, cpt.voxelSize))
                return false;

            if(!WriteBinary(out, cpt.boundPosX) ||
               !WriteBinary(out, cpt.boundNegX) ||
               !WriteBinary(out, cpt.boundPosY) ||
               !WriteBinary(out, cpt.boundNegY) ||
               !WriteBinary(out, cpt.boundPosZ) ||
               !WriteBinary(out, cpt.boundNegZ))
                return false;

            std::uint32_t voxelCount = static_cast<std::uint32_t>(cpt.voxels.size());
            if(!WriteBinary(out, voxelCount))
                return false;

            for(auto &vol : cpt.voxels)
            {
                if(!WriteBinary(out, vol.x) ||
                   !WriteBinary(out, vol.y) ||
                   !WriteBinary(out, vol.z) ||
                   !WriteBinary(out, vol.r) ||
                   !WriteBinary(out, vol.g) ||
                   !WriteBinary(out, vol.b))
                    return false;
            }
        }

        return true;
    }
}

bool VoxelModelBinding::CreateEmptyBindingFile(const std::wstring &filename)
{
    BindingContent content;
    content.Clear();
    std::ofstream fout(filename, std::ios_base::out |
                                 std::ios_base::trunc |
                                 std::ios_base::binary);
    return WriteBinding(fout, content);
}

void VoxelModelBinding::Clear(void)
{
    name_ = "";
    content_.Clear();
}

const std::string &VoxelModelBinding::GetName(void) const
{
    return name_;
}

bool VoxelModelBinding::LoadFromFile(const std::wstring &filename)
{
    name_ = filesystem::path(filename).stem().string();

    std::ifstream fin(filename, std::ios_base::in | std::ios_base::binary);
    if(!fin || !ReadBinding(fin, content_))
        return false;

    return true;
}

bool VoxelModelBinding::SaveToFile(const std::wstring &filename) const
{
    return false;
}

void VoxelModelBinding::RefreshDisplay(VoxelModelBindingDisplay &display)
{
    display.bindingName_ = name_;
}
