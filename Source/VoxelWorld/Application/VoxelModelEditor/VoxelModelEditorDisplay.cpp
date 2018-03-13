/*================================================================
Filename: VoxelModelEditorDisplay.cpp
Date: 2018.3.10
Created by AirGuanZ
================================================================*/
#include <Screen/GUISystem.h>
#include <Window/Window.h>

#include "VoxelModelBinding.h"
#include "VoxelModelEditorDisplay.h"

VoxelModelEditorDisplay::VoxelModelEditorDisplay(VMECmdQueue &cmdQueue)
    : cmdQueue_(cmdQueue)
{
    newBindingNameBuf_.resize(32);
    newBindingNameBuf_[0] = '\0';
}

void VoxelModelEditorDisplay::Display(void)
{
    Window &win = Window::GetInstance();

    GUI::NewFrame();

    win.SetBackgroundColor(0.3f, 0.3f, 0.3f, 0.0f);
    win.ClearDepthStencil();
    win.ClearRenderTarget();

    Frame();

    GUI::RenderImGui();

    win.Present();
    win.DoEvents();
}

void VoxelModelEditorDisplay::Frame(void)
{
    if(ImGui::BeginMainMenuBar())
    {
        if(ImGui::BeginMenu("System"))
        {
            if(ImGui::MenuItem("Exit"))
                cmdQueue_.push_back(new VMECmd_ExitClicked());

            if(ImGui::MenuItem("Save"))
                cmdQueue_.push_back(new VMECmd_SaveLoadedBinding());

            ImGui::EndMenu();
        }

        ImGui::EndMainMenuBar();
    }

    if(ImGui::Begin("Select Binding##VoxelModelEditor", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
    {
        ImGui::Text("Select Binding Name:");
        if(bindingNames_.size())
        {
            assert(0 <= selectedBindingNameIndex_ &&
                selectedBindingNameIndex_ < static_cast<int>(bindingNames_.size()));
            int selected = selectedBindingNameIndex_;
            ImGui::ListBox("", &selected,
                bindingNames_.data(), bindingNames_.size(), 5);
            if(selected != selectedBindingNameIndex_)
                cmdQueue_.push_back(new VMECmd_SelectBindingName(selected));
        }
        else
            ImGui::Text("No Binding");

        if(ImGui::SmallButton("Load"))
            cmdQueue_.push_back(new VMECmd_LoadSelectedBinding());

        ImGui::SameLine();
        if(ImGui::SmallButton("Unload"))
            cmdQueue_.push_back(new VMECmd_UnloadBinding());

        ImGui::SameLine();
        if(ImGui::SmallButton("Delete"))
            cmdQueue_.push_back(new VMECmd_DeleteSelectedBinding());

        ImGui::SameLine();
        if(ImGui::SmallButton("New") && newBindingNameBuf_[0] != '\0')
        {
            cmdQueue_.push_back(new VMECmd_CreateBinding(std::string(newBindingNameBuf_.data())));
            newBindingNameBuf_[0] = '\0';
        }

        ImGui::InputText("", newBindingNameBuf_.data(), newBindingNameBuf_.size());

        bindingDisplay_.Display();
    }
    ImGui::End();

    cmdWinDisplay_.Display();
}
