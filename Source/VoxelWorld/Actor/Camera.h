/*================================================================
Filename: Camera.h
Date: 2018.1.14
Created by AirGuanZ
================================================================*/
#ifndef VW_CAMERA_H
#define VW_CAMERA_H

#include <Utility/D3D11Header.h>
#include <Utility/Math.h>

#include <Collision/AABB.h>

class Camera
{
public:
    Camera(void);

    void SetPosition(const Vector3 &pos)
    {
        pos_ = pos;
    }

    const Vector3 &GetPosition(void) const
    {
        return pos_;
    }

    void SetYaw(float radian)
    {
        yaw_ = radian;
    }

    float GetYaw(void) const
    {
        return yaw_;
    }

    void SetPitch(float radian)
    {
        pitch_ = radian;
    }

    float GetPitch(void) const
    {
        return pitch_;
    }

    void SetRoll(float radian)
    {
        roll_ = radian;
    }

    float GetRoll(void) const
    {
        return roll_;
    }

    Vector3 GetDirection(void) const;

    void SetFOVy(float radian)
    {
        FOVy_ = radian;
    }

    float GetFOVy(void) const
    {
        return FOVy_;
    }

    void SetNearPlane(float distance)
    {
        near_ = distance;
    }

    float GetNearPlane(void) const
    {
        return near_;
    }

    void SetFarPlane(float distance)
    {
        far_ = distance;
    }

    float GetFarPlane(void) const
    {
        return far_;
    }

    const Matrix &GetViewMatrix(void) const
    {
        return view_;
    }

    const Matrix &GetProjMatrix(void) const
    {
        return proj_;
    }

    const Matrix &GetViewProjMatrix(void) const
    {
        return viewProj_;
    }

    void UpdateViewProjMatrix(void);

    bool InFrustum(const AABB &aabb) const;

private:
    float yaw_;
    float pitch_;
    float roll_;

    Vector3 pos_;

    float FOVy_;
    float near_;
    float far_;

    Matrix view_;
    Matrix proj_;
    Matrix viewProj_;
};

#endif //VW_CAMERA_H
