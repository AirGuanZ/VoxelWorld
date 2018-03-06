/*================================================================
Filename: VoxelModelEditor.cpp
Date: 2018.3.5
Created by AirGuanZ
================================================================*/
#include <filesystem>
#include <fstream>

#include <Input/InputManager.h>
#include <Resource/ResourceNameManager.h>
#include <Screen/GUISystem.h>
#include <VoxelModel/ModelSkeletonBinding.h>
#include <Window/Window.h>
#include "VoxelModelEditor.h"

namespace filesystem = std::experimental::filesystem::v1;

namespace
{
    const std::string LIST_ITEM_NO_SKELETON = "No Skeleton";
    const std::string LIST_ITEM_NO_COMPONENT = "No Component";
    const std::string LIST_ITEM_NO_BINDING = "No Binding";
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

        switch(state_)
        {
        case State::Normal:
            SelectionWindow();
            break;
        case State::AddNewBinding:
            AddNewBindingWindow();
            break;
        }

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

    state_ = State::Normal;

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
    for(auto &it : bindingPaths_)
        bindingNames_.push_back(it.first.data());
    if(bindingNames_.empty())
        bindingNames_.push_back(LIST_ITEM_NO_BINDING.data());
    currentBindingIdx_ = 0;

    ReloadBinding();

    exitClicked_ = false;

    return true;
}

void VoxelModelEditor::SelectionWindow(void)
{
    if(ImGui::Begin("Object Selection##VoxelModelEditor",
       nullptr, ImGuiWindowFlags_AlwaysAutoResize))
    {
        ImGui::PushID(0);
        ImGui::Text("Binding List:");
        ImGui::ListBox("", &currentBindingIdx_,
                       bindingNames_.data(), bindingNames_.size(), 4);
        ImGui::SameLine();
        if(ImGui::Button("+"))
            state_ = State::AddNewBinding;
        ImGui::PopID();

        if(!NoBindingSelected())
        {
            ImGui::PushID(1);
            ImGui::Text("Component List:");
            ImGui::ListBox("", &currentComponentIdx_,
                           componentsOfSelectedBinding_.data(),
                           componentsOfSelectedBinding_.size(), 4);
            ImGui::PopID();
        }

        if(ImGui::Button("Exit"))
            exitClicked_ = true;
        ImGui::SameLine();
        if(ImGui::Button("Save"))
            exitClicked_ = true;
    }
    ImGui::End();
}

void VoxelModelEditor::AddNewBindingWindow(void)
{
    static std::vector<char> textBuf(40);
    if(ImGui::Begin("Add New Binding##VoxelModelEditor", nullptr,
                    ImGuiWindowFlags_NoCollapse))
    {
        if(ImGui::InputText("Name", textBuf.data(), textBuf.size(),
                            ImGuiInputTextFlags_EnterReturnsTrue))
        {
            state_ = State::Normal;
            CreateNewBinding(std::string(textBuf.data()));
            ReloadBinding();
        }
    }
    ImGui::End();
}

bool VoxelModelEditor::NoBindingSelected(void)
{
    return bindingNames_[currentBindingIdx_] == LIST_ITEM_NO_BINDING.data();
}

void VoxelModelEditor::CreateNewBinding(const std::string &name)
{
    if(name.empty())
        return;
    std::ofstream fout(
        RscNameMgr::GetInstance().AsString("VoxelModelEditor", "Binding") + name +
        RscNameMgr::GetInstance().AsString("VoxelModelEditor", "BindingExtension"),
        std::ios_base::out | std::ios_base::trunc);
}

void VoxelModelEditor::ReloadBinding(void)
{
    if(!NoBindingSelected())
    {
        auto pathIt = bindingPaths_.find(bindingNames_[currentBindingIdx_]);
        if(pathIt == bindingPaths_.end())
            return;
        ModelSkeletonBinding oldBinding = currentBinding_;
        if(!currentBinding_.LoadFromFile(pathIt->second))
        {
            currentBinding_ = oldBinding;
            return;
        }

        componentsOfSelectedBinding_.clear();
        currentBinding_.ForAllComponents([&](const std::string &name)
        {
            componentsOfSelectedBinding_.push_back(name.data());
        });
        if(componentsOfSelectedBinding_.empty())
            componentsOfSelectedBinding_.push_back(LIST_ITEM_NO_COMPONENT.data());
        currentComponentIdx_ = 0;
    }
    else
    {
        componentsOfSelectedBinding_.clear();
        componentsOfSelectedBinding_.push_back(LIST_ITEM_NO_COMPONENT.data());
        currentComponentIdx_ = 0;
    }
}
