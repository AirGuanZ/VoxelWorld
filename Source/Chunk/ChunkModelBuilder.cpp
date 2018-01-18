/*================================================================
Filename: ChunkModelBuilder.cpp
Date: 2018.1.18
Created by AirGuanZ
================================================================*/
#include <cassert>

#include "../Block/BlockModelBuilder.h"
#include "ChunkModelBuilder.h"

ChunkModelBuilder::ChunkModelBuilder(ChunkManager *ckMgr, ChunkSection *section)
    : ckMgr_(ckMgr), section_(section)
{
    assert(ckMgr != nullptr && section != nullptr);
}

void ChunkModelBuilder::Build(void)
{

}
