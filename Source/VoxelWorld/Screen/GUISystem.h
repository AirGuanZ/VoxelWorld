/*================================================================
Filename: GUI.h
Date: 2018.2.23
Created by AirGuanZ
================================================================*/
#pragma once

#include <string>
#include <vector>

#include <Utility/Math.h>
#include <Utility/Singleton.h>

#include <Input/InputManager.h>

#include "imgui/imgui.h"

class GUI;
class GUIContext;

namespace CEGUI
{
    class GUIContext;
    class Window;
}

struct ImFontSpec
{
    std::string ttfFilename;
    float pixelSize;
};

struct ImFontID
{
    int id;
};

class GUI
{
public:
    static bool Initialize(const std::vector<ImFontSpec> &ttfFonts, std::string &errMsg);
    static void Destroy(void);

    static void NewFrame(void);
    static void RenderImGui(void);

    static void BeginCERender(void);
    static void EndCERender(void);

    static void PushFont(ImFontID id);
    static void PopFont(void);

    static ImFontID GetFontByName(const std::string &name);

    static void LoadCEGUIScheme(const std::string &schemeName);
    static void LoadCEGUIFont(const std::string &fontName);

    static GUIContext *CreateGUIContext(void);
    static GUIContext *CreateGUIContextFromLayoutFile(const std::string &layoutFilename);
    static void DestroyGUIContext(GUIContext *ctx);

    static void TimeElapsed(float dT);

    //输入事件，一般由消息循环调用

    static void MousePosition(int x, int y);

    static void MouseButtonDown(MouseButton button);
    static void MouseButtonUp(MouseButton button);

    static void MouseWheel(int wheel);

    static void KeyDown(int VK);
    static void KeyUp(int VK);

    static void Char(unsigned int ch);
};

class GUIContext
{
public:
    class Deleter
    {
    public:
        void operator()(GUIContext *ctx)
        {
            if(ctx)
                GUI::DestroyGUIContext(ctx);
        }
    };

    void SetDefaultFont(const std::string &fontName);

    static void SetWidgetRect(CEGUI::Window *wdgt, const Rect &rectPerc, const Rect &rectPixl);
    CEGUI::Window *CreateWidget(const std::string &type, const std::string &name,
                                const Rect &rectPerc, const Rect &rectPixl);

    CEGUI::GUIContext *GetCEGUIContext(void) { return ctx_; }

    void Render(void);

    void Enable(void);
    void Disable(void);

private:
    friend class GUI;

    GUIContext(void);
    GUIContext(const std::string &layoutFilename);
    ~GUIContext(void);

private:
    CEGUI::GUIContext *ctx_;
    CEGUI::Window *root_;
};
