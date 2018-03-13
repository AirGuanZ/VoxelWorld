/*================================================================
Filename: VoxelModelEditorCommandWindow.cpp
Date: 2018.3.13
Created by AirGuanZ
================================================================*/
#include <cassert>

#include <Screen/GUISystem.h>
#include "VoxelModelEditorCommandWindow.h"

const Color VMECmdMsg::ERROR_COLOR  = Color(1.0f, 0.4f, 0.4f, 1.0f);
const Color VMECmdMsg::NORMAL_COLOR = Color(1.0f, 1.0f, 1.0f, 1.0f);

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
    ImGui::SetNextWindowSize(ImVec2(600, 300), ImGuiCond_FirstUseEver);
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
        ImGui::PushStyleColor(ImGuiCol_Text,
            ImVec4(unit.color.R(), unit.color.G(), unit.color.B(), unit.color.A()));

        ImGui::TextWrapped("%s", ("> " + unit.msg).data());
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

    ImGui::SameLine();
    
    if(ImGui::SmallButton("Clear"))
        texts_.clear();

    ImGui::End();
}

void VoxelModelEditorCommandWindow::AddText(const std::string &text, const Color &color)
{
    texts_.push_back(VMECmdMsg{ color, text });
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
