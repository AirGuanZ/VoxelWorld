/*================================================================
Filename: Block.h
Date: 2018.1.15
Created by AirGuanZ
================================================================*/
#ifndef VW_BLOCK_H
#define VW_BLOCK_H

#include <cstdint>
#include <type_traits>

#include "../Utility/Math.h"
#include "BlockInfo.h"

//red, green, blue, sunlight四个分量，每个分量四位
//red: 12-15
//green: 8-11
//blue: 4-7
//sunlight: 0-3
using BlockLight = std::uint16_t;

enum BlockFace : std::uint8_t
{
    PosX = 0, NegX = 1,
    PosY = 2, NegY = 3,
    PosZ = 4, NegZ = 5
};

struct Block
{
    BlockType type = BlockType::Air;
    //注意这里的亮度不是方块本身的亮度，而是经由自身流出的亮度
    BlockLight rgbs = 0;
};

static_assert(std::is_trivially_copyable_v<Block>, "Block type must be trivially copiable");

inline std::uint8_t GetRed     (BlockLight bl) { return bl >> 12; }
inline std::uint8_t GetGreen   (BlockLight bl) { return (bl >> 8) & 0x0F; }
inline std::uint8_t GetBlue    (BlockLight bl) { return (bl >> 4) & 0x0F; }
inline std::uint8_t GetSunlight(BlockLight bl) { return bl & 0x0F; }

inline BlockLight SetRed     (BlockLight bl, std::uint8_t r) { return (r << 12) | (bl & 0x0FFF); }
inline BlockLight SetGreen   (BlockLight bl, std::uint8_t g) { return (g << 8) | (bl & 0xF0FF); }
inline BlockLight SetBlue    (BlockLight bl, std::uint8_t b) { return (b << 4) | (bl & 0xFF0F); }
inline BlockLight SetSunlight(BlockLight bl, std::uint8_t s) { return s | (bl & 0xFFF0); }

inline float LightToFloat(std::uint8_t component) { return static_cast<float>(component + 2) / (0x0F + 2); }

inline void SetRed     (Block &blk, std::uint8_t r) { blk.rgbs = SetRed(blk.rgbs, r); }
inline void SetGreen   (Block &blk, std::uint8_t g) { blk.rgbs = SetGreen(blk.rgbs, g); }
inline void SetBlue    (Block &blk, std::uint8_t b) { blk.rgbs = SetBlue(blk.rgbs, b); }
inline void SetSunlight(Block &blk, std::uint8_t s) { blk.rgbs = SetSunlight(blk.rgbs, s); }

inline void SetLight(Block &blk, std::uint8_t r, std::uint8_t g, std::uint8_t b, std::uint8_t s)
{
    blk.rgbs = (r << 12) | (g << 8) | (b << 4) | s;
}

inline Color LightToRGBA(BlockLight rgbs)
{
    return { LightToFloat(GetRed(rgbs)),
             LightToFloat(GetGreen(rgbs)),
             LightToFloat(GetBlue(rgbs)),
             LightToFloat(GetSunlight(rgbs)) };
}

constexpr std::uint8_t LIGHT_COMPONENT_MAX = 0x0F;

#endif //VW_BLOCK_H
