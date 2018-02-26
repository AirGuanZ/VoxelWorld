/*================================================================
Filename: Crosshair.h
Date: 2018.1.23
Created by AirGuanZ
================================================================*/
#pragma once

#include <OWEShader.hpp>

#include <Utility/D3D11Header.h>

#include <Texture/Texture2D.h>
#include "ImmediateScreen2D.h"

class Crosshair
{
public:
    bool Initialize(void);

    void Draw(ImmediateScreen2D *imScr2D);

private:
    Texture2D tex_;
};
