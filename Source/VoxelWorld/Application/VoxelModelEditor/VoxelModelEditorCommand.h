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
    void Execute(VoxelModelEditorCore &core);
};

class VMECmd_ReloadBindingNames : public VoxelModelEditorCommand
{
public:
    void Execute(VoxelModelEditorCore &core);
};

class VMWCmd_SelectBindingName : public VoxelModelEditorCommand
{
public:
    VMWCmd_SelectBindingName(int selected);

    void Execute(VoxelModelEditorCore &core);

private:
    int selected_;
};

class VMWCmd_DeleteSelectedBinding : public VoxelModelEditorCommand
{
public:
    void Execute(VoxelModelEditorCore &core);
};

class VMWCmd_LoadSelectedBinding : public VoxelModelEditorCommand
{
public:
    void Execute(VoxelModelEditorCore &core);
};

class VMWCmd_UnloadBinding : public VoxelModelEditorCommand
{
public:
    void Execute(VoxelModelEditorCore &core);
};

class VMWCmd_CreateBinding : public VoxelModelEditorCommand
{
public:
    VMWCmd_CreateBinding(const std::string &name);

    void Execute(VoxelModelEditorCore &core);

private:
    std::string name_;
};
