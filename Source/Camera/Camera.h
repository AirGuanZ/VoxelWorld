/*================================================================
Filename: Camera.h
Date: 2018.1.14
Created by AirGuanZ
================================================================*/
#ifndef VW_CAMERA_H
#define VW_CAMERA_H

#include "../Utility/D3D11Header.h"

class Camera
{
public:
    Camera(void);

    void SetPosition(const Vector3 &pos);
    const Vector3 &GetPosition(void) const;

    void SetYaw(float radian);
    float GetYaw(void) const;

    void SetPitch(float radian);
    float GetPitch(void) const;

    void SetRoll(float radian);
    float GetRoll(void) const;

    void SetFOVy(float radian);
    float GetFOVy(void) const;

    void SetNearPlane(float distance);
    float GetNearPlane(void) const;

    void SetFarPlane(float distance);
    float GetFarPlane(void) const;

    const Matrix &GetViewMatrix(void) const;
    const Matrix &GetProjMatrix(void) const;
    const Matrix &GetViewProjMatrix(void) const;

    void UpdateViewProjMatrix(void);

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
