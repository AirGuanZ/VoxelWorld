/*================================================================
Filename: GUISystem.cpp
Date: 2018.2.23
Created by AirGuanZ
================================================================*/
#define _CRT_SECURE_NO_WARNINGS

#include <cassert>
#include <cstring>
#include <memory>

#include <imgui.h>
#include <Utility/D3D11Header.h>

#include <Window/Window.h>
#include "GUISystem.h"

#define GUI_SYSTEM_IG
#define GUI_SYSTEM_NK

SINGLETON_CLASS_DEFINITION(GUISystem);

#ifdef GUI_SYSTEM_IG

#include "imgui/imgui_impl_dx11.h"

#endif

#ifdef GUI_SYSTEM_NK

#define NK_INCLUDE_FIXED_TYPES
#define NK_INCLUDE_STANDARD_IO
#define NK_INCLUDE_STANDARD_VARARGS
#define NK_INCLUDE_DEFAULT_ALLOCATOR
#define NK_INCLUDE_VERTEX_BUFFER_OUTPUT
#define NK_INCLUDE_FONT_BAKING
#define NK_INCLUDE_DEFAULT_FONT
#define NK_IMPLEMENTATION
#define NK_D3D11_IMPLEMENTATION

#include <nuklear.h>

#include "nuklear/nuklear_d3d11.h"

namespace
{
    constexpr int NK_MAX_VTX_BUF = 512 * 1024;
    constexpr int NK_MAX_IDX_BUF = 128 * 1024;

    nk_context *nkCtx = nullptr;
}

#endif

bool GUISystem::Initialize(std::string &errMsg)
{
    Window &window = Window::GetInstance();

#ifdef GUI_SYSTEM_IG
    if(!ImGui_ImplDX11_Init(window.GetWindowHandle(),
                            window.GetD3DDevice(),
                            window.GetD3DDeviceContext()))
        return false;

    ImGui::StyleColorsClassic();
#endif

#ifdef GUI_SYSTEM_NK
    nkCtx = nk_d3d11_init(window.GetD3DDevice(),
                          window.GetClientWidth(), window.GetClientHeight(),
                          NK_MAX_VTX_BUF, NK_MAX_IDX_BUF);
    {
        nk_font_atlas *atlas;
        nk_d3d11_font_stash_begin(&atlas);
        nk_d3d11_font_stash_end();
    }
#endif

    return true;
}

void GUISystem::Destroy(void)
{
#ifdef GUI_SYSTEM_IG
    ImGui_ImplDX11_Shutdown();
#endif

#ifdef GUI_SYSTEM_NK
    nk_d3d11_shutdown();
#endif
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

#ifdef GUI_SYSTEM_NK
    nk_d3d11_render(
        Window::GetInstance().GetD3DDeviceContext(),
        NK_ANTI_ALIASING_ON);
#endif
}

void GUISystem::BeginInput(void)
{
#ifdef GUI_SYSTEM_NK
    nk_input_begin(nkCtx);
#endif
}

void GUISystem::EndInput(void)
{
#ifdef GUI_SYSTEM_NK
    nk_input_end(nkCtx);
#endif
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
    ImGui::GetIO().AddInputCharacter(static_cast<ImWchar>(ch));
#endif
}
