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
#include <set>

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

    std::set<CEGUI::GUIContext*> ceguiCtxs;
}

static bool InitImGui(const std::vector<ImFontSpec> &ttfFonts)
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

ImFontID GUI::GetFontByName(const std::string &name)
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
    GUIContext *ctx = new GUIContext();
    ceguiCtxs.insert(ctx->GetCEGUIContext());
    return ctx;
}

GUIContext *GUI::CreateGUIContextFromLayoutFile(const std::string &filename)
{
    GUIContext *ctx = new GUIContext(filename);
    ceguiCtxs.insert(ctx->GetCEGUIContext());
    return ctx;
}

void GUI::DestroyGUIContext(GUIContext *ctx)
{
    if(!ctx)
        return;

    ceguiCtxs.erase(ctx->GetCEGUIContext());
    delete ctx;
}

namespace
{
    inline int clamp(int v, int minV, int maxV)
    {
        return (std::min)(maxV, (std::max)(v, minV));
    }

    CEGUI::Key::Scan VKToCEGUIKeyScan(int VK)
    {
        using namespace CEGUI;
        static const class ScanTable
        {
        public:
            ScanTable(void)
            {
                for(auto &k : ks)
                    k = Key::Unknown;
                ks[VK_ESCAPE] = Key::Escape;
                ks['0'] = Key::Zero;
                ks['1'] = Key::One;
                ks['2'] = Key::Two;
                ks['3'] = Key::Three;
                ks['4'] = Key::Four;
                ks['5'] = Key::Five;
                ks['6'] = Key::Six;
                ks['7'] = Key::Seven;
                ks['8'] = Key::Eight;
                ks['9'] = Key::Nine;
                ks[VK_OEM_MINUS] = Key::Minus;
                ks[VK_OEM_PLUS] = Key::Equals;
                ks[VK_BACK] = Key::Backspace;
                ks[VK_TAB] = Key::Tab;
                ks['Q'] = Key::Q;
                ks['W'] = Key::W;
                ks['E'] = Key::E;
                ks['R'] = Key::R;
                ks['T'] = Key::T;
                ks['Y'] = Key::Y;
                ks['U'] = Key::U;
                ks['I'] = Key::I;
                ks['O'] = Key::O;
                ks['P'] = Key::P;
                ks[VK_OEM_4] = Key::LeftBracket;
                ks[VK_OEM_6] = Key::RightBracket;
                ks[VK_RETURN] = Key::Return;
                ks[VK_LCONTROL] = Key::LeftControl;
                ks['A'] = Key::A;
                ks['S'] = Key::S;
                ks['D'] = Key::D;
                ks['F'] = Key::F;
                ks['G'] = Key::G;
                ks['H'] = Key::H;
                ks['J'] = Key::J;
                //TODO
            }

            Key::Scan ks[256];
        } table;
        return table.ks[VK];
        /*
            enum  	Scan {
            Unknown =0x00, Escape =0x01, One =0x02, Two =0x03,
            Three =0x04, Four =0x05, Five =0x06, Six =0x07,
            Seven =0x08, Eight =0x09, Nine =0x0A, Zero =0x0B,
            Minus =0x0C, Equals =0x0D, Backspace =0x0E, Tab =0x0F,
            Q =0x10, W =0x11, E =0x12, R =0x13,
            T =0x14, Y =0x15, U =0x16, I =0x17,
            O =0x18, P =0x19, LeftBracket =0x1A, RightBracket =0x1B,
            Return =0x1C, LeftControl =0x1D, A =0x1E, S =0x1F,
            D =0x20, F =0x21, G =0x22, H =0x23,
            J =0x24, K =0x25, L =0x26, Semicolon =0x27,
            Apostrophe =0x28, Grave =0x29, LeftShift =0x2A, Backslash =0x2B,
            Z =0x2C, X =0x2D, C =0x2E, V =0x2F,
            B =0x30, N =0x31, M =0x32, Comma =0x33,
            Period =0x34, Slash =0x35, RightShift =0x36, Multiply =0x37,
            LeftAlt =0x38, Space =0x39, Capital =0x3A, F1 =0x3B,
            F2 =0x3C, F3 =0x3D, F4 =0x3E, F5 =0x3F,
            F6 =0x40, F7 =0x41, F8 =0x42, F9 =0x43,
            F10 =0x44, NumLock =0x45, ScrollLock =0x46, Numpad7 =0x47,
            Numpad8 =0x48, Numpad9 =0x49, Subtract =0x4A, Numpad4 =0x4B,
            Numpad5 =0x4C, Numpad6 =0x4D, Add =0x4E, Numpad1 =0x4F,
            Numpad2 =0x50, Numpad3 =0x51, Numpad0 =0x52, Decimal =0x53,
            OEM_102 =0x56, F11 =0x57, F12 =0x58, F13 =0x64,
            F14 =0x65, F15 =0x66, Kana =0x70, ABNT_C1 =0x73,
            Convert =0x79, NoConvert =0x7B, Yen =0x7D, ABNT_C2 =0x7E,
            NumpadEquals =0x8D, PrevTrack =0x90, At =0x91, Colon =0x92,
            Underline =0x93, Kanji =0x94, Stop =0x95, AX =0x96,
            Unlabeled =0x97, NextTrack =0x99, NumpadEnter =0x9C, RightControl =0x9D,
            Mute =0xA0, Calculator =0xA1, PlayPause =0xA2, MediaStop =0xA4,
            VolumeDown =0xAE, VolumeUp =0xB0, WebHome =0xB2, NumpadComma =0xB3,
            Divide =0xB5, SysRq =0xB7, RightAlt =0xB8, Pause =0xC5,
            Home =0xC7, ArrowUp =0xC8, PageUp =0xC9, ArrowLeft =0xCB,
            ArrowRight =0xCD, End =0xCF, ArrowDown =0xD0, PageDown =0xD1,
            Insert =0xD2, Delete =0xD3, LeftWindows =0xDB, RightWindows =0xDC,
            AppMenu =0xDD, Power =0xDE, Sleep =0xDF, Wake =0xE3,
            WebSearch =0xE5, WebFavorites =0xE6, WebRefresh =0xE7, WebStop =0xE8,
            WebForward =0xE9, WebBack =0xEA, MyComputer =0xEB, Mail =0xEC,
            MediaSelect =0xED
        }
        */
    }
}

