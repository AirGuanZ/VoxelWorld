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

class VoxelModelEditorCommand
{
public:
    virtual void Execute(VoxelModelEditorCore &core) = 0;
};

class VoxelModelEditorView
{
public:
    void Render(void);

private:
    std::vector<const char*> bindingNames_;
};

class VoxelModelEditor : private VoxelModelEditorCore
{
public:
    AppState Run(void);

private:
    void RefreshDisplay(void);

private:
    VoxelModelEditorView view_;

    std::queue<VoxelModelEditorCommand*> cmdQueue_;
};
