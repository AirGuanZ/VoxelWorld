/*================================================================
Filename: Actor.cpp
Date: 2018.1.19
Created by AirGuanZ
================================================================*/
#include <iostream>
#include <algorithm>

#include <Windows.h>
#include <Keyboard.h>
#include <Mouse.h>

#include "../Input/InputManager.h"
#include "Actor.h"

void Actor::UpdateCamera(float deltaT)
{
    auto &kb = InputManager::GetInstance().GetKeyboard();
    auto &ms = InputManager::GetInstance().GetMouse();
    auto kbState = kb.GetState();
    auto msState = ms.GetState();

    Vector3 pos = camera_.GetPosition();
    Vector3 dir = camera_.GetDirection();

    //垂直移动

    if(kbState.Space)
        pos.y += deltaT * flyUpSpeed_;
    if(kbState.LeftShift)
        pos.y -= deltaT * flyDownSpeed_;

    //水平移动

    Vector3 horDir = { dir.x, 0.0f, dir.z };
    Vector3 horLeftDir = { horDir.z, 0.0f, -horDir.x };
    int horFBMove = 0, horLRMove = 0;
    if(kbState.W)
        ++horFBMove;
    if(kbState.S)
        --horFBMove;
    if(kbState.A)
        ++horLRMove;
    if(kbState.D)
        --horLRMove;

    Vector3 horMove = static_cast<float>(horFBMove) * horDir
                    + static_cast<float>(horLRMove) * horLeftDir;
    horMove.Normalize();
    pos += horMoveSpeed_ * horMove;
    camera_.SetPosition(pos);

    //视角转动

    auto clamp = [](float x, float minV, float maxV)
    {
        return (std::min)(maxV, (std::max)(minV, x));
    };

    std::cout << camera_.GetPitch() << std::endl;

    int mouseDeltaX = msState.x - lastMouseX_;
    int mouseDeltaY = msState.y - lastMouseY_;
    lastMouseX_ = msState.x, lastMouseY_ = msState.y;
    camera_.SetYaw(camera_.GetYaw() - mouseXSpeed_ * mouseDeltaX);
    camera_.SetPitch(clamp(camera_.GetPitch() - mouseYSpeed_ * mouseDeltaY,
                           -DirectX::XM_PIDIV2 + 0.02f, DirectX::XM_PIDIV2 - 0.02f));

    camera_.UpdateViewProjMatrix();
}

const Matrix &Actor::GetViewProjMatrix(void) const
{
    return camera_.GetViewProjMatrix();
}

const Camera &Actor::GetCamera(void) const
{
    return camera_;
}
