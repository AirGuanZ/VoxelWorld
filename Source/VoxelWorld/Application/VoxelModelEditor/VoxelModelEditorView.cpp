/*================================================================
Filename: VoxelModelEditorView.cpp
Date: 2018.3.14
Created by AirGuanZ
================================================================*/
#include <Utility\FileSystem.h>

#include <Screen\GUISystem.h>
#include <Window\Window.h>
#include "VoxelModelEditorCommand.h"
#include "VoxelModelEditorCore.h"
#include "VoxelModelEditorView.h"

VMEView::VMEView(void)
{
    showConsole_ = true;
    showBindingAttributes_ = true;
}

void VMEView::Display(VMEViewControl &ctrl, std::queue<VMECmd*> &cmds)
{
    Window &win = Window::GetInstance();

    GUI::NewFrame();
    win.ClearDepthStencil();
    win.ClearRenderTarget();

    DisplayMainMenuBar(ctrl, cmds);
    DisplayBindingAttributes(ctrl, cmds);

    if(showConsole_)
        console_.Display();

    GUI::RenderImGui();

    win.Present();
    win.DoEvents();
}

void VMEView::Clear(void)
{
    loadedBindingPath_ = "";
}

void VMEView::Refresh(const VMEViewRefreshConfig &config, const VMECore &core)
{
    if(config.all)
    {
        loadedBindingPath_ = FileSystem::PathToFilename(core.bindingContent.bindingPath);
    }
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
        if(ImGui::BeginMenu("File"))
        {
            if(ImGui::MenuItem("Load"))
                cmds.push(new VMECmd_LoadBinding());

            if(ImGui::MenuItem("Unload"))
                cmds.push(new VMECmd_UnloadBinding());

            if(ImGui::MenuItem("Exit"))
                ctrl.exit = true;

            ImGui::EndMenu();
        }

        if(ImGui::BeginMenu("View"))
        {
            ImGui::MenuItem("Binding", nullptr, &showBindingAttributes_);
            ImGui::MenuItem("Console", nullptr, &showConsole_);

            ImGui::EndMenu();
        }

        ImGui::EndMainMenuBar();
    }
}

void VMEView::DisplayBindingAttributes(VMEViewControl &ctrl, std::queue<VMECmd*> &cmds)
{
    if(loadedBindingPath_.empty() || !showBindingAttributes_)
        return;

    if(ImGui::Begin("Binding##VoxelModelEditor", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
    {
        ImGui::Text("Binding name: %s", loadedBindingPath_.c_str());
    }
    ImGui::End();
}
