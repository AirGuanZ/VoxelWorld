/*================================================================
Filename: GUISystem.h
Date: 2018.2.23
Created by AirGuanZ
================================================================*/
#pragma once

#include <string>
#include <vector>

#include <Utility/Singleton.h>

#include <Input/InputManager.h>

#define GUI_SYSTEM_IG

#ifdef GUI_SYSTEM_IG
#include "imgui/imgui.h"
#endif

class GUISystem : public Singleton<GUISystem>
{
public:
    struct FontSpecifier
    {
        std::string ttfFilename;
        float pixelSize;
    };

    bool Initialize(const std::vector<FontSpecifier> &ttfFonts, std::string &errMsg);
    void Destroy(void);

    void NewFrame(void);
    void Render(void);

#ifdef GUI_SYSTEM_IG
    void PushFont(int index = -1);
    void PopFont(void);
#endif

    //输入事件，一般来说由消息循环调用

    void MouseButtonDown(MouseButton button);
    void MouseButtonUp(MouseButton button);

    void MouseWheel(int wheel);
    void MouseMove(int posX, int posY);

    void KeyDown(int VK);
    void KeyUp(int VK);

    void Char(unsigned int ch);
};
