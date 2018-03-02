/*================================================================
Filename: GUI.cpp
Date: 2018.2.23
Created by AirGuanZ
================================================================*/
#define _CRT_SECURE_NO_WARNINGS

#include <algorithm>
#include <cassert>
#include <cstring>
#include <map>
#include <memory>

#include <filesystem>

namespace filesystem = std::experimental::filesystem::v1;

#include <Resource/ResourceNameManager.h>
#include <Window/Window.h>
#include "GUISystem.h"

#include <CEGUI/CEGUI.h>
#include <CEGUI/RendererModules/Direct3D11/Renderer.h>

SINGLETON_CLASS_DEFINITION(GUI);

#include "imgui/imgui.h"
#include "imgui/imgui_impl_dx11.h"

namespace
{
    bool GUISystemInited = false;
    int clientWidth = 100, clientHeight = 100;

    std::vector<ImFont*> imGuiFonts;
    std::map<std::string, int> imGuiFontMap;

    CEGUI::Direct3D11Renderer::Renderer *ceguiRenderer = nullptr;
}

static bool InitImGui(const std::vector<GUI::ImFontSpec> &ttfFonts)
{
    Window &window = Window::GetInstance();

    if(!ImGui_ImplDX11_Init(window.GetWindowHandle(),
                            window.GetD3DDevice(),
                            window.GetD3DDeviceContext()))
        return false;

    ImGui::StyleColorsClassic();

    //ImGui字体设置
    ImGuiIO &io = ImGui::GetIO();
    imGuiFonts.resize(ttfFonts.size() + 1);
    imGuiFonts[0] = io.Fonts->AddFontDefault();
    imGuiFontMap[u8"Default"] = 0;
    for(size_t i = 0; i < ttfFonts.size(); ++i)
    {
        imGuiFonts[i + 1] = io.Fonts->AddFontFromFileTTF(
            ttfFonts[i].ttfFilename.c_str(), ttfFonts[i].pixelSize);
        imGuiFontMap[filesystem::path(ttfFonts[i].ttfFilename).stem().u8string()] = i + 1;
    }

    return true;
}

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
    
    CEGUI::ImageManager::setImagesetDefaultResourceGroup(u8"imagesets");
    CEGUI::Scheme::setDefaultResourceGroup(u8"schemes");
    CEGUI::Font::setDefaultResourceGroup(u8"fonts");
    CEGUI::WidgetLookManager::setDefaultResourceGroup(u8"looknfeel");
    CEGUI::WindowManager::setDefaultResourceGroup(u8"layouts");
    CEGUI::ScriptModule::setDefaultResourceGroup(u8"lua_scripts");

    //scheme加载

    GUI::LoadCEGUIScheme(rM.AsString(u8"CEGUI", u8"Scheme"));

    //font加载

    int fontNum = std::stoi(rM(u8"CEGUI", u8"FontCount"));
    for(int i = 0; i < fontNum; ++i)
        GUI::LoadCEGUIFont(rM.AsString(u8"CEGUI", u8"Font[" + std::to_string(i) + u8"]"));

    return true;
}

bool GUI::Initialize(const std::vector<ImFontSpec> &ttfFonts, std::string &errMsg)
{
    Window &window = Window::GetInstance();

    clientWidth  = window.GetClientWidth();
    clientHeight = window.GetClientHeight();

    if(!InitImGui(ttfFonts))
        return false;

    if(!InitCEGUI())
    {
        ImGui_ImplDX11_Shutdown();
        return false;
    }

    GUISystemInited = true;
    return true;
}

void GUI::Destroy(void)
{
    if(!GUISystemInited)
        return;

    ImGui_ImplDX11_Shutdown();

    CEGUI::Direct3D11Renderer::destroySystem();
    ceguiRenderer = nullptr;

    GUISystemInited = false;
}

void GUI::NewFrame(void)
{
    ImGui_ImplDX11_NewFrame();
}

void GUI::RenderImGui(void)
{
    ImGui::Render();
}

void GUI::BeginCERender(void)
{
    ceguiRenderer->beginRendering();
}

void GUI::EndCERender(void)
{
    ceguiRenderer->endRendering();
}

void GUI::PushFont(ImFontID id)
{
    ImGui::PushFont(imGuiFonts[id.id]);
}

