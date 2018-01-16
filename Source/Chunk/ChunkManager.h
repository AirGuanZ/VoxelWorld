/*================================================================
Filename: ChunkManager.h
Date: 2018.1.16
Created by AirGuanZ
================================================================*/
#ifndef VW_CHUNK_MANAGER_H
#define VW_CHUNK_MANAGER_H

#include <unordered_map>

#include "../Utility/Math.h"
#include "../Utility/Uncopiable.h"
#include "Chunk.h"
#include "ChunkLoader.h"

class ChunkManager : public Uncopiable
{
public:
    ChunkManager(void);
    ~ChunkManager(void);

    void StartChunkLoading(void);
    void EndChunkLoading(void);

    Chunk *GetChunkByBlockPos(int x, int z);
    Chunk *GetChunkByChunkPos(int x, int z);

private:
    std::unordered_map<IntVector2XZ, Chunk*, IntVector2XZHasher> cks_;
    ChunkLoader ckLoader_;
};

inline IntVector2XZ BlockPosToChunkPos(const IntVector2XZ &pos)
{
    //在|n| <= 2^30范围内符合要求
    constexpr int POS_OFFSET = 1 << 30;
    return {
        (pos.x + POS_OFFSET) / CHUNK_SECTION_SIZE - POS_OFFSET / CHUNK_SECTION_SIZE,
        (pos.z + POS_OFFSET) / CHUNK_SECTION_SIZE - POS_OFFSET / CHUNK_SECTION_SIZE
    };
}

inline int BlockPosToChunkSection(int y)
{
    assert(0 <= y && y < CHUNK_SECTION_SIZE * CHUNK_SECTION_NUM);
    return y / CHUNK_SECTION_SIZE;
}

#endif //VW_CHUNK_MANAGER_H
