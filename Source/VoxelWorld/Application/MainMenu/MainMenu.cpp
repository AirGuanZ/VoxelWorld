/*================================================================
Filename: MainMenu.cpp
Date: 2018.2.27
Created by AirGuanZ
================================================================*/
#include <memory>

#include <CEGUI/CEGUI.h>

#include <Input/InputManager.h>
#include <Resource/ResourceNameManager.h>
#include <Screen/GUISystem.h>
#include <Window/Window.h>

#include "MainMenu.h"

AppState MainMenu::Run(void)
{
    Window &win         = Window::GetInstance();
    InputManager &input = InputManager::GetInstance();
    RscNameMgr &rM      = RscNameMgr::GetInstance();

    bool done = false;
    AppState ret = AppState::Exit;

    win.SetBackgroundColor(0.0f, 1.0f, 1.0f, 0.0f);

    std::unique_ptr<GUIContext, GUIContext::Deleter> ctx(
        GUI::CreateGUIContextFromLayoutFile(rM.AsString(u8"MainMenu", u8"LayoutFile")));
    ctx->SetDefaultFont(rM.AsString(u8"MainMenu", u8"Font"));

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

    return ret;
}
