/*================================================================
Filename: Application.h
Date: 2018.1.12
Created by AirGuanZ
================================================================*/
#ifndef VW_APPLICATION_H
#define VW_APPLICATION_H

#include <string>

#include <Input/InputManager.h>
#include <Screen/Crosshair.h>
#include <Screen/GUISystem.h>
#include <Screen/ImmediateScreen2D.h>
#include <Window/Window.h>
#include <World/World.h>
#include "ChunkRendererManager.h"

class Application
{
public:
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
    };

    Application(void);

    bool Initialize(std::string &errMsg);

    void Run(void);

private:
    AppConf appConf_;

    Window &win_;
    InputManager &input_;
    GUISystem &gui_;

    ID3D11Device *dev_;
    ID3D11DeviceContext *DC_;

    float fogStart_, fogRange_;

    ChunkRendererManager ckRendererMgr_;

    ImmediateScreen2D immScr2D_;
    Crosshair crosshair_;

    std::unique_ptr<World> world_;
};

#endif //VW_APPLICATION_H
