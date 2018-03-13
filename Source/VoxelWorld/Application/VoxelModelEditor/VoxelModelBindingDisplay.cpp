/*================================================================
Filename: VoxelModelBindingDisplay.cpp
Date: 2018.3.13
Created by AirGuanZ
================================================================*/
#include <Screen/GUISystem.h>
#include "VoxelModelBindingDisplay.h"

void VoxelModelBindingDisplay::Clear(void)
{
    bindingName_ = "";
}

void VoxelModelBindingDisplay::Display(void)
{
    if(bindingName_.empty())
        return;
    ImGui::Separator();
    ImGui::Text("Selected Binding: %s", bindingName_.c_str());
}
