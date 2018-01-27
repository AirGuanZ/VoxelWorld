/*================================================================
Filename: BlockModelBuilder.h
Date: 2018.1.18
Created by AirGuanZ
================================================================*/
#ifndef VW_BLOCK_MODEL_BUILDER_H
#define VW_BLOCK_MODEL_BUILDER_H

#include "../Chunk/Chunk.h"

constexpr float BLOCK_SIDE_BOTTOM_LIGHT_DEC_RATIO = 0.93f;

class BlockModelBuilder
{
public:
    virtual void Build(
        const Vector3 &posOffset,
        const Block (&blks)[3][3][3],
        ChunkSectionModels *models) const = 0;
};

const BlockModelBuilder *GetBlockModelBuilder(BlockType type);

class BlockModelBuilder_Null : public BlockModelBuilder
{
public:
    void Build(
        const Vector3 &posOffset,
        const Block(&blks)[3][3][3],
        ChunkSectionModels *models) const;
};

class BlockModelBuilder_BasicRenderer_Box : public BlockModelBuilder
{
public:
    void Build(
        const Vector3 &posOffset,
        const Block(&blks)[3][3][3],
        ChunkSectionModels *models) const;
};

class BlockModelBuilder_CarveRenderer_Box : public BlockModelBuilder
{
public:
    void Build(
        const Vector3 &posOffset,
        const Block(&blks)[3][3][3],
        ChunkSectionModels *models) const;
};

class BlockModelBuilder_CarveRenderer_Cross : public BlockModelBuilder
{
public:
    void Build(
        const Vector3 &posOffset,
        const Block(&blks)[3][3][3],
        ChunkSectionModels *models) const;
};

class BlockModelBuilder_TransLiquidRenderer_Liquid : public BlockModelBuilder
{
public:
    void Build(
        const Vector3 &posOffset,
        const Block(&blks)[3][3][3],
        ChunkSectionModels *models) const;
};

#endif //VW_BLOCK_MODEL_BUILDER_H
