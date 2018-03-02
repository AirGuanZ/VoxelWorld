/*================================================================
Filename: Game.cpp
Date: 2018.1.13
Created by AirGuanZ
================================================================*/
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>

#include <Windows.h>

#include <Utility/Clock.h>
#include <Utility/FPSCounter.h>
#include <Utility/HelperFunctions.h>

#include <Input/InputManager.h>
#include <Screen/GUISystem.h>
#include "Game.h"

Game::Game(const AppConf &conf)
    : win_(Window::GetInstance()),
      input_(InputManager::GetInstance()),
      appConf_(conf)
{
    dev_ = nullptr;
    DC_  = nullptr;

    fogStart_ = 0.0f;
    fogRange_ = 1.0f;
}

bool Game::Initialize(std::string &errMsg)
{
    errMsg = "";

    dev_ = win_.GetD3DDevice();
    DC_  = win_.GetD3DDeviceContext();

    if(!ckRendererMgr_.Initialize(errMsg))
        return false;

    if(!immScr2D_.Initialize(errMsg))
        return false;

    if(!crosshair_.Initialize())
    {
        errMsg = "Failed to initialize crosshair";
        return false;
    }

    world_ = std::make_unique<World>(appConf_.preloadDistance,
                                     appConf_.renderDistance,
                                     appConf_.unloadDistance);
    if(!world_->Initialize(appConf_.loaderCount, errMsg))
        return false;

    return true;
}

AppState Game::Run(void)
{
    std::string errMsg;
    if(!Initialize(errMsg))
    {
        ShowErrMsgBox(errMsg);
        return AppState::MainMenu;
    }

    ChunkSectionRenderQueue renderQueue;

    input_.LockCursor(true, win_.GetClientWidth() / 2, win_.GetClientHeight() / 2);
    input_.ShowCursor(false);

    Clock clock;
    clock.Restart();

    FPSCounter fps;
    float lastFPS = 0.0f;
    fps.Restart();

    float daynightT = 0.0f;
    
    while(!input_.IsKeyDown(VK_ESCAPE))
    {
        GUI::NewFrame();

        fps.Tick(clock.ElapsedTime());
        if(fps.GetFPS() != lastFPS)
            lastFPS = fps.GetFPS();

        DebugWindow::Info debugInfo;
        debugInfo.FPS           = lastFPS;
        debugInfo.actorOnGround = world_->GetActor().OnGround();
        debugInfo.actorPos      = world_->GetActor().GetPosition();
        debugInfo.camPos        = world_->GetActor().GetCameraPosition();

        mainDebugWin_.SetInfo(debugInfo);
        mainDebugWin_.Update(input_);

        //����ʱ��������ͱ���ɫ
        daynightT += input_.IsKeyDown('T') ? 0.01f : 0.0001f;
        float absdnt = 0.5f * (std::max)((std::min)(2.0f * std::cos(daynightT), 1.0f), -1.0f) + 0.5f;
        Vector3 sunlight = { absdnt, absdnt, absdnt };
        win_.SetBackgroundColor(0.0f, absdnt, absdnt, 0.0f);

        win_.ClearRenderTarget();
        win_.ClearDepthStencil();

        //���ǿ������
        ckRendererMgr_.SetSunlight(sunlight);

        //������
        fogStart_ = (std::min)(fogStart_ + 0.12f, appConf_.maxFogStart);
        fogRange_ = (std::min)(fogRange_ + 0.12f, appConf_.maxFogRange);
        ckRendererMgr_.SetFog(fogStart_, fogRange_, { 0.0f, absdnt, absdnt }, world_->GetActor().GetCameraPosition());

        //�������
        world_->Update(16.6667f);

        //�ύ������Ⱦ����
        world_->Render(&renderQueue);

        //��Ⱦ����
        ckRendererMgr_.SetTrans(world_->GetActor().GetCamera().GetViewProjMatrix().Transpose());
        ckRendererMgr_.Render(renderQueue);

        //��Ⱦ��ɫ
        world_->GetActor().Render();

        //����׼��
        crosshair_.Draw(&immScr2D_);

        //debug���ڻ���
        mainDebugWin_.Render();

        //����GUI
        GUI::RenderImGui();

        //win_.SetVsync(false);
        win_.Present();
        win_.DoEvents();
        clock.Tick();
    }

    input_.LockCursor(false);
    input_.ShowCursor(true);

    return AppState::MainMenu;
}
