/*================================================================
Filename: VoxelModelEditorDisplay.h
Date: 2018.3.10
Created by AirGuanZ
================================================================*/
#pragma once

#include <vector>

#include "VoxelModelEditorCommand.h"

class VoxelModelEditorDisplay
{
public:
    VoxelModelEditorDisplay(VMECmdQueue &cmdQueue);

    void Display(void);

private:
    void Frame(void);

private:
    VMECmdQueue &cmdQueue_;

    friend class VoxelModelEditor;

    std::vector<const char*> bindingNames_;
    int selectedBindingNameIndex_;
};
