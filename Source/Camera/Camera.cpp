/*================================================================
Filename: Camera.cpp
Date: 2018.1.14
Created by AirGuanZ
================================================================*/
#include "../Window/Window.h"
#include "Camera.h"

Camera::Camera(void)
    : pos_(0.0f, 150.0f, 0.0f)
{
    yaw_ = pitch_ = roll_ = 0.0f;

    FOVy_ = DirectX::XMConvertToRadians(45.0f);
    near_ = 0.1f;
    far_ = 1000.0f;
}

void Camera::SetPosition(const Vector3 &pos)
{
    pos_ = pos;
}

const Vector3 &Camera::GetPosition(void) const
{
    return pos_;
}

void Camera::SetYaw(float radian)
{
    yaw_ = radian;
}

float Camera::GetYaw(void) const
{
    return yaw_;
}

void Camera::SetPitch(float radian)
{
    pitch_ = radian;
}

float Camera::GetPitch(void) const
{
    return pitch_;
}

void Camera::SetRoll(float radian)
{
    roll_ = radian;
}

float Camera::GetRoll(void) const
{
    return roll_;
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

void Camera::SetFOVy(float radian)
{
    FOVy_ = radian;
}

float Camera::GetFOVy(void) const
{
    return FOVy_;
}

void Camera::SetNearPlane(float distance)
{
    near_ = distance;
}

float Camera::GetNearPlane(void) const
{
    return near_;
}

void Camera::SetFarPlane(float distance)
{
    far_ = distance;
}

float Camera::GetFarPlane(void) const
{
    return far_;
}

const Matrix &Camera::GetViewMatrix(void) const
{
    return view_;
}

const Matrix &Camera::GetProjMatrix(void) const
{
    return proj_;
}

const Matrix &Camera::GetViewProjMatrix(void) const
{
    return viewProj_;
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
