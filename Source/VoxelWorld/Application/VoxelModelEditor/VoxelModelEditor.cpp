/*================================================================
Filename: VoxelModelEditor.cpp
Date: 2018.3.5
Created by AirGuanZ
================================================================*/
#include <filesystem>

#include <Resource/ResourceNameManager.h>
#include <Screen/GUISystem.h>
#include <Utility/HelperFunctions.h>
#include <VoxelModel/ModelSkeletonBinding.h>
#include <Window/Window.h>

#include "VoxelModelEditor.h"

namespace
{
    const std::string &LIST_ITEM_NO_BINDING_FILE = "No Binding";
}

AppState VoxelModelEditor::Run(void)
{
    Window &win = Window::GetInstance();

    std::string errMsg;
    if(!Initialize(errMsg))
    {
        ShowErrMsgBox(errMsg);
        return AppState::MainMenu;
    }

    bool done = false;
    while(!done)
    {
        GUI::NewFrame();
        win.ClearDepthStencil();
        win.ClearRenderTarget();

        ShowBindingWindow();
        ShowErrMsgWindow();

        GUI::RenderImGui();

        win.Present();
        win.DoEvents();

        if(exit_)
            done = true;
    }

    return AppState::MainMenu;
}

bool VoxelModelEditor::Initialize(std::string &errMsg)
{
    errMsg = "";

    winState_.showBindingWindow = true;
    winState_.useBindingWindow = true;

    winState_.showErrMsgWindow = false;
    winState_.useErrMsgWindow = true;
    
    winState_.errMsg = "";

    selectedBinding_ = 0;

    exit_ = false;

    ReloadBindingPaths();

    return true;
}

bool VoxelModelEditor::HasBinding(void) const
{
    if(bindingNames_[selectedBinding_] == LIST_ITEM_NO_BINDING_FILE.data())
        return false;
    return true;
}

void VoxelModelEditor::ReloadBindingPaths(void)
{
    RscNameMgr &rM = RscNameMgr::GetInstance();

    Helper::GetFilenames(rM.AsString("VoxelModelEditor", "Binding"),
                         rM.AsString("VoxelModelEditor", "BindingExtension"),
                         bindingPaths_);

    bindingNames_.clear();
    for(auto &it : bindingPaths_)
        bindingNames_.push_back(it.first.data());
    if(bindingNames_.empty())
        bindingNames_.push_back(LIST_ITEM_NO_BINDING_FILE.data());

    SetSelectedBinding(0);
}

void VoxelModelEditor::SetSelectedBinding(int idx)
{
    assert(0 <= idx && idx < static_cast<int>(bindingNames_.size()));
    selectedBinding_ = idx;

    if(!HasBinding())
    {
        skeletonName_ = "";
        skeletonPath_ = L"";
        return;
    }

    ModelSkeletonBinding bindingFile;
    if(!bindingFile.LoadFromFile(bindingPaths_[bindingNames_[idx]]))
    {
        EnableErrorWindow("Failed to open binding file");
        exit_ = true;
    }

    skeletonName_ = bindingFile.GetSkeletonName();
    skeletonPath_ = bindingFile.GetSkeletonPath();
}

void VoxelModelEditor::ShowBindingWindow(void)
{
    if(!winState_.showBindingWindow)
        return;

    ImGuiWindowFlags winFlag = ImGuiWindowFlags_AlwaysAutoResize;
    if(!winState_.useBindingWindow)
        winFlag |= ImGuiWindowFlags_NoInputs;
    if(ImGui::Begin("Object Selection", nullptr, winFlag))
    {
        ImGui::Text("Bindings:");
        int oldSelectedBinding = selectedBinding_;
        ImGui::ListBox("", &selectedBinding_, bindingNames_.data(),
                       bindingNames_.size(), 4);
        if(oldSelectedBinding != selectedBinding_)
            SetSelectedBinding(selectedBinding_);

        ImGui::Text("Skeleton: %s", skeletonName_.c_str());

        if(ImGui::Button("Delete Selected Binding") && HasBinding())
        {
            std::experimental::filesystem::v1::remove(
                bindingPaths_[bindingNames_[selectedBinding_]]);
            ReloadBindingPaths();
        }

        if(ImGui::Button("Exit"))
            exit_ = true;
        ImGui::SameLine();
        if(ImGui::Button("Error"))
            EnableErrorWindow("Test Error Message");
    }
    ImGui::End();
}

void VoxelModelEditor::ShowErrMsgWindow(void)
{
    if(!winState_.showErrMsgWindow)
        return;

    ImGuiWindowFlags winFlag = ImGuiWindowFlags_AlwaysAutoResize |
                               ImGuiWindowFlags_NoCollapse;
    if(!winState_.useErrMsgWindow)
        winFlag |= ImGuiWindowFlags_NoInputs;
    ImGui::SetNextWindowPosCenter(ImGuiCond_Always);
    if(ImGui::Begin("Error", nullptr, winFlag))
    {
        ImGui::Text("%s", winState_.errMsg.c_str());
        if(ImGui::Button("  OK  "))
            PopWindowState();
    }
    ImGui::End();
}

void VoxelModelEditor::PushWindowState(void)
{
    winStateStack_.push(winState_);
}

void VoxelModelEditor::PopWindowState(void)
{
    if(winStateStack_.empty())
        return;
    winState_ = winStateStack_.top();
    winStateStack_.pop();
}

void VoxelModelEditor::EnableErrorWindow(const std::string &errMsg)
{
    PushWindowState();

    winState_.useBindingWindow = false;

    winState_.showErrMsgWindow = true;
    winState_.useErrMsgWindow = true;

    winState_.errMsg = errMsg;
}
