/*================================================================
Filename: VoxelModelEditorCommandWindow.h
Date: 2018.3.13
Created by AirGuanZ
================================================================*/
#pragma once

#include <list>
#include <queue>
#include <string>

class VoxelModelEditorCommandWindow
{
public:
    enum class TextType
    {
        Normal,
        Error
    };

    struct TextUnit
    {
        TextType type;
        std::string text;
    };

    VoxelModelEditorCommandWindow(void);

    void SetMaxTextCount(size_t cnt);

    void Display(void);

    void AddText(TextType type, const std::string &text);

    void Clear(void);

    bool FetchInputText(std::string &text);

private:
    void DeleteRedundantText(void);

private:
    std::list<TextUnit> texts_;
    size_t maxTextCount_;

    std::vector<char> inputBuf_;
    std::queue<std::string> inputText_;
};
