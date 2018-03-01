/*================================================================
Filename: GUI.h
Date: 2018.2.23
Created by AirGuanZ
================================================================*/
#pragma once

#include <string>
#include <vector>

#include <Utility/Singleton.h>

#include <Input/InputManager.h>

#define GUI_IG
#define GUI_CE

#ifdef GUI_IG

#include "imgui/imgui.h"

#endif

class GUI : public Singleton<GUI>
{
public:
    struct ImFontSpec
    {
        std::string ttfFilename;
        float pixelSize;
    };

    struct ImFontID
    {
        int id;
    };

    bool Initialize(const std::vector<ImFontSpec> &ttfFonts, std::string &errMsg);
    void Destroy(void);

    void NewFrame(void);
    void Render(void);

#ifdef GUI_IG

    void PushFont(ImFontID id);
    void PopFont(void);

    ImFontID GetFontByName(const std::string &name);

#endif

    //输入事件，一般由消息循环调用

    //该函数需要自己调用
    void MousePosition(int x, int y);

    void MouseButtonDown(MouseButton button);
    void MouseButtonUp(MouseButton button);

    void MouseWheel(int wheel);
    void MouseMove(int posX, int posY);

    void KeyDown(int VK);
    void KeyUp(int VK);

    void Char(unsigned int ch);
};
