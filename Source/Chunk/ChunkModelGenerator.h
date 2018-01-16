/*================================================================
Filename: ChunkModelGenerator.h
Date: 2018.1.16
Created by AirGuanZ
================================================================*/
#ifndef VW_CHUNK_MODEL_GENERATOR_H
#define VW_CHUNK_MODEL_GENERATOR_H

#include "../Model/Model.h"
#include "Chunk.h"

class ChunkModelGenerator
{
public:
    static Model *GenerateChunkSectionModel(
        Block (&ckData)[CHUNK_SECTION_SIZE][CHUNK_SECTION_SIZE][CHUNK_SECTION_SIZE],
        Block (&pXData)[CHUNK_SECTION_SIZE][CHUNK_SECTION_SIZE],
        Block (&nXData)[CHUNK_SECTION_SIZE][CHUNK_SECTION_SIZE],
        Block (&pYData)[CHUNK_SECTION_SIZE][CHUNK_SECTION_SIZE],
        Block (&nYData)[CHUNK_SECTION_SIZE][CHUNK_SECTION_SIZE],
        Block (&pZData)[CHUNK_SECTION_SIZE][CHUNK_SECTION_SIZE],
        Block (&nZData)[CHUNK_SECTION_SIZE][CHUNK_SECTION_SIZE]);
};

#endif //VW_CHUNK_MODEL_GENERATOR_H
