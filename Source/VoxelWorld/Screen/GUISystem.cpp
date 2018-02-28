/*================================================================
Filename: GUI.cpp
Date: 2018.2.23
Created by AirGuanZ
================================================================*/
#define _CRT_SECURE_NO_WARNINGS

#include <algorithm>
#include <cassert>
#include <cstring>
#include <memory>

#include <Resource/ResourceNameManager.h>
#include <Window/Window.h>
#include "GUISystem.h"

#ifdef GUI_CE

#include <CEGUI/CEGUI.h>
#include <CEGUI/RendererModules/Direct3D11/Renderer.h>

#endif

SINGLETON_CLASS_DEFINITION(GUI);

#ifdef GUI_IG

#include "imgui/imgui.h"
#include "imgui/imgui_impl_dx11.h"

#endif

namespace
{
    bool GUISystemInited = false;
    int clientWidth = 100, clientHeight = 100;

#ifdef GUI_IG

    std::vector<ImFont*> imGuiFonts;

#endif

#ifdef GUI_CE

    CEGUI::Direct3D11Renderer::Renderer *ceguiRenderer = nullptr;

#endif
}

#ifdef GUI_CE

static bool InitCEGUI(void)
{
    Window &window = Window::GetInstance();
    RscNameMgr &rM = RscNameMgr::GetInstance();

    ceguiRenderer = &CEGUI::Direct3D11Renderer::bootstrapSystem(
        window.GetD3DDevice(), window.GetD3DDeviceContext());

    //设置默认资源路径

    std::string defaultRscDir = rM.AsString(u8"CEGUI", u8"DefaultResourceDirectory");

    CEGUI::DefaultResourceProvider *rp = reinterpret_cast<CEGUI::DefaultResourceProvider*>
        (CEGUI::System::getSingleton().getResourceProvider());
    assert(rp != nullptr);

    rp->setResourceGroupDirectory(
        u8"imagesets",   defaultRscDir + u8"/imagesets/");
    rp->setResourceGroupDirectory(
        u8"schemes",     defaultRscDir + u8"/schemes/");
    rp->setResourceGroupDirectory(
        u8"fonts",       defaultRscDir + u8"/fonts/");
    rp->setResourceGroupDirectory(
        u8"layouts",     defaultRscDir + u8"/layouts/");
    rp->setResourceGroupDirectory(
        u8"looknfeel",   defaultRscDir + u8"/looknfeel/");
    rp->setResourceGroupDirectory(
        u8"lua_scripts", defaultRscDir + u8"/lua_scripts/");
    
    CEGUI::ImageManager::setImagesetDefaultResourceGroup("imagesets");
    CEGUI::Scheme::setDefaultResourceGroup("schemes");
    CEGUI::Font::setDefaultResourceGroup("fonts");
    CEGUI::WidgetLookManager::setDefaultResourceGroup("looknfeels");
    CEGUI::WindowManager::setDefaultResourceGroup("layouts");
    CEGUI::ScriptModule::setDefaultResourceGroup("lua_scripts");

    return true;
}

#endif

bool GUI::Initialize(const std::vector<FontSpecifier> &ttfFonts, std::string &errMsg)
{
    Window &window = Window::GetInstance();

    clientWidth  = window.GetClientWidth();
    clientHeight = window.GetClientHeight();

#ifdef GUI_IG

    if(!ImGui_ImplDX11_Init(window.GetWindowHandle(),
                            window.GetD3DDevice(),
                            window.GetD3DDeviceContext()))
        return false;

    ImGui::StyleColorsClassic();

    //ImGui字体设置
    ImGuiIO &io = ImGui::GetIO();
    imGuiFonts.resize(ttfFonts.size() + 1);
    imGuiFonts[0] = io.Fonts->AddFontDefault();
    for(size_t i = 0; i < ttfFonts.size(); ++i)
    {
        imGuiFonts[i + 1] = io.Fonts->AddFontFromFileTTF(
            ttfFonts[i].ttfFilename.c_str(), ttfFonts[i].pixelSize);
    }

#endif

#ifdef GUI_CE

    if(!InitCEGUI())
    {
        ImGui_ImplDX11_Shutdown();
        return false;
    }

#endif

    GUISystemInited = true;
    return true;
}

void GUI::Destroy(void)
{
    if(!GUISystemInited)
        return;
#ifdef GUI_IG

    ImGui_ImplDX11_Shutdown();

#endif

#ifdef GUI_CE

    CEGUI::Direct3D11Renderer::destroySystem();
    ceguiRenderer = nullptr;

#endif

    GUISystemInited = false;
}

void GUI::NewFrame(void)
{
#ifdef GUI_IG

    ImGui_ImplDX11_NewFrame();

#endif
}

void GUI::Render(void)
{
#ifdef GUI_IG

    ImGui::Render();

#endif
}

#ifdef GUI_IG

void GUI::PushFont(int idx)
{
    ImGui::PushFont(imGuiFonts[idx + 1]);
}

void GUI::PopFont(void)
{
    ImGui::PopFont();
}

#endif

namespace
{
    inline int clamp(int v, int minV, int maxV)
    {
        return (std::min)(maxV, (std::max)(v, minV));
    }
}

void GUI::MousePosition(int x, int y)
{
#ifdef GUI_IG

    ImGui::GetIO().MousePos = ImVec2(static_cast<float>(clamp(x, 0, clientWidth)),
                                     static_cast<float>(clamp(y, 0, clientHeight)));

#endif
}

void GUI::MouseButtonDown(MouseButton button)
{
#ifdef GUI_IG

    switch(button)
    {
    case MouseButton::Left:
        ImGui::GetIO().MouseDown[0] = true;
        break;
    case MouseButton::Middle:
        ImGui::GetIO().MouseDown[1] = true;
        break;
    case MouseButton::Right:
        ImGui::GetIO().MouseDown[2] = true;
        break;
    }

#endif
}

void GUI::MouseButtonUp(MouseButton button)
{
#ifdef GUI_IG

    switch(button)
    {
    case MouseButton::Left:
        ImGui::GetIO().MouseDown[0] = false;
        break;
    case MouseButton::Middle:
        ImGui::GetIO().MouseDown[1] = false;
        break;
    case MouseButton::Right:
        ImGui::GetIO().MouseDown[2] = false;
        break;
    }

#endif
}

void GUI::MouseWheel(int wheel)
{
#ifdef GUI_IG

    ImGui::GetIO().MouseWheel += static_cast<float>(wheel);

#endif
}

void GUI::MouseMove(int posX, int posY)
{
#ifdef GUI_IG

    ImGuiIO &io = ImGui::GetIO();
    io.MousePos.x = static_cast<float>(posX);
    io.MousePos.y = static_cast<float>(posY);

#endif
}

void GUI::KeyDown(int VK)
{
#ifdef GUI_IG

    if(0 <= VK && VK < 256)
        ImGui::GetIO().KeysDown[VK] = true;

#endif
}

void GUI::KeyUp(int VK)
{
#ifdef GUI_IG

    if(0 <= VK && VK < 256)
        ImGui::GetIO().KeysDown[VK] = false;

#endif
}

void GUI::Char(unsigned int ch)
{
#ifdef GUI_IG

    if(0 < ch && ch < 0x10000)
        ImGui::GetIO().AddInputCharacter(static_cast<ImWchar>(ch));

#endif
}