void GUI::PopFont(void)
{
    ImGui::PopFont();
}

GUI::ImFontID GUI::GetFontByName(const std::string &name)
{
    auto it = imGuiFontMap.find(name);
    return { it != imGuiFontMap.end() ? it->second : 0 };
}

void GUI::LoadCEGUIScheme(const std::string &schemeName)
{
    CEGUI::SchemeManager::getSingleton().createFromFile(schemeName + u8".scheme");
}

void GUI::LoadCEGUIFont(const std::string &fontName)
{
    CEGUI::FontManager::getSingleton().createFromFile(fontName + u8".font");
}

GUIContext *GUI::CreateGUIContext(void)
{
    return new GUIContext();
}

GUIContext *GUI::CreateGUIContextFromLayoutFile(const std::string &filename)
{
    return new GUIContext(filename);
}

void GUI::DestroyGUIContext(GUIContext *ctx)
{
    if(!ctx)
        return;

    delete ctx;
}

namespace
{
    inline int clamp(int v, int minV, int maxV)
    {
        return (std::min)(maxV, (std::max)(v, minV));
    }
}

void GUI::MousePosition(int x, int y)
{
    ImGui::GetIO().MousePos = ImVec2(static_cast<float>(clamp(x, 0, clientWidth)),
                                     static_cast<float>(clamp(y, 0, clientHeight)));
}

void GUI::MouseButtonDown(MouseButton button)
{
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
}

void GUI::MouseButtonUp(MouseButton button)
{
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
}

void GUI::MouseWheel(int wheel)
{
    ImGui::GetIO().MouseWheel += static_cast<float>(wheel);
}

void GUI::MouseMove(int posX, int posY)
{
    ImGuiIO &io = ImGui::GetIO();
    io.MousePos.x = static_cast<float>(posX);
    io.MousePos.y = static_cast<float>(posY);
}

void GUI::KeyDown(int VK)
{
    if(0 <= VK && VK < 256)
        ImGui::GetIO().KeysDown[VK] = true;
}

void GUI::KeyUp(int VK)
{
    if(0 <= VK && VK < 256)
        ImGui::GetIO().KeysDown[VK] = false;
}

void GUI::Char(unsigned int ch)
{
    if(0 < ch && ch < 0x10000)
        ImGui::GetIO().AddInputCharacter(static_cast<ImWchar>(ch));
}

void GUIContext::SetDefaultFont(const std::string &fontName)
{
    ctx_->setDefaultFont(fontName);
}

void GUIContext::SetWidgetRect(CEGUI::Window *wdgt, const Rect &rectPerc, const Rect &rectPixl)
{
    assert(wdgt != nullptr);

    wdgt->setPosition(CEGUI::UVector2(CEGUI::UDim(rectPerc.x, rectPixl.x),
                                      CEGUI::UDim(rectPerc.y, rectPixl.y)));
    wdgt->setSize(CEGUI::USize(CEGUI::UDim(rectPerc.w, rectPixl.w),
                               CEGUI::UDim(rectPerc.h, rectPixl.h)));
}

CEGUI::Window *GUIContext::CreateWidget(const std::string &type, const std::string &name,
                                        const Rect &rectPerc, const Rect &rectPixl)
{
    CEGUI::Window *rt = CEGUI::WindowManager::getSingleton().createWindow(type, name);
    root_->addChild(rt);
    SetWidgetRect(rt, rectPerc, rectPixl);
    return rt;
}

void GUIContext::Render(void)
{
    ctx_->draw();
}

GUIContext::GUIContext(void)
{
    ctx_ = &CEGUI::System::getSingleton().createGUIContext(
        ceguiRenderer->getDefaultRenderTarget());
    root_ = CEGUI::WindowManager::getSingleton().createWindow(
        "DefaultWindow", "Root");
    ctx_->setRootWindow(root_);
}

GUIContext::GUIContext(const std::string &layoutFilename)
{
    ctx_ = &CEGUI::System::getSingleton().createGUIContext(
        ceguiRenderer->getDefaultRenderTarget());
    root_ = CEGUI::WindowManager::getSingleton().loadLayoutFromFile(layoutFilename);
    ctx_->setRootWindow(root_);
}

GUIContext::~GUIContext(void)
{
    if(ctx_)
        CEGUI::System::getSingleton().destroyGUIContext(*ctx_);
}
