/*================================================================
Filename: AABB.h
Date: 2018.1.28
Created by AirGuanZ
================================================================*/
#ifndef VW_COLLISION_H
#define VW_COLLISION_H

#include "../Utility/Math.h"

class AABB
{
public:
    AABB(void) = default;

    AABB(const Vector3 &L, const Vector3 &H)
        : L_(L), H_(H)
    {

    }

    bool IsPointIn(const Vector3 &pnt) const
    {
        return L_.x <= pnt.x && pnt.x <= H_.x &&
            L_.y <= pnt.y && pnt.y <= H_.y &&
            L_.z <= pnt.z && pnt.z <= H_.z;
    }

private:
    Vector3 L_, H_;
};

#endif //VW_COLLISION_H
