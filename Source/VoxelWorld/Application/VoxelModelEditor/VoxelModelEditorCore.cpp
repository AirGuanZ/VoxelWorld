/*================================================================
Filename: VoxelModelEditorCore.cpp
Date: 2018.3.14
Created by AirGuanZ
================================================================*/
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
    return true;
}

bool VMEBindingContent::SaveToFile(const std::string &filename)
{
    return true;
}

bool VMEBindingContent::IsAvailable(void) const
{
    return bindingPath.size() != 0;
}
