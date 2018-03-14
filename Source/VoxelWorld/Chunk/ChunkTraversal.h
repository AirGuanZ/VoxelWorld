/*================================================================
Filename: ChunkTraversal.h
Date: 2018.1.25
Created by AirGuanZ
================================================================*/
#pragma once

#include <vector>

#include <Utility\Math.h>
#include <Utility\Singleton.h>

#include "Chunk.h"

class ChunkTraversal : public Singleton<ChunkTraversal>
{
public:
    ChunkTraversal(void);

    const std::vector<IntVectorXZ> &ChunkXZEdge(void) const;
    const std::vector<IntVectorXZ> &ChunkXZInternal(void) const;
    const std::vector<IntVector3>  &ChunkFaces(void) const;

private:
    std::vector<IntVectorXZ> XZEdge_;
    std::vector<IntVectorXZ> XZInternal_;
    std::vector<IntVector3>  XYZFaces_;
};
