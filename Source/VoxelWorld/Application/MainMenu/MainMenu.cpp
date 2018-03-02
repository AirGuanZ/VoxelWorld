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

    GUIContext *ctx = GUI::CreateGUIContextFromLayoutFile("MainMenu.layout");
    ctx->SetDefaultFont(u8"IMFePIrm29P");

    class ButtonClicked
    {
    public:
        bool Game(const CEGUI::EventArgs&)
        {
            game = true;
            return true;
        }

        bool Exit(const CEGUI::EventArgs&)
        {
            exit = true;
            return true;
        }

        bool game = false;
        bool exit = false;
    } buttonClicked;

    CEGUI::PushButton *gameButton = static_cast<CEGUI::PushButton*>
        (ctx->GetCEGUIContext()->getRootWindow()->getChild(u8"Game"));
    gameButton->subscribeEvent(CEGUI::PushButton::EventMouseClick,
                               CEGUI::Event::Subscriber(&ButtonClicked::Game, &buttonClicked));

    CEGUI::PushButton *exitButton = static_cast<CEGUI::PushButton*>
        (ctx->GetCEGUIContext()->getRootWindow()->getChild(u8"Exit"));
    exitButton->subscribeEvent(CEGUI::PushButton::EventMouseClick,
                               CEGUI::Event::Subscriber(&ButtonClicked::Exit, &buttonClicked));

    while(!done)
    {
        GUI::NewFrame();

        win.ClearDepthStencil();
        win.ClearRenderTarget();

        if(buttonClicked.game)
        {
            ret = AppState::Game;
            done = true;
        }
        else if(buttonClicked.exit)
        {
            ret = AppState::Exit;
            done = true;
        }

        GUI::BeginCERender();
        ctx->Render();
        GUI::EndCERender();

        win.Present();
        win.DoEvents();
    }

    GUI::DestroyGUIContext(ctx);

    return ret;
}
