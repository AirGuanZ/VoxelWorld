/*================================================================
Filename: Chunk.h
Date: 2018.1.16
Created by AirGuanZ
================================================================*/
#ifndef VW_CHUNK_H
#define VW_CHUNK_H

#include "../Utility/Uncopiable.h"
#include "ChunkSection.h"

constexpr int CHUNK_SECTION_NUM = 16;

class Chunk : public Uncopiable
{
public:
    Chunk(int ckX, int ckY, int ckZ);
    ~Chunk(void);

    DirectX::XMINT3 GetPosition(void) const;

    const ChunkSection &GetChunkSection(int sectionIdx) const;
    ChunkSection &GetChunkSection(int sectionIdx);

private:
    DirectX::XMINT3 ckPos_;
    ChunkSection sections_[CHUNK_SECTION_NUM];
};

#endif //VW_CHUNK_H
