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
    GUISystem &gui      = GUISystem::GetInstance();
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
        if(Begin("MainMenu", nullptr, ImVec2(400.0f, 400.0f), -1.0f,
                 ImGuiWindowFlags_NoCollapse |
                 ImGuiWindowFlags_NoResize |
                 ImGuiWindowFlags_NoSavedSettings))
        {
            LabelText("MousePosition", (std::to_string(input.GetCursorPosX()) + ", " +
                                        std::to_string(input.GetCursorPosY())).c_str());
            if(Button("Game"))
            {
                ret = AppState::Game;
                done = true;
            }
            else if(Button("Exit"))
            {
                ret = AppState::Exit;
                done = true;
            }
        }
        End();

        gui.Render();

        win.Present();
        win.DoEvents();
    }

    return ret;
}
