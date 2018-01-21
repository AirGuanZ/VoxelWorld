/*================================================================
Filename: CameraFrustum.h
Date: 2018.1.21
Created by AirGuanZ
================================================================*/
#ifndef VW_CAMERA_FRUSTUM_H
#define VW_CAMERA_FRUSTUM_H

#include "../Utility/Math.h"

class CameraFrustum
{
public:
    void Update(float FOVy, float aspectRatio);

    //一个AABB Box是否在视截体内
    bool Contain(const Matrix &view, const Vector3 &L, const Vector3 &H) const;

private:
    //up, down, left, right
    //过原点的平面只需要法向量表征
    Vector3 planes_[4];
};

#endif //VW_CAMERA_FRUSTUM_H
