/*================================================================
Filename: VoxelModelEditor.cpp
Date: 2018.3.5
Created by AirGuanZ
================================================================*/
#include <cassert>
#include <map>

#include <Utility/HelperFunctions.h>

#include <Resource/ResourceNameManager.h>
#include <Screen/GUISystem.h>
#include <Window/Window.h>
#include "VoxelModelEditor.h"

namespace
{
    class VMECmd_ExitClicked : public VoxelModelEditorCommand
    {
    public:
        void Execute(VoxelModelEditorCore &core)
        {
            core.mainLoopDone_ = true;
        }
    };

    class VMECmd_ReloadBindingNames : public VoxelModelEditorCommand
    {
    public:
        void Execute(VoxelModelEditorCore &core)
        {
            core.bindingNames_.clear();
            core.selectedBindingNameIndex_ = 0;

            auto &rM = RscNameMgr::GetInstance();
            std::map<std::string, std::wstring> filenames;
            Helper::GetFilenames(
                rM.AsString("VoxelModelEditor", "Binding"),
                rM.AsString("VoxelModelEditor", "BindingExtension"),
                filenames);
            for(auto &it : filenames)
                core.bindingNames_.push_back(it.first);

            core.needRefreshDisplay_ = true;
        }
    };

    class VMWCmd_SelectBindingName : public VoxelModelEditorCommand
    {
    public:
        VMWCmd_SelectBindingName(int selected)
            : selected_(selected)
        {

        }

        void Execute(VoxelModelEditorCore &core)
        {
            core.selectedBindingNameIndex_ = selected_;
            core.needRefreshDisplay_ = true;
        }

    private:
        int selected_;
    };
}

bool VoxelModelEditorCore::Initialize(void)
{
    mainLoopDone_ = false;
    needRefreshDisplay_ = false;

    return true;
}

void VoxelModelEditorDisplay::Display(std::queue<VoxelModelEditorCommand*> &cmdQueue)
{
    Window &win = Window::GetInstance();

    GUI::NewFrame();
    win.ClearDepthStencil();
    win.ClearRenderTarget();
    
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
                cmdQueue.push(new VMWCmd_SelectBindingName(selected));
        }
        else
            ImGui::Text("No Binding");

        if(ImGui::Button("Exit"))
            cmdQueue.push(new VMECmd_ExitClicked());
    }
    ImGui::End();

    GUI::RenderImGui();

    win.Present();
    win.DoEvents();
}

AppState VoxelModelEditor::Run(void)
{
    if(!VoxelModelEditorCore::Initialize())
        return AppState::MainMenu;

    cmdQueue_.push(new VMECmd_ReloadBindingNames());
    while(!mainLoopDone_)
    {
        view_.Display(cmdQueue_);

        while(cmdQueue_.size())
        {
            auto *cmd = cmdQueue_.front();
            cmdQueue_.pop();
            cmd->Execute(*this);
            Helper::SafeDeleteObjects(cmd);
        }

        if(needRefreshDisplay_)
            RefreshDisplay();
    }

    return AppState::MainMenu;
}

void VoxelModelEditor::RefreshDisplay(void)
{
    view_.bindingNames_.clear();
    for(auto &it : bindingNames_)
        view_.bindingNames_.push_back(it.data());
    view_.selectedBindingNameIndex_ = selectedBindingNameIndex_;
}
