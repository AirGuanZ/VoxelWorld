/*================================================================
Filename: Crosshair.cpp
Date: 2018.1.23
Created by AirGuanZ
================================================================*/
#include <Resource/ResourceNameManager.h>
#include <Window/Window.h>
#include "Crosshair.h"

bool Crosshair::Initialize(void)
{
    if(!tex_.IsAvailable())
        return tex_.LoadFromFile(RscNameMgr::GetInstance()("Crosshair", "Texture"));
    return true;
}

void Crosshair::Draw(ImmediateScreen2D *imScr2D)
{
    assert(imScr2D != nullptr);
    Window &win = Window::GetInstance();

    constexpr float CROSSHAIR_PIXEL_SIZE = 35.0f;
    float XSize = 0.5f * CROSSHAIR_PIXEL_SIZE / win.GetClientWidth();
    float YSize = 0.5f * CROSSHAIR_PIXEL_SIZE / win.GetClientHeight();

    imScr2D->DrawRectangle({ -XSize, -YSize }, { XSize, YSize }, tex_);
}
