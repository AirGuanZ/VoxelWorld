/*================================================================
Filename: VoxelModelEditorView.cpp
Date: 2018.3.14
Created by AirGuanZ
================================================================*/
#include <Screen\GUISystem.h>
#include <Window\Window.h>
#include "VoxelModelEditorCore.h"
#include "VoxelModelEditorView.h"

void VMEView::Display(VMEViewControl &ctrl, std::queue<VMECmd*> &cmds)
{
    Window &win = Window::GetInstance();

    GUI::NewFrame();
    win.ClearDepthStencil();
    win.ClearRenderTarget();

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
        loadedBindingPath_ = core.bindingContent.bindingPath;
    }
}

bool VMEView::FetchConsoleInput(std::string &text)
{
    return console_.FetchInputText(text);
}

VMEConsole & VMEView::GetConsole(void)
{
    return console_;
}
