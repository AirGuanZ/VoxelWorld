/*================================================================
Filename: World.cpp
Date: 2018.1.20
Created by AirGuanZ
================================================================*/
#include "../Input/InputManager.h"
#include "../Resource/ResourceName.h"
#include "World.h"

World::World(void)
    : ckMgr_(11, 8, 8, 8, 13)
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

    //�����ƻ�
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
    else if(InputManager::GetInstance().IsMouseButtonPressed(MouseButton::Right)) //�������
    {
        Block blk; BlockFace face; IntVector3 pickPos;
        if(ckMgr_.PickBlock(actor_.GetCameraPosition(), actor_.GetCamera().GetDirection(),
            10.0f, 0.01f, IsNotAir, blk, face, pickPos))
        {
            blk.type = BlockType::GlowStone;
            IntVector3 faceDir[] =
            {
                { 1, 0, 0 }, { -1, 0, 0 },
                { 0, 1, 0 }, { 0, -1, 0 },
                { 0, 0, 1 }, { 0, 0, -1 }
            };
            IntVector3 &d = faceDir[static_cast<int>(face)];
            ckMgr_.SetBlock(pickPos.x + d.x, pickPos.y + d.y, pickPos.z + d.z, blk);
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
