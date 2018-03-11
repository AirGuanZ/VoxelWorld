/*================================================================
Filename: VoxelModelBinding.cpp
Date: 2018.3.10
Created by AirGuanZ
================================================================*/
#include <filesystem>
#include <fstream>

#include "VoxelModelBinding.h"

namespace filesystem = std::experimental::filesystem::v1;

void VoxelModelBinding::CreateEmptyBindingFile(const std::wstring &filename)
{
    std::ofstream fout(filename, std::ios_base::out | std::ios_base::trunc);
}

const std::string &VoxelModelBinding::GetName(void) const
{
    return name_;
}

bool VoxelModelBinding::LoadFromFile(const std::wstring &filename)
{
    name_ = filesystem::path(filename).stem().string();
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
