/*================================================================
Filename: VoxelModelEditor.h
Date: 2018.3.4
Created by AirGuanZ
================================================================*/
#pragma once

#include <queue>
#include <string>
#include <vector>

#include <Application/Common.h>

class VoxelModelEditorCore
{
public:
    bool Initialize(void);

    int selectedBindingNameIndex_;
    std::vector<std::string> bindingNames_;

    bool mainLoopDone_;
    bool needRefreshDisplay_;
};

class VoxelModelEditorCommand;
using VMECmdQueue = std::queue<VoxelModelEditorCommand*>;

class VoxelModelEditorCommand
{
public:
    virtual void Execute(VoxelModelEditorCore &core, VMECmdQueue &cmdQueue) = 0;
};

class VoxelModelEditorDisplay
{
public:
    void Display(std::queue<VoxelModelEditorCommand*> &cmdQueue);

private:
    friend class VoxelModelEditor;

    std::vector<const char*> bindingNames_;
    int selectedBindingNameIndex_;
};

class VoxelModelEditor : private VoxelModelEditorCore
{
public:
    AppState Run(void);

private:
    void RefreshDisplay(void);

private:
    VoxelModelEditorDisplay view_;

    std::queue<VoxelModelEditorCommand*> cmdQueue_;
};
