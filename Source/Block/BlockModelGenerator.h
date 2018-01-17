/*================================================================
Filename: BlockModelGenerator.h
Date: 2018.1.17
Created by AirGuanZ
================================================================*/
#ifndef VW_BLOCK_MODEL_GENERATOR_H
#define VW_BLOCK_MODEL_GENERATOR_H

#include "../Chunk/Chunk.h"
#include "../Model/BasicModel.h"
#include "Block.h"
#include "BlockInfo.h"

class BlockModelGenerator
{
public:
    virtual void AddTriangles(
        const Vector3 &posOffset,
        const Block &blk,
        const Block &pX, const Block &nX,
        const Block &pY, const Block &nY,
        const Block &pZ, const Block &nZ,
        ChunkSectionModels *models) const = 0;
};

const BlockModelGenerator *GetBlockModelGenerator(const Block &block);

class BlockModelGenerator_Null : public BlockModelGenerator
{
public:
    void AddTriangles(
        const Vector3 &posOffset,
        const Block &blk,
        const Block &pX, const Block &nX,
        const Block &pY, const Block &nY,
        const Block &pZ, const Block &nZ,
        ChunkSectionModels *models) const;
};

class BlockModelGenerator_BasicRenderer_Box : public BlockModelGenerator
{
public:
    void AddTriangles(
        const Vector3 &posOffset,
        const Block &blk,
        const Block &pX, const Block &nX,
        const Block &pY, const Block &nY,
        const Block &pZ, const Block &nZ,
        ChunkSectionModels *models) const;
};

#endif //VW_BLOCK_MODEL_GENERATOR_H
