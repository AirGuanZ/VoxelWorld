/*================================================================
Filename: ChunkSection.h
Date: 2018.1.15
Created by AirGuanZ
================================================================*/
#ifndef VW_CHUNK_SECTION_H
#define VW_CHUNK_SECTION_H

#include <vector>

#include "../Utility/D3D11Header.h"
#include "../Utility/Uncopiable.h"
#include "Block.h"

constexpr int CHUNK_SIZE = 16;

class ChunkSection : public Uncopiable
{
public:

private:
    Block blocks_[CHUNK_SIZE][CHUNK_SIZE][CHUNK_SIZE];

};

#endif //VW_CHUNK_SECTION_H
