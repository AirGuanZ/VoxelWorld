/*================================================================
Filename: ChunkFile.h
Date: 2018.1.18
Created by AirGuanZ
================================================================*/
#ifndef VW_CHUNK_FILE_H
#define VW_CHUNK_FILE_H

#include <string>

#include "Chunk.h"

class ChunkFile
{
public:
    static bool LoadChunk(const std::wstring &savename, Chunk *chunk);
    static bool SaveChunk(const std::wstring &savename, Chunk *chunk);
};

#endif //VW_CHUNK_FILE_H
