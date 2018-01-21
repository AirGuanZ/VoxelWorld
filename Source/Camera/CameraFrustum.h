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

    //һ��AABB Box�Ƿ����ӽ�����
    bool Contain(const Matrix &view, const Vector3 &L, const Vector3 &H) const;

private:
    //up, down, left, right
    //��ԭ���ƽ��ֻ��Ҫ����������
    Vector3 planes_[4];
};

#endif //VW_CAMERA_FRUSTUM_H
