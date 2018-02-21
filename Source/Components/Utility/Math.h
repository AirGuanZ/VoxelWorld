/*================================================================
Filename: Math.h
Date: 2018.1.16
Created by AirGuanZ
================================================================*/
#ifndef VW_MATH_H
#define VW_MATH_H

#include <cmath>
#include <d3d11.h>
#include <DirectXTK/SimpleMath.h>

using DirectX::SimpleMath::Vector2;
using DirectX::SimpleMath::Vector3;
using DirectX::SimpleMath::Vector4;
using DirectX::SimpleMath::Matrix;
using DirectX::SimpleMath::Quaternion;
using DirectX::SimpleMath::Color;

struct IntVectorXZ
{
    IntVectorXZ(int _x = 0, int _z = 0)
        : x(_x), z(_z)
    {

    }
    int x, z;
};

inline bool operator==(const IntVectorXZ &lhs, const IntVectorXZ &rhs) noexcept
{
    return lhs.x == rhs.x && lhs.z == rhs.z;
}

inline bool operator<(const IntVectorXZ &lhs, const IntVectorXZ &rhs) noexcept
{
    return lhs.x < rhs.x || (lhs.x == rhs.x && lhs.z < rhs.z);
}

inline IntVectorXZ operator+(const IntVectorXZ &lhs, const IntVectorXZ &rhs) noexcept
{
    return { lhs.x + rhs.x, lhs.z + rhs.z };
}

inline IntVectorXZ operator*(int lhs, const IntVectorXZ &rhs) noexcept
{
    return { lhs * rhs.x, lhs * rhs.z };
}

inline float Distance(const IntVectorXZ &L, const IntVectorXZ &R) noexcept
{
    return std::sqrt(static_cast<float>(
        (L.x - R.x) * (L.x - R.x) + (L.z - R.z) * (L.z - R.z)));
}

struct IntVectorXZHasher
{
    size_t operator()(const IntVectorXZ &v) const noexcept
    {
        return std::hash<size_t>()((std::hash<int>()(v.x) ^ std::hash<int>()(v.z)) >> 2);
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

inline bool operator<(const IntVector3 &lhs, const IntVector3 &rhs) noexcept
{
    return lhs.x < rhs.x || (lhs.x == rhs.x && lhs.y < rhs.y) ||
           (lhs.x == rhs.x && lhs.y == rhs.y && lhs.z < rhs.z);
}

inline IntVector3 operator+(const IntVector3 &L, const IntVector3 &R) noexcept
{
    return { L.x + R.x, L.y + R.y, L.z + R.z };
}

struct IntVector3Hasher
{
    size_t operator()(const IntVector3 &v) const noexcept
    {
        return std::hash<size_t> {}(std::hash<int>()(v.x) ^
                                    (std::hash<int>()(v.y) << std::hash<int>()(v.z)) ^
                                    std::hash<int>()(v.z));
    }
};

constexpr float PI = DirectX::XM_PI;
constexpr float PIDIV2 = DirectX::XM_PIDIV2;
constexpr float _2PI = DirectX::XM_2PI;

#endif //VW_MATH_H
