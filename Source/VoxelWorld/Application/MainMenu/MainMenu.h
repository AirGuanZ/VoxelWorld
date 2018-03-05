/*================================================================
Filename: MainMenu.h
Date: 2018.2.23
Created by AirGuanZ
================================================================*/
#pragma once

#include <cstdint>

#include <CEGUI/CEGUI.h>

#include <Application/Common.h>
#include <Texture/Texture2D.h>

class MainMenu
{
public:
    AppState Run(void);

private:
    bool GameClicked(const CEGUI::EventArgs&) { return gameClicked_ = true; }
    bool ExitClicked(const CEGUI::EventArgs&) { return exitClicked_ = true; }
    bool EditClicked(const CEGUI::EventArgs&) { return editClicked_ = true; }

private:
    bool gameClicked_ = false;
    bool exitClicked_ = false;
    bool editClicked_ = false;
};
