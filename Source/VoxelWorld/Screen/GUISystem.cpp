/*================================================================
Filename: GUISystem.cpp
Date: 2018.2.23
Created by AirGuanZ
================================================================*/
#include <cassert>
#include <cstring>
#include <memory>

#include <imgui.h>
#include <Utility/D3D11Header.h>

#include "../Window/Window.h"
#include "GUISystem.h"
#include "imgui_impl_dx11.h"

SINGLETON_CLASS_DEFINITION(GUISystem);

bool GUISystem::Initialize(std::string &errMsg)
{
    Window &window = Window::GetInstance();

    if(!ImGui_ImplDX11_Init(window.GetWindowHandle(),
                            window.GetD3DDevice(),
                            window.GetD3DDeviceContext()))
        return false;

    ImGui::StyleColorsClassic();
    return true;
}

void GUISystem::Destroy(void)
{
    ImGui_ImplDX11_Shutdown();
}

void GUISystem::NewFrame(void)
{
    ImGui_ImplDX11_NewFrame();
}

void GUISystem::Render(void)
{
    ImGui::Render();
}

void GUISystem::MouseButtonDown(MouseButton button)
{
    int butIdx = 0;
    switch(button)
    {
    case MouseButton::Left:
        butIdx = 0;
        break;
    case MouseButton::Middle:
        butIdx = 1;
        break;
    case MouseButton::Right:
        butIdx = 2;
        break;
    }
    ImGui::GetIO().MouseDown[butIdx] = true;
}

void GUISystem::MouseButtonUp(MouseButton button)
{
    int butIdx = 0;
    switch(button)
    {
    case MouseButton::Left:
        butIdx = 0;
        break;
    case MouseButton::Middle:
        butIdx = 1;
        break;
    case MouseButton::Right:
        butIdx = 2;
        break;
    }
    ImGui::GetIO().MouseDown[butIdx] = false;
}

void GUISystem::MouseWheel(int wheel)
{
    ImGui::GetIO().MouseWheel += static_cast<float>(wheel);
}

void GUISystem::MouseMove(int posX, int posY)
{
    ImGuiIO &io = ImGui::GetIO();
    io.MousePos.x = static_cast<float>(posX);
    io.MousePos.y = static_cast<float>(posY);
}

void GUISystem::KeyDown(int VK)
{
    if(0 <= VK && VK < 256)
        ImGui::GetIO().KeysDown[VK] = true;
}

void GUISystem::KeyUp(int VK)
{
    if(0 <= VK && VK < 256)
        ImGui::GetIO().KeysDown[VK] = false;
}

void GUISystem::Char(unsigned int ch)
{
    ImGui::GetIO().AddInputCharacter(static_cast<ImWchar>(ch));
}
