/*================================================================
Filename: VoxelModelEditorConsole.cpp
Date: 2018.3.14
Created by AirGuanZ
================================================================*/
#include <cassert>

#include <Screen\GUISystem.h>
#include "VoxelModelEditorConsole.h"

VMEConsole::VMEConsole(int inputBufSize, int maxTextCount)
{
    assert(maxTextCount >= 0 && inputBufSize > 1);

    maxTextCount_ = maxTextCount;
    inputBuf_.resize(inputBufSize);
    inputBuf_[0] = '\0';

    scrollToBottom_ = true;
}

void VMEConsole::Display(void)
{
    ImGui::SetNextWindowPos(ImVec2(300, 300), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(600, 300), ImGuiCond_FirstUseEver);

    if(!ImGui::Begin("Console##VoxelModelEditor"))
    {
        ImGui::End();
        return;
    }

    //留一个separator+一行text的空间给输入槽
    const float footer_height_to_reserve = ImGui::GetStyle().ItemSpacing.y
                                         + ImGui::GetFrameHeightWithSpacing();
    ImGui::BeginChild("ScrollingRegion", ImVec2(0, -footer_height_to_reserve),
                      false, ImGuiWindowFlags_HorizontalScrollbar);
    
    for(auto &t : texts_)
    {
        static const ImVec4 TEXT_COLOR[] =
        {
            { 1.0f, 1.0f, 1.0f, 1.0f }, //Normal
            { 1.0f, 0.4f, 0.4f, 1.0f }, //Error
        };
        ImGui::PushStyleColor(ImGuiCol_Text, TEXT_COLOR[static_cast<int>(t.type)]);
        ImGui::TextWrapped("%s", (">> " + t.text).c_str());
        ImGui::PopStyleColor();
    }

    if(scrollToBottom_)
    {
        ImGui::SetScrollHere();
        scrollToBottom_ = false;
    }

    ImGui::EndChild();

    ImGui::Separator();

    bool inputTextEntered = ImGui::InputText("Input", inputBuf_.data(), inputBuf_.size(),
                                             ImGuiInputTextFlags_EnterReturnsTrue);
    ImGui::SameLine();
    inputTextEntered |= ImGui::Button("Enter");

    if(inputTextEntered && inputBuf_[0] != '\0')
    {
        inputTexts_.push(inputBuf_.data());
        inputBuf_[0] = '\0';
    }

    ImGui::SameLine();
    if(ImGui::Button("Clear"))
        texts_.clear();

    ImGui::End();
}

void VMEConsole::SetMaxTextCount(int count)
{
    assert(count >= 0);
    maxTextCount_ = count;
    DeleteRedundantTexts();
}

void VMEConsole::AddText(VMEConsoleText::Type type, const std::string &text)
{
    texts_.push_back({ type, text });
    DeleteRedundantTexts();
    scrollToBottom_ = true;
}

void VMEConsole::ClearTexts(void)
{
    texts_.clear();
}

void VMEConsole::ClearInputBox(void)
{
    inputBuf_[0] = '\0';
}

void VMEConsole::ClearInputQueue(void)
{
    while(inputTexts_.size())
        inputTexts_.pop();
}

bool VMEConsole::FetchInputText(std::string &text)
{
    if(inputTexts_.size())
    {
        text = std::move(inputTexts_.front());
        inputTexts_.pop();
        return true;
    }
    return false;
}

void VMEConsole::DeleteRedundantTexts(void)
{
    while(texts_.size() > static_cast<size_t>(maxTextCount_))
        texts_.pop_front();
}
