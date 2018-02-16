/*================================================================
Filename: Actor.cpp
Date: 2018.1.19
Created by AirGuanZ
================================================================*/
#include <algorithm>
#include <array>

#include "../Input/InputManager.h"
#include "Actor.h"

bool Actor::Initialize(std::string &errMsg)
{
    flyUpSpeed_ = 0.009f;
    flyDownSpeed_ = 0.009f;
    horMoveSpeed_ = 0.0085f;

    mouseXSpeed_ = 0.00014f;
    mouseYSpeed_ = 0.00010f;

    pos_ = Vector3(0.0f, 80.0f, 0.0f);
    dstYaw_ = 0.0f;
    actYaw_ = 0.0f;

    if(!model_.Initialize(errMsg))
        return false;
    model_.SetAnimationClip("Moving", true);

    return true;
}

void Actor::Update(float deltaT, ChunkManager *ckMgr)
{
    UpdateCameraDirection(deltaT, ckMgr);
    UpdateActorPosition  (deltaT, ckMgr);
    UpdateCameraApperance(deltaT, ckMgr);
    model_.Update(deltaT);
}

void Actor::Render(void)
{
    model_.Render(camera_);
}

void Actor::UpdateCameraDirection(float deltaT, ChunkManager *ckMgr)
{
    assert(ckMgr != nullptr);

    InputManager &input = InputManager::GetInstance();

    auto clamp = [](float x, float minV, float maxV)
    {
        return (std::min)(maxV, (std::max)(minV, x));
    };

    camera_.SetYaw(camera_.GetYaw() - mouseXSpeed_ * input.GetCursorMovX() * 16.66f);
    camera_.SetPitch(clamp(camera_.GetPitch() - mouseYSpeed_ * input.GetCursorMovY() * 16.66f,
                           -DirectX::XM_PIDIV2 + 0.02f,
                           +DirectX::XM_PIDIV2 - 0.02f));
}

void Actor::UpdateActorPosition(float deltaT, ChunkManager *ckMgr)
{
    InputManager &input = InputManager::GetInstance();

    Vector3 dir = camera_.GetDirection();

    //移动前备份位置坐标，用于其后的碰撞恢复
    Vector3 oldPos = pos_;

    //垂直移动

    if(input.IsKeyDown(VK_SPACE))
        pos_.y += 16.66f * flyUpSpeed_;
    if(input.IsKeyDown(VK_LSHIFT))
        pos_.y -= 16.66f * flyDownSpeed_;

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

    if(horFBMove || horLRMove)
    {
        static const std::array<float, 9> yawOffsets =
        {
            1.5f * DirectX::XM_PIDIV2, DirectX::XM_PI, -1.5f * DirectX::XM_PIDIV2,
            DirectX::XM_PIDIV2, 0.0f, -DirectX::XM_PIDIV2,
            0.5f * DirectX::XM_PIDIV2, 0.0, -0.5f * DirectX::XM_PIDIV2

        };
        dstYaw_ = -camera_.GetYaw() + yawOffsets[3 * (horFBMove + 1) + (horLRMove + 1)];
    }

    horMove.Normalize();
    pos_ += horMoveSpeed_ * horMove * 16.66f;
    if(input.IsKeyDown(VK_LCONTROL))
        pos_ += horMoveSpeed_ * horMove * (16.66f * 4.0f);

    //碰撞检测

    std::vector<Vector3> tryList =
    {
        pos_,
        { oldPos.x, pos_.y,   pos_.z },
        { pos_.x,   pos_.y,   oldPos.z },
        { pos_.x,   oldPos.y, pos_.z },
        { oldPos.x, pos_.y,   oldPos.z },
        { oldPos.x, oldPos.y, pos_.z },
        { pos_.x,   oldPos.y, oldPos.z },
        oldPos
    };

    //将一个位置设置为摄像机新位置，若它不会发生碰撞
    auto TryPos = [&](const Vector3 &p) -> bool
    {
        if(!ckMgr->DetectCollision(AABB{ p - Vector3{ 0.2f, 0.4f, 0.2f },
                                         p + Vector3{ 0.2f, 1.2f, 0.2f } }))
        {
            pos_ = p;
            return true;
        }
        return false;
    };

    for(Vector3 &p : tryList)
    {
        if(TryPos(p))
            break;
    }

    //根据dstYaw计算actYaw的变化

    float deltaYaw = dstYaw_ - actYaw_;
    while(deltaYaw > DirectX::XM_PI)
        deltaYaw -= DirectX::XM_2PI;
    while(deltaYaw < -DirectX::XM_PI)
        deltaYaw += DirectX::XM_2PI;

    constexpr float ACT_YAW_TURN_SPEED = 0.166667f;
    if(std::abs(deltaYaw) > 0.95 * DirectX::XM_PI)
        actYaw_ = dstYaw_;
    else
    {
        if(deltaYaw > 0.0f)
            actYaw_ = (std::min)(actYaw_ + ACT_YAW_TURN_SPEED, actYaw_ + deltaYaw);
        else if(deltaYaw < 0.0f)
            actYaw_ = (std::max)(actYaw_ - ACT_YAW_TURN_SPEED, actYaw_ + deltaYaw);
    }

    model_.SetTransform(
        Matrix::CreateFromAxisAngle({ 0.0f, 1.0f, 0.0f }, -actYaw_) *
        Matrix::CreateTranslation(pos_));
}

void Actor::UpdateCameraApperance(float deltaT, ChunkManager *ckMgr)
{
    camera_.SetPosition(pos_ - 5.0f * camera_.GetDirection());
    camera_.UpdateViewProjMatrix();
}
