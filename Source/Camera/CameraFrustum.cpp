/*================================================================
Filename: CameraFrustum.cpp
Date: 2018.1.21
Created by AirGuanZ
================================================================*/
#include <cmath>

#include "CameraFrustum.h"

void CameraFrustum::Update(float FOVy, float aspectRatio)
{
    float theta = 0.5f * FOVy;
    float cosT = std::cos(theta);
    float sinT = std::sin(theta);

    float aTanT = aspectRatio * sinT / cosT;
    float cosP = 1.0f / std::sqrt(1.0f + aTanT);
    float sinP = aTanT * cosP;

    planes_[0] = Vector3(0.0f, cosT, sinT);
    planes_[1] = Vector3(0.0f, -cosT, sinT);
    planes_[2] = Vector3(-cosP, sinP, 0.0f);
    planes_[3] = Vector3(cosP, sinP, 0.0f);
}

bool CameraFrustum::Contain(const Matrix &view, const Vector3 &L, const Vector3 &H) const
{
    auto SingleIn = [&](const Vector3 &p) -> bool
    {
        Vector4 _vp = Vector4::Transform(Vector4(p.x, p.y, p.z, 1.0f), view);
        Vector3 vp(_vp.x, _vp.y, _vp.z);
        return vp.Dot(planes_[0]) < 0.0f && vp.Dot(planes_[1]) < 0.0f &&
               vp.Dot(planes_[2]) < 0.0f && vp.Dot(planes_[3]) < 0.0f;
    };

    return SingleIn(L) || SingleIn({ H.x, L.y, L.z }) || SingleIn({ L.x, H.y, L.z }) ||
           SingleIn({ L.x, L.y, H.z }) || SingleIn({ H.x, H.y, L.z }) ||
           SingleIn({ H.x, L.y, H.z }) || SingleIn({ L.x, H.y, H.z }) ||
           SingleIn(H);
}
