/*================================================================
Filename: Game.h
Date: 2018.1.12
Created by AirGuanZ
================================================================*/
#pragma once

#include <string>

#include <Input/InputManager.h>
#include <Screen/Crosshair.h>
#include <Screen/GUISystem.h>
#include <Screen/ImmediateScreen2D.h>
#include <Window/Window.h>
#include <World/World.h>

#include <Application/Common.h>
#include "ChunkRendererManager.h"
#include "DebugWindow.h"

class Game
{
public:

    Game(const AppConf &conf);

    AppState Run(void);

private:
    bool Initialize(std::string &errMsg);

private:
    AppConf appConf_;

    Window &win_;
    InputManager &input_;
    GUI &gui_;

    ID3D11Device *dev_;
    ID3D11DeviceContext *DC_;

    float fogStart_, fogRange_;

    ChunkRendererManager ckRendererMgr_;

    ImmediateScreen2D immScr2D_;
    Crosshair crosshair_;

    std::unique_ptr<World> world_;

    DebugWindow mainDebugWin_;
};
