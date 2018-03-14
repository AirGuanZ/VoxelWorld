/*================================================================
Filename: VoxelModelEditorCommandWindow.h
Date: 2018.3.13
Created by AirGuanZ
================================================================*/
#pragma once

#include <list>
#include <queue>
#include <string>

#include <Utility/Math.h>

#include "VoxelModelEditorCore.h"

struct VMECmdMsg
{
    Color color;
    std::string msg;

    static const Color ERROR_COLOR;
    static const Color NORMAL_COLOR;
};

using VMECmdMsgQueue = std::queue<VMECmdMsg>;

class VoxelModelEditorCommandWindow
{
public:
    VoxelModelEditorCommandWindow(void);

    void SetMaxTextCount(size_t cnt);

    void Display(void);

    void AddText(const std::string &text, const Color &color = VMECmdMsg::NORMAL_COLOR);

    void Clear(void);

    bool FetchInputText(std::string &text);

private:
    void DeleteRedundantText(void);

private:
    std::list<VMECmdMsg> texts_;
    size_t maxTextCount_;

    std::vector<char> inputBuf_;
    std::queue<std::string> inputText_;
};
