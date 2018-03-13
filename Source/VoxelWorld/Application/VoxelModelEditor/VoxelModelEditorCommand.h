/*================================================================
Filename: VoxelModelEditorCommand.h
Date: 2018.3.10
Created by AirGuanZ
================================================================*/
#pragma once

#include <deque>

#include "VoxelModelEditorCommandWindow.h"

class VoxelModelEditorCore;
class VoxelModelEditorCommand;

using VMECmdQueue = std::deque<VoxelModelEditorCommand*>;

class VoxelModelEditorCommand
{
public:
    virtual void Execute(VoxelModelEditorCore &core, VMECmdMsgQueue &cmdMsgs) = 0;
};

class VMECmd_ExitClicked : public VoxelModelEditorCommand
{
public:
    void Execute(VoxelModelEditorCore &core, VMECmdMsgQueue &cmdMsgs);
};

class VMECmd_ReloadBindingNames : public VoxelModelEditorCommand
{
public:
    VMECmd_ReloadBindingNames(bool showMsg = true);

    void Execute(VoxelModelEditorCore &core, VMECmdMsgQueue &cmdMsgs);

private:
    bool showMsg_;
};

class VMWCmd_SelectBindingName : public VoxelModelEditorCommand
{
public:
    VMWCmd_SelectBindingName(int selected);

    void Execute(VoxelModelEditorCore &core, VMECmdMsgQueue &cmdMsgs);

private:
    int selected_;
};

class VMWCmd_DeleteSelectedBinding : public VoxelModelEditorCommand
{
public:
    void Execute(VoxelModelEditorCore &core, VMECmdMsgQueue &cmdMsgs);
};

class VMWCmd_LoadSelectedBinding : public VoxelModelEditorCommand
{
public:
    void Execute(VoxelModelEditorCore &core, VMECmdMsgQueue &cmdMsgs);
};

class VMWCmd_UnloadBinding : public VoxelModelEditorCommand
{
public:
    VMWCmd_UnloadBinding(bool showMsg = true);

    void Execute(VoxelModelEditorCore &core, VMECmdMsgQueue &cmdMsgs);

private:
    bool showMsg_;
};

class VMWCmd_CreateBinding : public VoxelModelEditorCommand
{
public:
    VMWCmd_CreateBinding(const std::string &name);

    void Execute(VoxelModelEditorCore &core, VMECmdMsgQueue &cmdMsgs);

private:
    std::string name_;
};
