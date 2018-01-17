/*================================================================
Filename: BlockModelGenerator.cpp
Date: 2018.1.17
Created by AirGuanZ
================================================================*/
#include <cassert>

#include "BlockInfoManager.h"
#include "BlockModelGenerator.h"

const BlockModelGenerator *GetBlockModelGenerator(const Block &block)
{
    static const BlockModelGenerator_BasicRenderer_Box BMG_BasicRenderer_Box;
    static const BlockModelGenerator_Null              BMG_Null;

    const BlockInfo &info = BlockInfoManager::GetInstance().GetBlockInfo(block.type);
    switch(info.shape)
    {
    case BlockShape::Null:
        return &BMG_Null;
    case BlockShape::Box:
        switch(info.renderer)
        {
        case BlockRenderer::Null:
            return &BMG_Null;
        case BlockRenderer::BasicRenderer:
            return &BMG_BasicRenderer_Box;
        default:
            abort();
        }
    default:
        abort();
    }
    return nullptr;
}

void BlockModelGenerator_Null::AddTriangles(
    const Block &blk,
    const Block &pX, const Block &nX,
    const Block &pY, const Block &nY,
    const Block &pZ, const Block &nZ,
    ChunkSectionModels *models) const
{

}

void BlockModelGenerator_BasicRenderer_Box::AddTriangles(
    const Block &blk,
    const Block &pX, const Block &nX,
    const Block &pY, const Block &nY,
    const Block &pZ, const Block &nZ,
    ChunkSectionModels *models) const
{
    assert(models != nullptr);
    BlockInfoManager &infoMgr = BlockInfoManager::GetInstance();
    const BlockInfo &info = infoMgr.GetBlockInfo(blk.type);
    assert(info.shape == BlockShape::Box && info.renderer == BlockRenderer::BasicRenderer);

    //x+
    if(infoMgr.IsFaceVisible(blk.type, pX.type))
    {

    }
}
