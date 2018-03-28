/*================================================================
Filename: VMEView.cpp
Date: 2018.3.14
Created by AirGuanZ
================================================================*/
#include <Utility\FileSystem.h>

#include <Screen\GUISystem.h>
#include <Window\Window.h>
#include "VMECommand.h"
#include "VMECore.h"
#include "VMEView.h"

void VMEViewRefreshConfig::Closure(void)
{
    componentModel |= bindingFile;
}

VMEView::VMEView(void)
{
    showConsole_ = true;
    showBindingAttributes_ = true;
    showComponentView_ = true;
}

bool VMEView::Initialize(std::string &errMsg)
{
    if(!componentView_.Initialize(errMsg))
        return false;

    inputNewComponentName_.resize(256);
    inputNewComponentName_[0] = '\0';

    return true;
}

void VMEView::Display(VMEViewControl &ctrl, std::queue<VMECmd*> &cmds)
{
    Window &win = Window::GetInstance();

    GUI::NewFrame();
    win.ClearDepthStencil();
    win.ClearRenderTarget();

    DisplayMainMenuBar(ctrl, cmds);
    DisplayBindingAttributes(ctrl, cmds);
    DisplayComponentView(cmds);

    if(showConsole_)
        console_.Display();

    GUI::RenderImGui();

    win.Present();
    win.DoEvents();
}

void VMEView::Clear(void)
{
    showConsole_ = true;
    showBindingAttributes_ = true;
    showComponentView_ = true;

    console_.ClearInputBox();
    console_.ClearInputQueue();
    console_.ClearTexts();

    loadedBindingPath_ = "";
    skeletonPath_ = "";

    selectedBoneNameIndex_ = 0;
    skeletonBoneNames_.clear();

    inputNewComponentName_[0] = '\0';
}

void VMEView::Refresh(const VMEViewRefreshConfig &config, const VMECore &core)
{
    if(config.bindingFile)
    {
        loadedBindingPath_ = FileSystem::PathToFilename(core.bindingContent.bindingPath);
        skeletonPath_ = FileSystem::PathToFilename(core.bindingContent.skeletonPath);

        selectedBoneNameIndex_ = 0;
        skeletonBoneNames_.clear();
        for(auto &it : core.bindingContent.originSkeleton._getBoneMap())
            skeletonBoneNames_.push_back(it.first.c_str());
    }

    componentView_.Refresh(config, core);
}

bool VMEView::FetchConsoleInput(std::string &text)
{
    return console_.FetchInputText(text);
}

VMEConsole &VMEView::GetConsole(void)
{
    return console_;
}

void VMEView::DisplayMainMenuBar(VMEViewControl &ctrl, std::queue<VMECmd*> &cmds)
{
    if(ImGui::BeginMainMenuBar())
    {
        if(ImGui::BeginMenu("Editor"))
        {
            if(ImGui::MenuItem("Exit"))
                ctrl.exit = true;

            ImGui::EndMenu();
        }

        if(ImGui::BeginMenu("Binding"))
        {
            if(ImGui::MenuItem("New"))
                cmds.push(new VMECmd_NewEmptyBinding());

            if(ImGui::MenuItem("Load"))
                cmds.push(new VMECmd_LoadBinding());

            if(ImGui::MenuItem("Unload"))
                cmds.push(new VMECmd_UnloadBinding());

            if(ImGui::MenuItem("Save"))
                cmds.push(new VMECmd_SaveBinding());

            if(ImGui::MenuItem("Save As"))
                cmds.push(new VMECmd_SaveBindingAs());

            ImGui::EndMenu();
        }

        if(ImGui::BeginMenu("View"))
        {
            ImGui::MenuItem("Binding", nullptr, &showBindingAttributes_);
            ImGui::MenuItem("Console", nullptr, &showConsole_);
            ImGui::MenuItem("Preview", nullptr, &showComponentView_);

            ImGui::EndMenu();
        }

        ImGui::EndMainMenuBar();
    }
}

void VMEView::DisplayBindingAttributes(VMEViewControl &ctrl, std::queue<VMECmd*> &cmds)
{
    if(!showBindingAttributes_)
        return;

    if(loadedBindingPath_.empty())
    {
        if(ImGui::Begin("Binding##VoxelModelEditor", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
        {
            ImGui::Text("%s", "No binding loaded");
        }
        ImGui::End();
        return;
    }

    if(ImGui::Begin("Binding##VoxelModelEditor", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
    {
        ImGui::Text("Binding: %s", loadedBindingPath_.c_str());
        ImGui::Text("Skeletin: %s", skeletonPath_.c_str());

        /*if(ImGui::InputText("New Component", inputNewComponentName_.data(), inputNewComponentName_.size(),
                            ImGuiInputTextFlags_EnterReturnsTrue))
        {
            cmds.push(nullptr);
        }*/
        ImGui::Text("Select Bone Name:");
        ImGui::ListBox("", &selectedBoneNameIndex_,
                       skeletonBoneNames_.data(), skeletonBoneNames_.size(), 4);
    }
    ImGui::End();
}

void VMEView::DisplayComponentView(std::queue<VMECmd*> &cmds)
{
    if(showComponentView_)
        componentView_.Display(cmds, 16.66667f);
}
