/*================================================================
Filename: VoxelModelEditorCommandWindow.cpp
Date: 2018.3.13
Created by AirGuanZ
================================================================*/
#include <cassert>

#include <Screen/GUISystem.h>
#include "VoxelModelEditorCommandWindow.h"

VoxelModelEditorCommandWindow::VoxelModelEditorCommandWindow(void)
{
    maxTextCount_ = 20;
    inputBuf_.resize(256);
    inputBuf_[0] = '\0';
}

void VoxelModelEditorCommandWindow::SetMaxTextCount(size_t cnt)
{
    maxTextCount_ = cnt;
    DeleteRedundantText();
}

void VoxelModelEditorCommandWindow::Display(void)
{
    ImGui::SetNextWindowPos(ImVec2(300, 300), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(520, 300), ImGuiCond_FirstUseEver);
    if(!ImGui::Begin("Command##VoxelModelEditor"))
    {
        ImGui::End();
        return;
    }

    //留一个separator+一行text的空间给输入槽
    const float footer_height_to_reserve = ImGui::GetStyle().ItemSpacing.y
                                         + ImGui::GetFrameHeightWithSpacing();
    ImGui::BeginChild("ScrollingRegion", ImVec2(0, -footer_height_to_reserve),
        false, ImGuiWindowFlags_HorizontalScrollbar);

    for(auto &unit : texts_)
    {
        static const ImVec4 normalColor = { 1.0f, 1.0f, 1.0f, 1.0f };
        static const ImVec4 errorColor  = { 1.0f, 0.3f, 0.3f, 1.0f };

        switch(unit.type)
        {
        case TextType::Normal:
            ImGui::PushStyleColor(ImGuiCol_Text, normalColor);
            break;
        case TextType::Error:
            ImGui::PushStyleColor(ImGuiCol_Text, errorColor);
            break;
        default:
            std::abort();
        }

        ImGui::TextWrapped("%s", unit.text.data());
        ImGui::PopStyleColor();
    }

    ImGui::EndChild();

    ImGui::Separator();

    if(ImGui::InputText("Command", inputBuf_.data(), inputBuf_.size(),
                        ImGuiInputTextFlags_EnterReturnsTrue))
    {
        inputText_.push(inputBuf_.data());
        inputBuf_[0] = '\0';
    }

    ImGui::End();
}

void VoxelModelEditorCommandWindow::AddText(TextType type, const std::string &text)
{
    texts_.push_back(TextUnit{ type, text });
    DeleteRedundantText();
}

void VoxelModelEditorCommandWindow::Clear(void)
{
    texts_.clear();
}

bool VoxelModelEditorCommandWindow::FetchInputText(std::string &text)
{
    if(inputText_.empty())
        return false;
    text = std::move(inputText_.front());
    inputText_.pop();
    return true;
}

void VoxelModelEditorCommandWindow::DeleteRedundantText(void)
{
    while(texts_.size() > maxTextCount_)
        texts_.pop_front();
}
