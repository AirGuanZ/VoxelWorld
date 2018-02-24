/*================================================================
Filename: GUISystem.h
Date: 2018.2.23
Created by AirGuanZ
================================================================*/
#ifndef VW_GUI_SYSTEM_H
#define VW_GUI_SYSTEM_H

#include <string>

#include <Utility/Singleton.h>

#include "../Input/InputManager.h"

class GUISystem : public Singleton<GUISystem>
{
public:
    bool Initialize(std::string &errMsg);
    void Destroy(void);

    void NewFrame(void);
    void Render(void);

    // ‰»Î ¬º˛

    void MouseButtonDown(MouseButton button);
    void MouseButtonUp(MouseButton button);

    void MouseWheel(int wheel);
    void MouseMove(int posX, int posY);

    void KeyDown(int VK);
    void KeyUp(int VK);

    void Char(unsigned int ch);
};

#endif //VW_GUI_SYSTEM_H
