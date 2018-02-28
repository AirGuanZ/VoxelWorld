/*================================================================
Filename: MainMenu.cpp
Date: 2018.2.27
Created by AirGuanZ
================================================================*/
#include <Input/InputManager.h>
#include <Screen/GUISystem.h>
#include <Window/Window.h>

#include "MainMenu.h"

AppState MainMenu::Run(void)
{
    Window &win         = Window::GetInstance();
    GUI &gui      = GUI::GetInstance();
    InputManager &input = InputManager::GetInstance();

    bool done = false;
    AppState ret = AppState::Exit;

    win.SetBackgroundColor(0.0f, 1.0f, 1.0f, 0.0f);

    while(!done)
    {
        gui.NewFrame();

        win.ClearDepthStencil();
        win.ClearRenderTarget();

        using namespace ImGui;

        gui.MousePosition(input.GetCursorPosX(), input.GetCursorPosY());
        if(Begin(u8"MainMenu", nullptr, ImVec2(400.0f, 400.0f), -1.0f,
                 ImGuiWindowFlags_NoTitleBar |
                 ImGuiWindowFlags_NoCollapse |
                 ImGuiWindowFlags_NoResize |
                 ImGuiWindowFlags_NoSavedSettings))
        {
            gui.PushFont(0);
            Text((std::to_string(input.GetCursorPosX()) + u8", " +
                  std::to_string(input.GetCursorPosY())).c_str());
            if(Button(u8"Game"))
            {
                ret = AppState::Game;
                done = true;
            }
            else if(Button(u8"Exit"))
            {
                ret = AppState::Exit;
                done = true;
            }
            gui.PopFont();
        }
        End();

        gui.Render();

        win.Present();
        win.DoEvents();
    }

    return ret;
}
