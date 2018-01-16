/*================================================================
Filename: ChunkSection.cpp
Date: 2018.1.16
Created by AirGuanZ
================================================================*/
#include <cassert>
#include <cstring>

#include "ChunkSection.h"

ChunkSection::ChunkSection(void)
    : basicModel_(nullptr), blockChanged_(true)
{
    for(int x = 0; x != CHUNK_SECTION_SIZE; ++x)
    {
        for(int y = 0; y != CHUNK_SECTION_SIZE; ++y)
        {
            for(int z = 0; z != CHUNK_SECTION_SIZE; ++z)
                blocks_[x][y][z].type = BlockType::Air;
        }
    }
}

ChunkSection::~ChunkSection(void)
{
    assert(basicModel_ == nullptr);
}

static inline void AssertValidPositionInSection(int x, int y, int z)
{
    assert(0 <= x && x < CHUNK_SECTION_SIZE);
    assert(0 <= y && y < CHUNK_SECTION_SIZE);
    assert(0 <= z && z < CHUNK_SECTION_SIZE);
}

void ChunkSection::SetBlock(int x, int y, int z, const Block &block)
{
    AssertValidPositionInSection(x, y, z);
    blocks_[x][y][z] = block;
    blockChanged_ = true;
}

const Block &ChunkSection::GetBlock(int x, int y, int z) const
{
    AssertValidPositionInSection(x, y, z);
    return blocks_[x][y][z];
}

Block &ChunkSection::GetBlock(int x, int y, int z)
{
    AssertValidPositionInSection(x, y, z);
    return blocks_[x][y][z];
}

bool ChunkSection::IsModelsAvailable(void) const
{
    return basicModel_ != nullptr;
}

void ChunkSection::GetBlockData(Block (&data)[CHUNK_SECTION_SIZE][CHUNK_SECTION_SIZE][CHUNK_SECTION_SIZE]) const
{
    std::memcpy(data, blocks_, sizeof(blocks_));
}

void ChunkSection::GetPosXFaceBlockData(Block (&data)[CHUNK_SECTION_SIZE][CHUNK_SECTION_SIZE]) const
{
    std::memcpy(data, blocks_[CHUNK_SECTION_SIZE - 1], sizeof(blocks_[CHUNK_SECTION_SIZE - 1]));
}

void ChunkSection::GetNegXFaceBlockData(Block (&data)[CHUNK_SECTION_SIZE][CHUNK_SECTION_SIZE]) const
{
    std::memcpy(data, blocks_[0], sizeof(blocks_[0]));
}

void ChunkSection::GetPosYFaceBlockData(Block (&data)[CHUNK_SECTION_SIZE][CHUNK_SECTION_SIZE]) const
{
    for(int x = 0; x != CHUNK_SECTION_SIZE; ++x)
        std::memcpy(data[x], blocks_[x][CHUNK_SECTION_SIZE - 1], sizeof(blocks_[x][CHUNK_SECTION_SIZE]));
}

void ChunkSection::GetNegYFaceBlockData(Block (&data)[CHUNK_SECTION_SIZE][CHUNK_SECTION_SIZE]) const
{
    for(int x = 0; x != CHUNK_SECTION_SIZE; ++x)
        std::memcpy(data[x], blocks_[x][0], sizeof(blocks_[x][0]));
}

void ChunkSection::GetPosZFaceBlockData(Block (&data)[CHUNK_SECTION_SIZE][CHUNK_SECTION_SIZE]) const
{
    for(int x = 0; x != CHUNK_SECTION_SIZE; ++x)
    {
        for(int y = 0; y != CHUNK_SECTION_SIZE; ++y)
            data[x][y] = blocks_[x][y][CHUNK_SECTION_SIZE - 1];
    }
}

void ChunkSection::GetNegZFaceBlockData(Block (&data)[CHUNK_SECTION_SIZE][CHUNK_SECTION_SIZE]) const
{
    for(int x = 0; x != CHUNK_SECTION_SIZE; ++x)
    {
        for(int y = 0; y != CHUNK_SECTION_SIZE; ++y)
            data[x][y] = blocks_[x][y][0];
    }
}