void GUI::MousePosition(int x, int y)
{
    float fx = static_cast<float>(clamp(x, 0, clientWidth));
    float fy = static_cast<float>(clamp(y, 0, clientHeight));
    ImGui::GetIO().MousePos = ImVec2(fx, fy);
    for(auto ctx : ceguiCtxs)
        ctx->injectMousePosition(fx, fy);
}

void GUI::MouseButtonDown(MouseButton button)
{
    switch(button)
    {
    case MouseButton::Left:
        ImGui::GetIO().MouseDown[0] = true;
        for(auto ctx : ceguiCtxs)
            ctx->injectMouseButtonDown(CEGUI::MouseButton::LeftButton);
        break;
    case MouseButton::Middle:
        ImGui::GetIO().MouseDown[1] = true;
        for(auto ctx : ceguiCtxs)
            ctx->injectMouseButtonDown(CEGUI::MouseButton::MiddleButton);
        break;
    case MouseButton::Right:
        ImGui::GetIO().MouseDown[2] = true;
        for(auto ctx : ceguiCtxs)
            ctx->injectMouseButtonDown(CEGUI::MouseButton::RightButton);
        break;
    }
}

void GUI::MouseButtonUp(MouseButton button)
{
    switch(button)
    {
    case MouseButton::Left:
        ImGui::GetIO().MouseDown[0] = false;
        for(auto ctx : ceguiCtxs)
            ctx->injectMouseButtonUp(CEGUI::MouseButton::LeftButton);
        break;
    case MouseButton::Middle:
        ImGui::GetIO().MouseDown[1] = false;
        for(auto ctx : ceguiCtxs)
            ctx->injectMouseButtonUp(CEGUI::MouseButton::MiddleButton);
        break;
    case MouseButton::Right:
        ImGui::GetIO().MouseDown[2] = false;
        for(auto ctx : ceguiCtxs)
            ctx->injectMouseButtonUp(CEGUI::MouseButton::RightButton);
        break;
    }
}

void GUI::MouseWheel(int wheel)
{
    float delta = static_cast<float>(wheel);
    ImGui::GetIO().MouseWheel += delta;
    for(auto ctx : ceguiCtxs)
        ctx->injectMouseWheelChange(delta);
}

void GUI::KeyDown(int VK)
{
    if(0 <= VK && VK < 256)
    {
        ImGui::GetIO().KeysDown[VK] = true;
        for(auto ctx : ceguiCtxs)
            ctx->injectKeyDown(VKToCEGUIKeyScan(VK));
    }
}

void GUI::KeyUp(int VK)
{
    if(0 <= VK && VK < 256)
    {
        ImGui::GetIO().KeysDown[VK] = false;
        for(auto ctx : ceguiCtxs)
            ctx->injectKeyUp(VKToCEGUIKeyScan(VK));
    }
}

void GUI::Char(unsigned int ch)
{
    if(0 < ch && ch < 0x10000)
    {
        ImGui::GetIO().AddInputCharacter(static_cast<ImWchar>(ch));
        for(auto ctx : ceguiCtxs)
            ctx->injectChar(ch);
    }
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
