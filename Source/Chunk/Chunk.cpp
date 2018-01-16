/*================================================================
Filename: Chunk.cpp
Date: 2018.1.16
Created by AirGuanZ
================================================================*/
#include "Chunk.h"

Chunk::Chunk(int ckX, int ckY, int ckZ)
    : ckPos_(ckX, ckY, ckZ)
{

}

Chunk::~Chunk(void)
{

}

DirectX::XMINT3 Chunk::GetPosition(void) const
{
    return ckPos_;
}

const ChunkSection &Chunk::GetChunkSection(int idx) const
{
    assert(0 <= idx && idx < CHUNK_SECTION_NUM);
    return sections_[idx];
}

ChunkSection &Chunk::GetChunkSection(int idx)
{
    assert(0 <= idx && idx < CHUNK_SECTION_NUM);
    return sections_[idx];
}
