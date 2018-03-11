/*================================================================
Filename: VoxelModelBinding.h
Date: 2018.3.10
Created by AirGuanZ
================================================================*/
#pragma once

#include <string>

#include "VoxelModelEditorDisplay.h"

class VoxelModelBinding
{
public:
    static void CreateEmptyBindingFile(const std::wstring &filename);

    const std::string &GetName(void) const;

    bool LoadFromFile(const std::wstring &filename);

    bool SaveToFile(const std::wstring &filename) const;

    void RefreshDisplay(VoxelModelBindingDisplay &display);

private:
    std::string name_;
};
