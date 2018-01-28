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
        : L(L), H(H)
    {

    }

    bool IsPointIn(const Vector3 &pnt) const
    {
        return L.x <= pnt.x && pnt.x <= H.x &&
               L.y <= pnt.y && pnt.y <= H.y &&
               L.z <= pnt.z && pnt.z <= H.z;
    }

    bool IsAABBIntersected(const AABB &other) const
    {
        return IsValid() && other.IsValid() &&
               SegInct(L.x, H.x, other.L.x, other.H.x) &&
               SegInct(L.y, H.y, other.L.y, other.H.y) &&
               SegInct(L.z, H.z, other.L.z, other.H.z);
    }

    bool IsValid(void) const
    {
        return L.x <= H.x && L.y <= H.y && L.z <= H.z;
    }

    Vector3 L, H;

private:
    bool SegInct(float L1, float H1, float L2, float H2) const
    {
        return (L2 <= L1 && L1 <= H2) ||
               (L2 <= H1 && H1 <= H2) ||
               (L1 <= L2 && H2 <= H1);
    }
};

inline AABB operator+(const AABB &aabb, const Vector3 &offset)
{
    return { aabb.L + offset, aabb.H + offset };
}

#endif //VW_COLLISION_H
