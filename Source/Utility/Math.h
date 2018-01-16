/*================================================================
Filename: Math.h
Date: 2018.1.16
Created by AirGuanZ
================================================================*/
#ifndef VW_MATH_H
#define VW_MATH_H

#include <d3d11.h>
#include <SimpleMath.h>

using DirectX::SimpleMath::Vector2;
using DirectX::SimpleMath::Vector3;
using DirectX::SimpleMath::Vector4;
using DirectX::SimpleMath::Matrix;

struct IntVector2XZ
{
    int x, z;
};

inline bool operator==(const IntVector2XZ &lhs, const IntVector2XZ &rhs) noexcept
{
    return lhs.x == rhs.x && lhs.z == rhs.z;
}

struct IntVector2XZHasher
{
    size_t operator()(const IntVector2XZ &v) const noexcept
    {
        return std::hash<int>()((std::hash<int>()(v.x) ^ std::hash<int>()(v.z)) >> 2);
    }
};

struct IntVector3
{
    int x, y, z;
};

inline bool operator==(const IntVector3 &lhs, const IntVector3 &rhs) noexcept
{
    return lhs.x == rhs.x && lhs.y == rhs.y && lhs.z == rhs.z;
}

struct IntVector3Hasher
{
    size_t operator()(const IntVector3 &v) const noexcept
    {
        std::hash<int> {}(std::hash<int>()(v.x) ^
                          (std::hash<int>()(v.y) << std::hash<int>()(v.z)) ^
                          std::hash<int>()(v.z));
    }
};

#endif //VW_MATH_H
