/*================================================================
Filename: ChunkSection.cpp
Date: 2018.1.16
Created by AirGuanZ
================================================================*/
#include <cassert>
#include <cstring>

#include "ChunkSection.h"

ChunkSection::ChunkSection(void)
    : models_(nullptr), blockChanged_(true)
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
    assert(models_ == nullptr);
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
    return models_ != nullptr;
}

const ChunkSection::ChunkSectionData &ChunkSection::GetBlockData(void) const
{
    return blocks_;
}
