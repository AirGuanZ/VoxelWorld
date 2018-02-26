/*================================================================
Filename: GUISystem.cpp
Date: 2018.2.23
Created by AirGuanZ
================================================================*/
#define _CRT_SECURE_NO_WARNINGS

#include <cassert>
#include <cstring>
#include <memory>
#include <iostream>

#include <Window/Window.h>
#include "GUISystem.h"

SINGLETON_CLASS_DEFINITION(GUISystem);

#ifdef GUI_SYSTEM_IG

#include "imgui/imgui.h"
#include "imgui/imgui_impl_dx11.h"

#endif

namespace
{
    bool GUISystemInited = false;

#ifdef GUI_SYSTEM_IG
    std::vector<ImFont*> imGuiFonts;
#endif
}

bool GUISystem::Initialize(const std::vector<FontSpecifier> &ttfFonts, std::string &errMsg)
{
    Window &window = Window::GetInstance();

#ifdef GUI_SYSTEM_IG
    if(!ImGui_ImplDX11_Init(window.GetWindowHandle(),
                            window.GetD3DDevice(),
                            window.GetD3DDeviceContext()))
        return false;

    ImGui::StyleColorsClassic();

    ImGuiIO &io = ImGui::GetIO();
    imGuiFonts.resize(ttfFonts.size() + 1);
    imGuiFonts[0] = io.Fonts->AddFontDefault();
    for(size_t i = 0; i < ttfFonts.size(); ++i)
        imGuiFonts[i + 1] = io.Fonts->AddFontFromFileTTF(ttfFonts[i].ttfFilename.c_str(), ttfFonts[i].pixelSize);
#endif

    GUISystemInited = true;

    return true;
}

void GUISystem::Destroy(void)
{
    if(GUISystemInited)
    {
#ifdef GUI_SYSTEM_IG
        ImGui_ImplDX11_Shutdown();
#endif

        GUISystemInited = false;
    }
}

void GUISystem::NewFrame(void)
{
#ifdef GUI_SYSTEM_IG
    ImGui_ImplDX11_NewFrame();
#endif
}

void GUISystem::Render(void)
{
#ifdef GUI_SYSTEM_IG
    ImGui::Render();
#endif
}

#ifdef GUI_SYSTEM_IG
void GUISystem::PushFont(int idx)
{
    ImGui::PushFont(imGuiFonts[idx + 1]);
}

void GUISystem::PopFont(void)
{
    ImGui::PopFont();
}
#endif

void GUISystem::BeginInput(void)
{

}

void GUISystem::EndInput(void)
{

}

void GUISystem::MouseButtonDown(MouseButton button)
{
#ifdef GUI_SYSTEM_IG
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
#endif
}

void GUISystem::MouseButtonUp(MouseButton button)
{
#ifdef GUI_SYSTEM_IG
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
#endif
}

void GUISystem::MouseWheel(int wheel)
{
#ifdef GUI_SYSTEM_IG
    ImGui::GetIO().MouseWheel += static_cast<float>(wheel);
#endif
}

void GUISystem::MouseMove(int posX, int posY)
{
#ifdef GUI_SYSTEM_IG
    ImGuiIO &io = ImGui::GetIO();
    io.MousePos.x = static_cast<float>(posX);
    io.MousePos.y = static_cast<float>(posY);
#endif
}

void GUISystem::KeyDown(int VK)
{
#ifdef GUI_SYSTEM_IG
    if(0 <= VK && VK < 256)
        ImGui::GetIO().KeysDown[VK] = true;
#endif
}

void GUISystem::KeyUp(int VK)
{
#ifdef GUI_SYSTEM_IG
    if(0 <= VK && VK < 256)
        ImGui::GetIO().KeysDown[VK] = false;
#endif
}

void GUISystem::Char(unsigned int ch)
{
#ifdef GUI_SYSTEM_IG
    if(0 < ch && ch < 0x10000)
        ImGui::GetIO().AddInputCharacter(static_cast<ImWchar>(ch));
#endif
}
