/*================================================================
Filename: ChunkModelBuilder.h
Date: 2018.1.18
Created by AirGuanZ
================================================================*/
#pragma once

#include <Utility\Uncopiable.h>

#include <World\Block\BlockModelBuilder.h>

class ChunkManager;

class ChunkModelBuilder : public Uncopiable
{
public:
    ChunkModelBuilder(ChunkManager *ckMgr, Chunk *ck, int section);

    ChunkSectionModels *Build(void);

private:
    ChunkManager *ckMgr_;
    Chunk *ck_;
    int section_;
};

class BackgroundChunkModelBuilder : public Uncopiable
{
public:
    ChunkSectionModels *Build(Chunk *(&cks)[3][3], int section) const;
};
