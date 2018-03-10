/*================================================================
Filename: VoxelModelEditorDisplay.cpp
Date: 2018.3.10
Created by AirGuanZ
================================================================*/
#include <Screen/GUISystem.h>
#include <Window/Window.h>

#include "VoxelModelEditorDisplay.h"

VoxelModelEditorDisplay::VoxelModelEditorDisplay(VMECmdQueue &cmdQueue)
    : cmdQueue_(cmdQueue)
{

}

void VoxelModelEditorDisplay::Display(void)
{
    Window &win = Window::GetInstance();

    GUI::NewFrame();
    win.ClearDepthStencil();
    win.ClearRenderTarget();

    Frame();

    GUI::RenderImGui();

    win.Present();
    win.DoEvents();
}

void VoxelModelEditorDisplay::Frame(void)
{
    if(ImGui::Begin("Object Select", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
    {
        ImGui::Text("Select Binding Name:");
        if(bindingNames_.size())
        {
            assert(0 <= selectedBindingNameIndex_ &&
                selectedBindingNameIndex_ < static_cast<int>(bindingNames_.size()));
            int selected = selectedBindingNameIndex_;
            ImGui::ListBox("", &selected,
                bindingNames_.data(), bindingNames_.size(), 4);
            if(selected != selectedBindingNameIndex_)
                cmdQueue_.push_back(new VMWCmd_SelectBindingName(selected));
        }
        else
            ImGui::Text("No Binding");

        if(ImGui::Button("Exit"))
            cmdQueue_.push_back(new VMECmd_ExitClicked());

        ImGui::SameLine();
        if(ImGui::Button("Refresh"))
            cmdQueue_.push_back(new VMECmd_ReloadBindingNames());

        ImGui::SameLine();
        if(ImGui::Button("Delete"))
            cmdQueue_.push_back(new VMWCmd_DeleteSelectedBinding());
    }
    ImGui::End();
}
