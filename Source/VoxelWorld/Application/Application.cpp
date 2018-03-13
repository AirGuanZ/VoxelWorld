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
#include "VoxelModelEditor/VoxelModelEditor.h"

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
            conf.winWidth  = std::stoi(file("Initialize", "WindowWidth"));
            conf.winHeight = std::stoi(file("Initialize", "WindowHeight"));
            conf.MSAA      = std::stoi(file("Initialize", "MSAA"));

            conf.unloadDistance  = std::stoi(file("World", "UnloadDistance"));
            conf.preloadDistance = std::stoi(file("World", "PreloadDistance"));
            conf.renderDistance  = std::stoi(file("World", "RenderDistance"));
            conf.loaderCount     = std::stoi(file("World", "LoaderCount"));

            conf.maxFogStart = std::stof(file("Fog", "Start"));
            conf.maxFogRange = std::stof(file("Fog", "Range"));

            conf.fonts.resize(std::stoi(rM.AsString("ImFont", "Count")));
            for(size_t i = 0; i < conf.fonts.size(); ++i)
            {
                conf.fonts[i].ttfFilename = rM.AsString("ImFont", "TTFName["   + std::to_string(i) + "]");
                conf.fonts[i].pixelSize   = std::stof(rM.AsString("ImFont", "PixelSize[" + std::to_string(i) + "]"));
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
        case AppState::VoxelModelEditor:
            state = VoxelModelEditor().Run();
            break;
        default:
            std::abort();
        }
    }
}

bool Application::Initialize(std::string &errMsg)
{
    ResourceNameManager &rscMgr = ResourceNameManager::GetInstance();
    errMsg = "";

    if(!rscMgr.LoadFromFile(L"resource.txt"))
    {
        errMsg = "Failed to load resource configuring file: resource.conf";
        return false;
    }

    if(!LoadAppConf(appConf_, rscMgr("Application", "ConfigFile")))
    {
        errMsg = "Failed to load configure file";
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
