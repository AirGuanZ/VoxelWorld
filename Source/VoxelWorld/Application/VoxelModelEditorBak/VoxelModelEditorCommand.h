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

class VMECmd_SelectBindingName : public VoxelModelEditorCommand
{
public:
    VMECmd_SelectBindingName(int selected);

    void Execute(VoxelModelEditorCore &core, VMECmdMsgQueue &cmdMsgs);

private:
    int selected_;
};

class VMECmd_DeleteSelectedBinding : public VoxelModelEditorCommand
{
public:
    void Execute(VoxelModelEditorCore &core, VMECmdMsgQueue &cmdMsgs);
};

class VMECmd_LoadSelectedBinding : public VoxelModelEditorCommand
{
public:
    void Execute(VoxelModelEditorCore &core, VMECmdMsgQueue &cmdMsgs);
};

class VMECmd_UnloadBinding : public VoxelModelEditorCommand
{
public:
    VMECmd_UnloadBinding(bool showMsg = true);

    void Execute(VoxelModelEditorCore &core, VMECmdMsgQueue &cmdMsgs);

private:
    bool showMsg_;
};

class VMECmd_CreateBinding : public VoxelModelEditorCommand
{
public:
    VMECmd_CreateBinding(const std::string &name);

    void Execute(VoxelModelEditorCore &core, VMECmdMsgQueue &cmdMsgs);

private:
    std::string name_;
};

class VMECmd_SaveLoadedBinding : public VoxelModelEditorCommand
{
public:
    void Execute(VoxelModelEditorCore &core, VMECmdMsgQueue &cmdMsgs);
};
