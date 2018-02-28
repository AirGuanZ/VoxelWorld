/*================================================================
Filename: Application.cpp
Date: 2018.2.27
Created by AirGuanZ
================================================================*/
#include <Utility/ConfigFile.h>

#include <Resource/ResourceNameManager.h>
#include <Window/Window.h>

#include "Application.h"
#include "Game/Game.h"
#include "MainMenu/MainMenu.h"

namespace
{
    bool LoadAppConf(AppConf &conf, const std::wstring &filename)
    {
        RscNameMgr &rM = RscNameMgr::GetInstance();

        ConfigFile file(filename);
        if(!file)
            return false;

        try
        {
            conf.winWidth  = std::stoi(file(u8"Initialize", u8"WindowWidth"));
            conf.winHeight = std::stoi(file(u8"Initialize", u8"WindowHeight"));
            conf.MSAA      = std::stoi(file(u8"Initialize", u8"MSAA"));

            conf.unloadDistance  = std::stoi(file(u8"World", u8"UnloadDistance"));
            conf.preloadDistance = std::stoi(file(u8"World", u8"PreloadDistance"));
            conf.renderDistance  = std::stoi(file(u8"World", u8"RenderDistance"));
            conf.loaderCount     = std::stoi(file(u8"World", u8"LoaderCount"));

            conf.maxFogStart = std::stof(file(u8"Fog", u8"Start"));
            conf.maxFogRange = std::stof(file(u8"Fog", u8"Range"));

            conf.fonts.resize(std::stoi(rM.AsString(u8"Font", u8"Count")));
            for(size_t i = 0; i < conf.fonts.size(); ++i)
            {
                conf.fonts[i].ttfFilename = rM.AsString(u8"Font", u8"TTFName["   + std::to_string(i) + u8"]");
                conf.fonts[i].pixelSize   = std::stof(rM.AsString(u8"Font", u8"PixelSize[" + std::to_string(i) + u8"]"));
            }
        }
        catch(const std::exception&)
        {
            return false;
        }

        return true;
    }
}

void Application::Run(void)
{
    std::string errMsg;
    if(!Initialize(errMsg))
    {
        ShowErrMsgBox(errMsg);
        return;
    }

    AppState state = AppState::MainMenu;

    while(state != AppState::Exit)
    {
        switch(state)
        {
        case AppState::MainMenu:
            state = MainMenu().Run();
            break;
        case AppState::Game:
            state = Game(appConf_).Run();
            break;
        default:
            std::abort();
        }
    }
}

bool Application::Initialize(std::string &errMsg)
{
    ResourceNameManager &rscMgr = ResourceNameManager::GetInstance();
    errMsg = u8"";

    if(!rscMgr.LoadFromFile(L"resource.txt"))
    {
        errMsg = u8"Failed to load resource configuring file: resource.conf";
        return false;
    }

    if(!LoadAppConf(appConf_, rscMgr(u8"Application", u8"ConfigFile")))
    {
        errMsg = u8"Failed to load configure file";
        return false;
    }

    Window &win = Window::GetInstance();

    if(!win.InitWindow(appConf_.winWidth, appConf_.winHeight, L"VoxelWorld", errMsg) ||
       !win.InitD3D(appConf_.MSAA, 0, errMsg) ||
       !win.InitGUI(appConf_.fonts, errMsg))
    {
        return false;
    }

    return true;
}
