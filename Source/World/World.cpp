/*================================================================
Filename: World.cpp
Date: 2018.1.20
Created by AirGuanZ
================================================================*/
#include "../Resource/ResourceName.h"
#include "World.h"

World::World(void)
    : ckMgr_(10, 8, 5, 5, 7)
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

void World::Update(float deltaT)
{
    actor_.UpdateCamera(deltaT);

    ckMgr_.SetCentrePosition(
        BlockXZ_To_ChunkXZ(Camera_To_Block(actor_.GetCamera().GetPosition().x)),
        BlockXZ_To_ChunkXZ(Camera_To_Block(actor_.GetCamera().GetPosition().z)));
    
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
