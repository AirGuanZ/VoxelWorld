/*================================================================
Filename: VoxelModelEditor.cpp
Date: 2018.3.5
Created by AirGuanZ
================================================================*/
#include <filesystem>

#include <Input/InputManager.h>
#include <Resource/ResourceNameManager.h>
#include <Screen/GUISystem.h>
#include <Window/Window.h>
#include "VoxelModelEditor.h"

namespace filesystem = std::experimental::filesystem::v1;

AppState VoxelModelEditor::Run(void)
{
    Window &win         = Window::GetInstance();
    InputManager &input = InputManager::GetInstance();

    ID3D11Device *dev       = win.GetD3DDevice();
    ID3D11DeviceContext *DC = win.GetD3DDeviceContext();

    std::string errMsg;
    if(!Initialize(errMsg))
    {
        ShowErrMsgBox(errMsg);
        return AppState::MainMenu;
    }

    bool mainLoopDone_ = false;
    win.SetBackgroundColor(0.0f, 1.0f, 1.0f, 0.0f);

    while(!mainLoopDone_)
    {
        GUI::NewFrame();

        win.ClearDepthStencil();
        win.ClearRenderTarget();

        GUI::RenderImGui();

        win.Present();
        win.DoEvents();
    }

    return AppState::MainMenu;
}

bool VoxelModelEditor::Initialize(std::string &errMsg)
{
    if(!InitSkeletonPaths(errMsg))
        return false;
    if(!InitComponentPaths(errMsg))
        return false;
    if(!InitBindingPaths(errMsg))
        return false;

    return true;
}

bool VoxelModelEditor::InitSkeletonPaths(std::string &errMsg)
{
    return InitSpecFileList(
        skeletonPaths_, errMsg,
        RscNameMgr::GetInstance()("VoxelModelEditor", "Skeleton"),
        RscNameMgr::GetInstance()("VoxelModelEditor", "SkeletonExtension"));
}

bool VoxelModelEditor::InitComponentPaths(std::string &errMsg)
{
    return InitSpecFileList(
        componentPaths_, errMsg,
        RscNameMgr::GetInstance()("VoxelModelEditor", "Component"),
        RscNameMgr::GetInstance()("VoxelModelEditor", "ComponentExtension"));
}

bool VoxelModelEditor::InitBindingPaths(std::string &errMsg)
{
    return InitSpecFileList(
        bindingPaths_, errMsg,
        RscNameMgr::GetInstance()("VoxelModelEditor", "Binding"),
        RscNameMgr::GetInstance()("VoxelModelEditor", "BindingExtension"));
}

bool VoxelModelEditor::InitSpecFileList(SpecFilePaths &output, std::string &errMsg,
                                        const std::wstring &dirPath, const std::wstring &ext)
{
    output.clear();
    errMsg = "";

    if(dirPath.empty())
        return false;

    for(auto &entry : filesystem::directory_iterator(dirPath))
    {
        auto &path = entry.path();
        if(filesystem::is_regular_file(path) &&
           ((path.has_extension() && path.extension().wstring() == ext) ||
           (!path.has_extension() && ext.empty())))
        {
            output[path.stem().wstring()] = path.wstring();
        }
    }

    return true;
}
