/*================================================================
Filename: GUISystem.h
Date: 2018.2.23
Created by AirGuanZ
================================================================*/
#ifndef VW_GUI_SYSTEM_H
#define VW_GUI_SYSTEM_H

#include <string>
#include <vector>

#include <Utility/Singleton.h>

#include <Input/InputManager.h>

#define GUI_SYSTEM_IG
//#define GUI_SYSTEM_NK

#ifdef GUI_SYSTEM_IG
#include <imgui.h>
#endif

#ifdef GUI_SYSTEM_NK
extern "C"
{
    struct nk_context;
}
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

#ifdef GUI_SYSTEM_NK
    nk_context *GetNKContext(void);
#endif

    // ‰»Î ¬º˛

    void BeginInput(void);
    void EndInput(void);

    void MouseButtonDown(MouseButton button);
    void MouseButtonUp(MouseButton button);

    void MouseButtonDown(MouseButton button, LPARAM lParam);
    void MouseButtonUp(MouseButton button, LPARAM lParam);

    void MouseWheel(int wheel);
    void MouseMove(int posX, int posY);

    void KeyDown(int VK);
    void KeyUp(int VK);
    void KeyDownUp(WPARAM wParam, LPARAM lParam);

    void Char(unsigned int ch);
};

#endif //VW_GUI_SYSTEM_H
