/*================================================================
Filename: VoxelModelEditorView.h
Date: 2018.3.14
Created by AirGuanZ
================================================================*/
#pragma once

#include <queue>
#include <string>
#include <vector>

#include <D3DObject\BasicBuffer.h>
#include "VoxelModelEditorCommand.h"
#include "VoxelModelEditorConsole.h"

class VMECore;

struct VMEViewRefreshConfig
{
    bool all = false;
};

struct VMEViewControl
{
    bool exit = false;
    bool selectedComponentModel = false;
    bool allComponentsModel = false;
};

class VMEView
{
public:
    VMEView(void);

    void Display(VMEViewControl &ctrl, std::queue<VMECmd*> &cmds);

    void Clear(void);
    void Refresh(const VMEViewRefreshConfig &config, const VMECore &core);

    bool FetchConsoleInput(std::string &text);

    VMEConsole &GetConsole(void);

private:
    void DisplayMainMenuBar(VMEViewControl &ctrl, std::queue<VMECmd*> &cmds);
    void DisplayBindingAttributes(VMEViewControl &ctrl, std::queue<VMECmd*> &cmds);

private:
    bool showConsole_;
    bool showBindingAttributes_;

    VMEConsole console_;

    std::string loadedBindingPath_;
    std::string skeletonPath_;
};
