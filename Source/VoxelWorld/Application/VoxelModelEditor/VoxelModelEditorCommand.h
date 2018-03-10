/*================================================================
Filename: VoxelModelEditorCommand.h
Date: 2018.3.10
Created by AirGuanZ
================================================================*/
#pragma once

#include <deque>

#include "VoxelModelEditorCore.h"

class VMECmd_ExitClicked : public VoxelModelEditorCommand
{
public:
    void Execute(VoxelModelEditorCore &core, VMECmdQueue &cmdQueue);
};

class VMECmd_ReloadBindingNames : public VoxelModelEditorCommand
{
public:
    void Execute(VoxelModelEditorCore &core, VMECmdQueue &cmdQueue);
};

class VMWCmd_SelectBindingName : public VoxelModelEditorCommand
{
public:
    VMWCmd_SelectBindingName(int selected);

    void Execute(VoxelModelEditorCore &core, VMECmdQueue &cmdQueue);

private:
    int selected_;
};

class VMWCmd_DeleteSelectedBinding : public VoxelModelEditorCommand
{
public:
    void Execute(VoxelModelEditorCore &core, VMECmdQueue &cmdQueue);
};
