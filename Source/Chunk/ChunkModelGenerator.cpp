/*================================================================
Filename: ChunkModelGenerator.cpp
Date: 2018.1.16
Created by AirGuanZ
================================================================*/
#include "ChunkModelGenerator.h"

ChunkSectionModels *ChunkModelGenerator::GenerateChunkSectionModel(
    ChunkSection::ChunkData &ckData,
    ChunkSection::ChunkFace &pXData,
    ChunkSection::ChunkFace &nXData,
    ChunkSection::ChunkFace &pYData,
    ChunkSection::ChunkFace &nYData,
    ChunkSection::ChunkFace &pZData,
    ChunkSection::ChunkFace &nZData)
{
    ChunkSectionModels *models = new ChunkSectionModels;

    for(int x = 0; x != CHUNK_SECTION_SIZE; ++x)
    {
        for(int y = 0; y != CHUNK_SECTION_SIZE; ++y)
        {
            for(int z = 0; z != CHUNK_SECTION_SIZE; ++z)
            {

            }
        }
    }

    return models;
}
