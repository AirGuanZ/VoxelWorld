/*================================================================
Filename: Crosshair.cpp
Date: 2018.1.23
Created by AirGuanZ
================================================================*/
#include "../Resource/ResourceName.h"
#include "../Window/Window.h"
#include "Crosshair.h"

Crosshair::Crosshair(void)
{
    tex_.LoadFromFile(CROSSHAIR_BASIC_TEXTURE);
}

void Crosshair::Draw(ImmediateScreen2D *imScr2D)
{
    assert(imScr2D != nullptr);
    Window &win = Window::GetInstance();

    constexpr float CROSSHAIR_PIXEL_SIZE = 35;
    float XSize = 0.5f * CROSSHAIR_PIXEL_SIZE / win.GetClientWidth();
    float YSize = 0.5f * CROSSHAIR_PIXEL_SIZE / win.GetClientHeight();

    imScr2D->DrawRectangle({ -XSize, -YSize }, { XSize, YSize }, tex_);
}
