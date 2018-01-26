/*================================================================
Filename: World.cpp
Date: 2018.1.20
Created by AirGuanZ
================================================================*/
#include "../Block/BlockInfoManager.h"
#include "../Input/InputManager.h"
#include "../Resource/ResourceName.h"
#include "World.h"

World::World(int preloadDis, int renderDis)
    : ckMgr_(preloadDis, renderDis, 8, 8, 13, 200, 200, 300, renderDis)
{

}

World::~World(void)
{
    Destroy();
}

void World::Initialize(void)
{
    ckMgr_.StartLoading();
}

void World::Destroy(void)
{
    ckMgr_.Destroy();
}

namespace
{
    bool IsNotAir(const Block &blk)
    {
        return blk.type != BlockType::Air;
    }
}

void World::Update(float deltaT)
{
    actor_.UpdateCamera(deltaT);

    ckMgr_.SetCentrePosition(
        BlockXZ_To_ChunkXZ(Camera_To_Block(actor_.GetCameraPosition().x)),
        BlockXZ_To_ChunkXZ(Camera_To_Block(actor_.GetCameraPosition().z)));

    //∑ΩøÈ∆∆ªµ
    if(InputManager::GetInstance().IsMouseButtonPressed(MouseButton::Left))
    {
        Block blk; BlockFace face; IntVector3 pickPos;
        if(ckMgr_.PickBlock(actor_.GetCameraPosition(), actor_.GetCamera().GetDirection(),
            10.0f, 0.01f, IsNotAir, blk, face, pickPos))
        {
            blk.type = BlockType::Air;
            ckMgr_.SetBlock(pickPos.x, pickPos.y, pickPos.z, blk);
        }
    }
    else if(InputManager::GetInstance().IsMouseButtonPressed(MouseButton::Right)) //∑ΩøÈ∑≈÷√
    {
        Block blk; BlockFace face; IntVector3 pickPos;
        if(ckMgr_.PickBlock(actor_.GetCameraPosition(), actor_.GetCamera().GetDirection(),
            10.0f, 0.01f, IsNotAir, blk, face, pickPos))
        {
            blk.type = BlockType::Leaf;
            IntVector3 faceDir[] =
            {
                { 1, 0, 0 }, { -1, 0, 0 },
                { 0, 1, 0 }, { 0, -1, 0 },
                { 0, 0, 1 }, { 0, 0, -1 }
            };
            IntVector3 &p = pickPos + faceDir[static_cast<int>(face)];
            if(BlockInfoManager::GetInstance().IsCoverable(ckMgr_.GetBlock(p.x, p.y, p.z).type))
                ckMgr_.SetBlock(p.x, p.y, p.z, blk);
        }
    }

    ckMgr_.ProcessLightUpdates();
    ckMgr_.ProcessChunkLoaderMessages();
    ckMgr_.ProcessModelUpdates();
}

void World::Render(ChunkSectionRenderQueue *renderQueue)
{
    assert(renderQueue != nullptr);

    ckMgr_.Render(renderQueue);
}

Actor &World::GetActor(void)
{
    return actor_;
}
