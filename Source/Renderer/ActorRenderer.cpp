/*================================================================
Filename: ActorRenderer.cpp
Date: 2018.2.2
Created by AirGuanZ
================================================================*/
#include "../Resource/ResourceName.h"
#include "../Utility/HelperFunctions.h"
#include "ActorRenderer.h"

ActorRenderer::ActorRenderer(void)
    : inputLayout_(nullptr)
{

}

ActorRenderer::~ActorRenderer(void)
{
    Destroy();
}

bool ActorRenderer::Initialize(void)
{
    //TODO
}
