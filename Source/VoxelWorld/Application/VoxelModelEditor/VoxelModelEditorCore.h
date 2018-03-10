/*================================================================
Filename: VoxelModelEditorCore.h
Date: 2018.3.10
Created by AirGuanZ
================================================================*/
#pragma once

#include <deque>
#include <map>
#include <string>
#include <vector>

class VoxelModelEditorCore
{
public:
    VoxelModelEditorCore(void)
    {
        mainLoopDone_ = false;
        needRefreshDisplay_ = false;
    }

    int selectedBindingNameIndex_;
    std::vector<std::string> bindingNames_;

    std::map<std::string, std::string> componentToBone_;

    bool mainLoopDone_;
    bool needRefreshDisplay_;
};

class VoxelModelEditorCommand;
using VMECmdQueue = std::deque<VoxelModelEditorCommand*>;

class VoxelModelEditorCommand
{
public:
    virtual void Execute(VoxelModelEditorCore &core, VMECmdQueue &cmdQueue) = 0;
};
