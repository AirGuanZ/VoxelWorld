/*================================================================
Filename: ChunkModelGenerator.cpp
Date: 2018.1.16
Created by AirGuanZ
================================================================*/
#include "../Block/BlockModelGenerator.h"
#include "ChunkModelGenerator.h"

ChunkSectionModels *ChunkModelGenerator::GenerateChunkSectionModel(
    const Vector3 &sectionOffset,
    ChunkSection::ChunkData &ckData,
    ChunkSection::ChunkFace &pXData,
    ChunkSection::ChunkFace &nXData,
    ChunkSection::ChunkFace &pYData,
    ChunkSection::ChunkFace &nYData,
    ChunkSection::ChunkFace &pZData,
    ChunkSection::ChunkFace &nZData)
{
    ChunkSectionModels *models = new ChunkSectionModels;

    //对中间的Block，直接取周围的Block建立Model
    for(int x = 1; x != CHUNK_SECTION_SIZE - 1; ++x)
    {
        for(int y = 1; y != CHUNK_SECTION_SIZE - 1; ++y)
        {
            for(int z = 1; z != CHUNK_SECTION_SIZE - 1; ++z)
            {
                GetBlockModelGenerator(ckData[x][y][z])->AddTriangles(
                    sectionOffset + Vector3(static_cast<float>(x),
                                            static_cast<float>(y),
                                            static_cast<float>(z)),
                    ckData[x][y][z],
                    ckData[x + 1][y][z], ckData[x - 1][y][z],
                    ckData[x][y + 1][z], ckData[x][y - 1][z],
                    ckData[x][y][z + 1], ckData[x][y][z - 1],
                    models);
            }
        }
    }

    //对Chunk的六个面内部，分别从Face数据中取对应面的Block建立Model

    for(int y = 1; y != CHUNK_SECTION_SIZE - 1; ++y)
    {
        for(int z = 1; z != CHUNK_SECTION_SIZE - 1; ++z)
        {
            GetBlockModelGenerator(ckData[CHUNK_SECTION_SIZE - 1][y][z])->AddTriangles(
                sectionOffset + Vector3(static_cast<float>(CHUNK_SECTION_SIZE - 1),
                                        static_cast<float>(y),
                                        static_cast<float>(z)),
                ckData[CHUNK_SECTION_SIZE - 1][y][z],
                pXData[y][z], ckData[CHUNK_SECTION_SIZE - 2][y][z],
                ckData[CHUNK_SECTION_SIZE - 1][y + 1][z], ckData[CHUNK_SECTION_SIZE - 1][y - 1][z],
                ckData[CHUNK_SECTION_SIZE - 1][y][z + 1], ckData[CHUNK_SECTION_SIZE - 1][y][z - 1],
                models);
                
            GetBlockModelGenerator(ckData[0][y][z])->AddTriangles(
                sectionOffset + Vector3(0.0f,
                                        static_cast<float>(y),
                                        static_cast<float>(z)),
                ckData[0][y][z],
                ckData[1][y][z], nXData[y][z],
                ckData[0][y + 1][z], ckData[0][y - 1][z],
                ckData[0][y][z + 1], ckData[0][y][z - 1],
                models);
        }
    }
    
    for(int x = 1; x != CHUNK_SECTION_SIZE - 1; ++x)
    {
        for(int z = 1; z != CHUNK_SECTION_SIZE - 1; ++z)
        {
            GetBlockModelGenerator(ckData[x][CHUNK_SECTION_SIZE - 1][z])->AddTriangles(
                sectionOffset + Vector3(static_cast<float>(x),
                                        static_cast<float>(CHUNK_SECTION_SIZE - 1),
                                        static_cast<float>(z)),
                ckData[x][CHUNK_SECTION_SIZE - 1][z],
                ckData[x + 1][CHUNK_SECTION_SIZE - 1][z], ckData[x - 1][CHUNK_SECTION_SIZE - 1][z],
                pYData[x][z], ckData[x][CHUNK_SECTION_SIZE - 2][z],
                ckData[x][CHUNK_SECTION_SIZE - 1][z + 1], ckData[x][CHUNK_SECTION_SIZE - 1][z - 1],
                models);
                
            GetBlockModelGenerator(ckData[x][0][z])->AddTriangles(
                sectionOffset + Vector3(static_cast<float>(x),
                                        0.0f,
                                        static_cast<float>(z)),
                ckData[x][0][z],
                ckData[x + 1][0][z], ckData[x - 1][0][z],
                ckData[x][1][z], nYData[x][z],
                ckData[x][0][z + 1], ckData[x][0][z - 1],
                models);
        }
    }
    
    for(int x = 1; x != CHUNK_SECTION_SIZE - 1; ++x)
    {
        for(int y = 1; y != CHUNK_SECTION_SIZE - 1; ++y)
        {
            GetBlockModelGenerator(ckData[x][y][CHUNK_SECTION_SIZE - 1])->AddTriangles(
                sectionOffset + Vector3(static_cast<float>(x),
                                        static_cast<float>(y),
                                        static_cast<float>(CHUNK_SECTION_SIZE - 1)),
                ckData[x][y][CHUNK_SECTION_SIZE - 1],
                ckData[x + 1][y][CHUNK_SECTION_SIZE - 1], ckData[x - 1][y][CHUNK_SECTION_SIZE - 1],
                ckData[x][y + 1][CHUNK_SECTION_SIZE + 1], ckData[x][y - 1][CHUNK_SECTION_SIZE - 1],
                pZData[x][y], ckData[x][y][CHUNK_SECTION_SIZE - 2],
                models);
                
            GetBlockModelGenerator(ckData[x][y][0])->AddTriangles(
                sectionOffset + Vector3(static_cast<float>(x),
                                        static_cast<float>(y),
                                        0.0f),
                ckData[x][y][0],
                ckData[x + 1][y][0], ckData[x - 1][y][0],
                ckData[x][y + 1][0], ckData[x][y - 1][0],
                ckData[x][y][1], nZData[x][y],
                models);
        }
    }
    
    //对Chunk的12条棱，分别建立起Model
    
    

    return models;
}
