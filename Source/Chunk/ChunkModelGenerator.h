/*================================================================
Filename: ChunkModelGenerator.h
Date: 2018.1.16
Created by AirGuanZ
================================================================*/
#ifndef VW_CHUNK_MODEL_GENERATOR_H
#define VW_CHUNK_MODEL_GENERATOR_H

#include "../Model/BasicModel.h"
#include "Chunk.h"

class ChunkModelGenerator
{
public:
    static ChunkSectionModels *GenerateChunkSectionModel(
        ChunkSection::ChunkData &ckData,
        ChunkSection::ChunkFace &pXData,
        ChunkSection::ChunkFace &nXData,
        ChunkSection::ChunkFace &pYData,
        ChunkSection::ChunkFace &nYData,
        ChunkSection::ChunkFace &pZData,
        ChunkSection::ChunkFace &nZData);
};

#endif //VW_CHUNK_MODEL_GENERATOR_H
