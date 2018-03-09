/*================================================================
Filename: VoxelModelEditor.cpp
Date: 2018.3.5
Created by AirGuanZ
================================================================*/
#include <cassert>
#include <filesystem>
#include <map>

#include <Utility/HelperFunctions.h>

#include <Resource/ResourceNameManager.h>
#include <Screen/GUISystem.h>
#include <Window/Window.h>
#include "VoxelModelEditor.h"

namespace filesystem = std::experimental::filesystem::v1;

namespace
{
    class VMECmd_ExitClicked : public VoxelModelEditorCommand
    {
    public:
        void Execute(VoxelModelEditorCore &core, VMECmdQueue &cmdQueue)
        {
            core.mainLoopDone_ = true;
        }
    };

    class VMECmd_ReloadBindingNames : public VoxelModelEditorCommand
    {
    public:
        void Execute(VoxelModelEditorCore &core, VMECmdQueue &cmdQueue)
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

        void Execute(VoxelModelEditorCore &core, VMECmdQueue &cmdQueue)
        {
            core.selectedBindingNameIndex_ = selected_;
            core.needRefreshDisplay_ = true;
        }

    private:
        int selected_;
    };

    class VMWCmd_DeleteSelectedBinding : public VoxelModelEditorCommand
    {
    public:
        void Execute(VoxelModelEditorCore &core, VMECmdQueue &cmdQueue)
        {
            auto &rM = RscNameMgr::GetInstance();

            //合成binding文件名
            std::string filename = rM.AsString("VoxelModelEditor", "Binding") +
                                   core.bindingNames_[core.selectedBindingNameIndex_] +
                                   rM.AsString("VoxelModelEditor", "BindingExtension");
            filesystem::remove(filename);

            cmdQueue.push(new VMECmd_ReloadBindingNames());
        }
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

        ImGui::SameLine();
        if(ImGui::Button("Delete"))
            cmdQueue.push(new VMWCmd_DeleteSelectedBinding());
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
            cmd->Execute(*this, cmdQueue_);
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
