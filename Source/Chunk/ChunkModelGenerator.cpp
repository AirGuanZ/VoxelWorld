/*================================================================
Filename: ChunkModelGenerator.cpp
Date: 2018.1.16
Created by AirGuanZ
================================================================*/
#include "../Block/BlockModelGenerator.h"
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

    //���м��Block��ֱ��ȡ��Χ��Block����Model
    for(int x = 1; x != CHUNK_SECTION_SIZE - 1; ++x)
    {
        for(int y = 1; y != CHUNK_SECTION_SIZE - 1; ++y)
        {
            for(int z = 1; z != CHUNK_SECTION_SIZE - 1; ++z)
            {
                GetBlockModelGenerator(ckData[x][y][z])->AddTriangles(
                    ckData[x][y][z],
                    ckData[x + 1][y][z], ckData[x - 1][y][z],
                    ckData[x][y + 1][z], ckData[x][y - 1][z],
                    ckData[x][y][z + 1], ckData[x][y][z - 1],
                    models);
            }
        }
    }

    //��Chunk�������棬�ֱ��Face������ȡ��Ӧ���Block����Model
    //TODO

    return models;
}
