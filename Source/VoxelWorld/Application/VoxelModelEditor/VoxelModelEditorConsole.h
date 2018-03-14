/*================================================================
Filename: VoxelModelEditorConsole.h
Date: 2018.3.14
Created by AirGuanZ
================================================================*/
#pragma once

#include <list>
#include <queue>
#include <string>
#include <vector>

#include <Utility\Math.h>

struct VMEConsoleText
{
    enum Type
    {
        Normal,
        Error
    };
    Type type = Normal;
    std::string text;
};

class VMEConsole
{
public:
    VMEConsole(int inputBufSize = 256, int maxTextCount = 10);

    void Display(void);

    void SetMaxTextCount(int count);

    void AddText(VMEConsoleText::Type type, const std::string &text);

    void ClearTexts(void);
    void ClearInputBox(void);
    void ClearInputQueue(void);

    bool FetchInputText(std::string &text);

private:
    void DeleteRedundantTexts(void);

private:
    int maxTextCount_;
    std::list<VMEConsoleText> texts_;

    std::vector<char> inputBuf_;
    std::queue<std::string> inputTexts_;

    bool scrollToBottom_;
};
