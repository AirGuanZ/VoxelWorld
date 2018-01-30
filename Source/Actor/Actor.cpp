/*================================================================
Filename: Actor.cpp
Date: 2018.1.19
Created by AirGuanZ
================================================================*/
#include <algorithm>

#include <Windows.h>
#include <Keyboard.h>
#include <Mouse.h>

#include "../Input/InputManager.h"
#include "Actor.h"

void Actor::UpdateCamera(float deltaT, ChunkManager *ckMgr)
{
    assert(ckMgr != nullptr);

    InputManager &input = InputManager::GetInstance();

    Vector3 pos = camera_.GetPosition();
    Vector3 dir = camera_.GetDirection();

    //移动前备份位置坐标，方便其后的碰撞检测恢复
    Vector3 oldPos = pos;

    //垂直移动

    if(input.IsKeyDown(VK_SPACE))
        pos.y += 16.66f * flyUpSpeed_;
    if(input.IsKeyDown(VK_LSHIFT))
        pos.y -= 16.66f * flyDownSpeed_;

    //水平移动

    Vector3 horDir = { dir.x, 0.0f, dir.z };
    Vector3 horLeftDir = { horDir.z, 0.0f, -horDir.x };
    int horFBMove = 0, horLRMove = 0;
    if(input.IsKeyDown('W'))
        ++horFBMove;
    if(input.IsKeyDown('S'))
        --horFBMove;
    if(input.IsKeyDown('A'))
        ++horLRMove;
    if(input.IsKeyDown('D'))
        --horLRMove;

    Vector3 horMove = static_cast<float>(horFBMove) * horDir
                    + static_cast<float>(horLRMove) * horLeftDir;
    horMove.Normalize();
    pos += horMoveSpeed_ * horMove * 16.66f;
    if(input.IsKeyDown(VK_LCONTROL))
        pos += horMoveSpeed_ * horMove * (16.66f * 4.0f);

    //碰撞检测

    //将一个位置设置为摄像机新位置，若它不会发生碰撞
    auto TryPos = [&](const Vector3 &p) -> bool
    {
        if(!ckMgr->DetectCollision(AABB{ p - Vector3{ 0.2f, 1.6f, 0.2f },
                                         p + Vector3{ 0.2f, 0.2f, 0.2f } }))
        {
            camera_.SetPosition(p);
            return true;
        }
        return false;
    };

    std::vector<Vector3> tryList =
    {
        pos,
        { oldPos.x, pos.y, pos.z },
        { pos.x, pos.y, oldPos.z },
        { pos.x, oldPos.y, pos.z },
        { oldPos.x, pos.y, oldPos.z },
        { oldPos.x, oldPos.y, pos.z },
        { pos.x, oldPos.y, oldPos.z },
        oldPos
    };

    for(Vector3 &p : tryList)
    {
        if(TryPos(p))
            break;
    }

    //视角转动

    auto clamp = [](float x, float minV, float maxV)
    {
        return (std::min)(maxV, (std::max)(minV, x));
    };

    camera_.SetYaw(camera_.GetYaw() - mouseXSpeed_ * input.GetCursorMovX() * 16.66f);
    camera_.SetPitch(clamp(camera_.GetPitch() - mouseYSpeed_ * input.GetCursorMovY() * 16.66f,
                           -DirectX::XM_PIDIV2 + 0.02f, DirectX::XM_PIDIV2 - 0.02f));

    camera_.UpdateViewProjMatrix();
}
