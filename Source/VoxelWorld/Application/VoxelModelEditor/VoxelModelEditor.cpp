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

namespace
{
    const std::string LIST_ITEM_NO_SKELETON = "No Skeleton";
    const std::string LIST_ITEM_NO_COMPONENT = "No Component";
}

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

        SelectionWindow();

        GUI::RenderImGui();

        win.Present();
        win.DoEvents();

        if(exitClicked_)
            mainLoopDone_ = true;
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

    if(!InitGUI(errMsg))
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
            output[path.stem().string()] = path.wstring();
        }
    }

    return true;
}

bool VoxelModelEditor::InitGUI(std::string &errMsg)
{
    for(auto &it : skeletonPaths_)
        skeletonNames_.push_back(it.first.data());
    if(skeletonNames_.empty())
        skeletonNames_.push_back(LIST_ITEM_NO_SKELETON.data());
    currentSkeletionIdx_ = 0;

    for(auto &it : componentPaths_)
        componentNames_.push_back(it.first.data());
    if(componentNames_.empty())
        componentNames_.push_back(LIST_ITEM_NO_COMPONENT.data());
    currentComponentIdx_ = 0;

    exitClicked_ = false;

    return true;
}

void VoxelModelEditor::SelectionWindow(void)
{
    if(ImGui::Begin("Object Selection",
       nullptr, ImGuiWindowFlags_AlwaysAutoResize))
    {
        ImGui::PushID(0);
        ImGui::Text("Skeleton List:");
        ImGui::ListBox("", &currentSkeletionIdx_,
                       skeletonNames_.data(), skeletonNames_.size(), 4);
        ImGui::PopID();

        ImGui::PushID(1);
        ImGui::Text("Component List:");
        ImGui::ListBox("", &currentComponentIdx_,
                       componentNames_.data(), componentNames_.size(), 4);
        ImGui::PopID();

        if(ImGui::Button("Exit"))
            exitClicked_ = true;
        ImGui::SameLine();
        if(ImGui::Button("Save"))
            exitClicked_ = true;
    }
    ImGui::End();
}
