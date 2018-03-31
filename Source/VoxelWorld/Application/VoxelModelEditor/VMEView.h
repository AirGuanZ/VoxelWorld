/*================================================================
Filename: VMEView.h
Date: 2018.3.14
Created by AirGuanZ
================================================================*/
#pragma once

#include <queue>
#include <string>
#include <vector>

#include <D3DObject\BasicBuffer.h>
#include "VMECommand.h"
#include "VMEComponentView.h"
#include "VMEConsole.h"

class VMECore;

struct VMEViewRefreshConfig
{
    void Closure(void);

    bool bindingFile = false;    //加载/卸载binding的时候
    bool componentList = false;  //添加/删除component的时候
    bool componentModel = false; //修改component模型的时候
};

struct VMEViewControl
{
    bool exit = false;
};

class VMEView
{
public:
    VMEView(void);

    bool Initialize(std::string &errMsg);

    void Display(VMEViewControl &ctrl, std::queue<VMECmd*> &cmds);

    void Clear(void);
    void Refresh(const VMEViewRefreshConfig &config, const VMECore &core);

    bool FetchConsoleInput(std::string &text);

    VMEConsole &GetConsole(void);

private:
    void DisplayMainMenuBar(VMEViewControl &ctrl, std::queue<VMECmd*> &cmds);
    void DisplayBindingAttributes(VMEViewControl &ctrl, std::queue<VMECmd*> &cmds);
    void DisplayComponentView(std::queue<VMECmd*> &cmds);

private:
    bool showConsole_;
    bool showBindingAttributes_;
    bool showComponentView_;

    VMEConsole console_;

    std::string loadedBindingPath_;
    std::string skeletonPath_;

    int selectedBoneNameIndex_;
    std::vector<const char *> skeletonBoneNames_;

    int selectedAniNameIndex_;
    std::vector<const char *> skeletonAniNames_;

    std::vector<char> inputNewComponentName_;

    VMEComponentView componentView_;
};
