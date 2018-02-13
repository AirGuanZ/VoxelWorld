/*================================================================
Filename: Crosshair.h
Date: 2018.1.23
Created by AirGuanZ
================================================================*/
#ifndef VW_CROSSHAIR_H
#define VW_CROSSHAIR_H

#include <OWEShader.hpp>

#include <Utility/D3D11Header.h>

#include "../Texture/Texture2D.h"
#include "ImmediateScreen2D.h"

class Crosshair
{
public:
    bool Initialize(void);

    void Draw(ImmediateScreen2D *imScr2D);

private:
    Texture2D tex_;
};

#endif //VW_CROSSHAIR_H
