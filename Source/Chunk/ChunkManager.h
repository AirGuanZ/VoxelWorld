/*================================================================
Filename: ChunkManager.h
Date: 2018.1.16
Created by AirGuanZ
================================================================*/
#ifndef VW_CHUNK_MANAGER_H
#define VW_CHUNK_MANAGER_H

#include "../Utility/Uncopiable.h"
#include "Chunk.h"
#include "ChunkLoader.h"

class ChunkManager : public Uncopiable
{
public:
    ChunkManager(void);
    ~ChunkManager(void);

private:
    ChunkLoader ckLoader_;
};

#endif //VW_CHUNK_MANAGER_H
