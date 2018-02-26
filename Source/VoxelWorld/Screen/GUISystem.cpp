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

#include "nuklear/nuklear.h"
#include "nuklear/nuklear_d3d11.h"

namespace
{
    constexpr int NK_MAX_VTX_BUF = 512 * 1024;
    constexpr int NK_MAX_IDX_BUF = 128 * 1024;

    nk_context *nkCtx = nullptr;
}

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

#ifdef GUI_SYSTEM_NK
    nkCtx = nk_d3d11_init(window.GetD3DDevice(),
                          window.GetClientWidth(), window.GetClientHeight(),
                          NK_MAX_VTX_BUF, NK_MAX_IDX_BUF);
    if(!nkCtx)
    {
        ImGui_ImplDX11_Shutdown();
        return false;
    }

    {
        nk_font_atlas *atlas;
        nk_d3d11_font_stash_begin(&atlas);
        nk_d3d11_font_stash_end();
    }
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

#ifdef GUI_SYSTEM_NK
        nk_d3d11_shutdown();
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

#ifdef GUI_SYSTEM_NK
    nk_d3d11_render(
        Window::GetInstance().GetD3DDeviceContext(),
        NK_ANTI_ALIASING_ON);
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

#ifdef GUI_SYSTEM_NK
nk_context *GUISystem::GetNKContext(void)
{
    return nkCtx;
}
#endif

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

void GUISystem::MouseButtonDown(MouseButton button, LPARAM lParam)
{
#ifdef GUI_SYSTEM_NK
    switch(button)
    {
    case MouseButton::Left:
        nk_input_button(nkCtx, NK_BUTTON_LEFT, static_cast<short>(LOWORD(lParam)), static_cast<short>(HIWORD(lParam)), 1);
        break;
    case MouseButton::Middle:
        nk_input_button(nkCtx, NK_BUTTON_MIDDLE, static_cast<short>(LOWORD(lParam)), static_cast<short>(HIWORD(lParam)), 1);
        break;
    case MouseButton::Right:
        nk_input_button(nkCtx, NK_BUTTON_RIGHT, static_cast<short>(LOWORD(lParam)), static_cast<short>(HIWORD(lParam)), 1);
        break;
    }
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

void GUISystem::MouseButtonUp(MouseButton button, LPARAM lParam)
{
#ifdef GUI_SYSTEM_NK
    switch(button)
    {
    case MouseButton::Left:
        nk_input_button(nkCtx, NK_BUTTON_DOUBLE, static_cast<short>(LOWORD(lParam)), static_cast<short>(HIWORD(lParam)), 0);
        nk_input_button(nkCtx, NK_BUTTON_LEFT, static_cast<short>(LOWORD(lParam)), static_cast<short>(HIWORD(lParam)), 0);
        break;
    case MouseButton::Middle:
        nk_input_button(nkCtx, NK_BUTTON_MIDDLE, static_cast<short>(LOWORD(lParam)), static_cast<short>(HIWORD(lParam)), 0);
        break;
    case MouseButton::Right:
        nk_input_button(nkCtx, NK_BUTTON_RIGHT, static_cast<short>(LOWORD(lParam)), static_cast<short>(HIWORD(lParam)), 0);
        break;
    }
#endif
}

void GUISystem::MouseWheel(int wheel)
{
#ifdef GUI_SYSTEM_IG
    ImGui::GetIO().MouseWheel += static_cast<float>(wheel);
#endif

#ifdef GUI_SYSTEM_NK
    nk_input_scroll(nkCtx, nk_vec2(0.0f, static_cast<float>(wheel)));
#endif
}

void GUISystem::MouseMove(int posX, int posY)
{
#ifdef GUI_SYSTEM_IG
    ImGuiIO &io = ImGui::GetIO();
    io.MousePos.x = static_cast<float>(posX);
    io.MousePos.y = static_cast<float>(posY);
#endif

#ifdef GUI_SYSTEM_NK
    nk_input_motion(nkCtx, posX, posY);
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

void GUISystem::KeyDownUp(WPARAM wParam, LPARAM lParam)
{
#ifdef GUI_SYSTEM_NK
    int down = !((lParam >> 31) & 1);
    int ctrl = GetKeyState(VK_CONTROL) & (1 << 15);

    switch(wParam)
    {
    case VK_SHIFT:
    case VK_LSHIFT:
    case VK_RSHIFT:
        nk_input_key(&d3d11.ctx, NK_KEY_SHIFT, down);
        break;

    case VK_DELETE:
        nk_input_key(&d3d11.ctx, NK_KEY_DEL, down);
        break;

    case VK_RETURN:
        nk_input_key(&d3d11.ctx, NK_KEY_ENTER, down);
        break;

    case VK_TAB:
        nk_input_key(&d3d11.ctx, NK_KEY_TAB, down);
        break;

    case VK_LEFT:
        if(ctrl)
            nk_input_key(&d3d11.ctx, NK_KEY_TEXT_WORD_LEFT, down);
        else
            nk_input_key(&d3d11.ctx, NK_KEY_LEFT, down);
        break;

    case VK_RIGHT:
        if(ctrl)
            nk_input_key(&d3d11.ctx, NK_KEY_TEXT_WORD_RIGHT, down);
        else
            nk_input_key(&d3d11.ctx, NK_KEY_RIGHT, down);
        break;

    case VK_BACK:
        nk_input_key(&d3d11.ctx, NK_KEY_BACKSPACE, down);
        break;

    case VK_HOME:
        nk_input_key(&d3d11.ctx, NK_KEY_TEXT_START, down);
        nk_input_key(&d3d11.ctx, NK_KEY_SCROLL_START, down);
        break;

    case VK_END:
        nk_input_key(&d3d11.ctx, NK_KEY_TEXT_END, down);
        nk_input_key(&d3d11.ctx, NK_KEY_SCROLL_END, down);
        break;

    case VK_NEXT:
        nk_input_key(&d3d11.ctx, NK_KEY_SCROLL_DOWN, down);
        break;

    case VK_PRIOR:
        nk_input_key(&d3d11.ctx, NK_KEY_SCROLL_UP, down);
        break;

    case 'C':
        if(ctrl)
            nk_input_key(&d3d11.ctx, NK_KEY_COPY, down);
        break;

    case 'V':
        if(ctrl)
            nk_input_key(&d3d11.ctx, NK_KEY_PASTE, down);
        break;

    case 'X':
        if(ctrl)
            nk_input_key(&d3d11.ctx, NK_KEY_CUT, down);
        break;

    case 'Z':
        if(ctrl)
            nk_input_key(&d3d11.ctx, NK_KEY_TEXT_UNDO, down);
        break;

    case 'R':
        if(ctrl)
            nk_input_key(&d3d11.ctx, NK_KEY_TEXT_REDO, down);
        break;
    }
#endif
}

void GUISystem::Char(unsigned int ch)
{
#ifdef GUI_SYSTEM_IG
    if(0 < ch && ch < 0x10000)
        ImGui::GetIO().AddInputCharacter(static_cast<ImWchar>(ch));
#endif

#ifdef GUI_SYSTEM_NK
    if(ch >= 32)
        nk_input_unicode(nkCtx, static_cast<nk_rune>(ch));
#endif
}
