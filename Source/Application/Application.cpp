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

#include "../Actor/Actor.h"
#include "../Input/InputManager.h"
#include "../Renderer/BasicRenderer.h"
#include "../Texture/Texture2D.h"
#include "../Utility/Clock.h"
#include "../Utility/HelperFunctions.h"
#include "../Window/Window.h"
#include "Application.h"

void Application::Run(void)
{
    std::string initErrMsg;
    Window &window = Window::GetInstance();
    if(!window.InitWindow(1200, 900, L"Voxel World", initErrMsg) ||
       !window.InitD3D(4, 0, initErrMsg))
    {
        throw std::runtime_error(initErrMsg.c_str());
    }

    window.SetBackgroundColor(0.0f, 1.0f, 1.0f, 0.0f);

    Clock clock;
    clock.Restart();

    while(!InputManager::GetInstance().IsKeyDown(VK_ESCAPE))
    {
        window.ClearRenderTarget();
        window.ClearDepthStencil();

        window.Present();
        window.DoEvents();
        clock.Tick();
    }

    window.Destroy();
}
