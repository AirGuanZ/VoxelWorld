/*================================================================
Filename: MainMenu.cpp
Date: 2018.2.27
Created by AirGuanZ
================================================================*/
#include <CEGUI/CEGUI.h>

#include <Input/InputManager.h>
#include <Screen/GUISystem.h>
#include <Window/Window.h>

#include "MainMenu.h"

AppState MainMenu::Run(void)
{
    Window &win         = Window::GetInstance();
    InputManager &input = InputManager::GetInstance();

    bool done = false;
    AppState ret = AppState::Exit;

    win.SetBackgroundColor(0.0f, 1.0f, 1.0f, 0.0f);

    GUI::ImFontID font = GUI::GetFontByName(u8"IMFePIrm29P");

    GUIContext *ctx = GUI::CreateGUIContext();
    ctx->SetDefaultFont(u8"DejaVuSans-14");

    CEGUI::PushButton *but = static_cast<CEGUI::PushButton*>(
        ctx->CreateWidget(u8"AlfiskoSkin/Button", u8"MyButton", { 0.5f, 0.5f, 0.1f, 0.05f }, {  }));
    but->setText(u8"Hello, CEGUI");

    while(!done)
    {
        GUI::NewFrame();

        win.ClearDepthStencil();
        win.ClearRenderTarget();

        using namespace ImGui;

        GUI::MousePosition(input.GetCursorPosX(), input.GetCursorPosY());
        if(Begin(u8"MainMenu", nullptr, ImVec2(400.0f, 400.0f), -1.0f,
                 ImGuiWindowFlags_NoTitleBar |
                 ImGuiWindowFlags_NoCollapse |
                 ImGuiWindowFlags_NoResize |
                 ImGuiWindowFlags_NoSavedSettings))
        {
            GUI::PushFont(font);
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
            GUI::PopFont();
        }
        End();

        GUI::BeginCERender();
        ctx->Render();
        GUI::EndCERender();

        GUI::RenderImGui();

        win.Present();
        win.DoEvents();
    }

    GUI::DestroyGUIContext(ctx);

    return ret;
}
