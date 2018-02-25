/*================================================================
Filename: Application.cpp
Date: 2018.1.13
Created by AirGuanZ
================================================================*/
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>

#include <Windows.h>

#include <Utility/Clock.h>
#include <Utility/ConfigFile.h>
#include <Utility/FPSCounter.h>
#include <Utility/HelperFunctions.h>

#include <D3DObject/Sampler.h>
#include <Input/InputManager.h>
#include <Chunk/BasicRenderer.h>
#include <Resource/ResourceName.h>
#include <Screen/Crosshair.h>
#include <Screen/GUISystem.h>
#include <Texture/Texture2D.h>
#include <Window/Window.h>
#include <World/World.h>
#include "Application.h"

#ifdef GUI_SYSTEM_NK
#include <Screen/nuklear/nuklear.h>
#endif

#define PRINT_FPS 1

namespace
{
    bool LoadAppConf(Application::AppConf &conf)
    {
        ConfigFile file(APPLICATION_CONFIGURE_FILE);
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
        }
        catch(const std::invalid_argument&)
        {
            return false;
        }

        return true;
    }
}

Application::Application(void)
    : win_(Window::GetInstance()),
      input_(InputManager::GetInstance()),
      gui_(GUISystem::GetInstance())
{
    dev_ = nullptr;
    DC_ = nullptr;

    fogStart_ = 0.0f;
    fogRange_ = 1.0f;
}

bool Application::Initialize(std::string &errMsg)
{
    errMsg = "";

    if(!LoadAppConf(appConf_))
    {
        errMsg = "Failed to load configure file";
        return false;
    }

    if(!win_.InitWindow(appConf_.winWidth, appConf_.winHeight,
        L"VoxelWorld", errMsg) ||
       !win_.InitD3D(appConf_.MSAA, 0, errMsg))
    {
        return false;
    }
    dev_ = win_.GetD3DDevice();
    DC_ = win_.GetD3DDeviceContext();

    if(!ckRendererMgr_.Initialize(errMsg))
        return false;

    return true;
}

void Application::Run(void)
{
    //渲染器和纹理

    std::string initErrMsg;

    ChunkSectionRenderQueue renderQueue;

    //区块世界

    World world(appConf_.preloadDistance,
                appConf_.renderDistance,
                appConf_.unloadDistance);
    if(!world.Initialize(appConf_.loaderCount, initErrMsg))
    {
        std::cerr << initErrMsg << std::endl;
        return;
    }

    //准星

    ImmediateScreen2D imScr2D;
    if(!imScr2D.Initialize(initErrMsg))
    {
        std::cerr << initErrMsg << std::endl;
        return;
    }

    Crosshair crosshair;
    if(!crosshair.Initialize())
    {
        std::cerr << "Failed to initialize crosshair icon" << std::endl;
        return;
    }

    input_.LockCursor(true, win_.ClientCentreX(), win_.ClientCentreY());
    input_.ShowCursor(false);

    Clock clock;
    clock.Restart();

#if PRINT_FPS
    FPSCounter fps;
    float lastFPS = 0.0f;
    fps.Restart();
#endif

    float daynightT = 0.0f;
    
    while(!input_.IsKeyDown(VK_ESCAPE))
    {
        gui_.NewFrame();

#if PRINT_FPS
        {
            fps.Tick();
            if(fps.GetFPS() != lastFPS)
                lastFPS = fps.GetFPS();
            ImGui::SetNextWindowSize(ImVec2(200, 200));
            ImGui::Begin("Debug", nullptr, ImGuiWindowFlags_NoResize |
                                           ImGuiWindowFlags_NoMove |
                                           ImGuiWindowFlags_NoCollapse);
            {
                ImGui::Text(("FPS: " + std::to_string(lastFPS)).c_str());
            }
            ImGui::End();
        }
#endif

        daynightT += input_.IsKeyDown('T') ? 0.01f : 0.0001f;
        float absdnt = 0.5f * (std::max)((std::min)(2.0f * std::cos(daynightT), 1.0f), -1.0f) + 0.5f;

        Vector3 sunlight = { absdnt, absdnt, absdnt };

        win_.SetBackgroundColor(0.0f, absdnt, absdnt, 0.0f);

        win_.ClearRenderTarget();
        win_.ClearDepthStencil();

        //天光强度设置

        ckRendererMgr_.SetSunlight(DC_, sunlight);

        //雾设置

        fogStart_ = (std::min)(fogStart_ + 0.12f, appConf_.maxFogStart);
        fogRange_ = (std::min)(fogRange_ + 0.12f, appConf_.maxFogRange);

        ckRendererMgr_.SetFog(DC_, fogStart_, fogRange_, { 0.0f, absdnt, absdnt }, world.GetActor().GetCameraPosition());

        world.Update(16.6667f);

        world.Render(&renderQueue);

        ckRendererMgr_.SetTrans(DC_, world.GetActor().GetCamera().GetViewProjMatrix().Transpose());
        ckRendererMgr_.Render(DC_, renderQueue);

        world.GetActor().Render();

        crosshair.Draw(&imScr2D);

        gui_.Render();

        //win_.SetVsync(false);
        win_.Present();
        win_.DoEvents();
        clock.Tick();
    }
}
