/*================================================================
Filename: Application/Common.h
Date: 2018.2.27
Created by AirGuanZ
================================================================*/
#pragma once

#include <string>
#include <vector>

#include <Screen/GUISystem.h>

struct AppConf
{
    int winWidth;
    int winHeight;
    int MSAA;

    float maxFogStart;
    float maxFogRange;

    int unloadDistance;
    int preloadDistance;
    int renderDistance;

    int loaderCount;

    std::vector<ImFontSpec> fonts;
};

enum class AppState
{
    MainMenu,
    Exit,
    Game
};

void ShowErrMsgBox(const std::string &errMsg);
