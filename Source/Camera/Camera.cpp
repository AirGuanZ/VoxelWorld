/*================================================================
Filename: Camera.cpp
Date: 2018.1.14
Created by AirGuanZ
================================================================*/
#include "../Window/Window.h"
#include "Camera.h"

Camera::Camera(void)
    : pos_(0.0f, 0.0f, 0.0f)
{
    yaw_ = pitch_ = roll_ = 0.0f;

    FOVy_ = DirectX::XMConvertToRadians(50.0f);
    near_ = 0.1f;
    far_ = 1000.0f;
}

Vector3 Camera::GetDirection(void) const
{
    float cosPitch = cos(pitch_);
    Vector3 rt =
    {
        -cosPitch * sin(yaw_),
        sin(pitch_),
        -cosPitch * cos(yaw_)
    };
    rt.Normalize();
    return rt;
}

void Camera::UpdateViewProjMatrix(void)
{
    view_ = Matrix::CreateTranslation(-pos_) *
        Matrix::CreateRotationY(-yaw_) *
        Matrix::CreateRotationX(-pitch_) *
        Matrix::CreateRotationZ(-roll_);
    proj_ = Matrix::CreatePerspectiveFieldOfView(
        FOVy_, Window::GetInstance().GetClientAspectRatio(), near_, far_);
    viewProj_ = view_ * proj_;
}

namespace
{
    inline bool InPosHalfSpace(const Vector3 &ori, const Vector3 &dir, const Vector3 &dst)
    {
        return dir.Dot(dst - ori) > 0.0f;
    }
}

bool Camera::InFrustum(const AABB &aabb) const
{
    Vector3 dir = GetDirection();
    return (InPosHalfSpace(pos_, dir, aabb.L) |
            InPosHalfSpace(pos_, dir, aabb.H)) ||
           (InPosHalfSpace(pos_, dir, { aabb.H.x, aabb.L.y, aabb.L.z }) |
            InPosHalfSpace(pos_, dir, { aabb.L.x, aabb.H.y, aabb.L.z })) ||
           (InPosHalfSpace(pos_, dir, { aabb.L.x, aabb.L.y, aabb.H.z }) |
            InPosHalfSpace(pos_, dir, { aabb.L.x, aabb.H.y, aabb.H.z })) ||
           (InPosHalfSpace(pos_, dir, { aabb.H.x, aabb.L.y, aabb.H.z }) |
            InPosHalfSpace(pos_, dir, { aabb.H.x, aabb.H.y, aabb.L.z }));
}
