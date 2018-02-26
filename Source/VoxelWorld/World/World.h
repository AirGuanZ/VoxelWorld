/*================================================================
Filename: World.h
Date: 2018.1.20
Created by AirGuanZ
================================================================*/
#pragma once

#include <string>

#include <Actor/Actor.h>
#include <Chunk/ChunkManager.h>
#include <Texture/Texture2D.h>

class World
{
public:
    World(int preloadDis, int renderDis, int unloadDis);
    ~World(void);

    bool Initialize(int loaderCount, std::string &errMsg);
    void Destroy(void);

    void Update(float deltaT);
    void Render(ChunkSectionRenderQueue *renderQueue);

    Actor &GetActor(void)
    {
        return actor_;
    }

private:
    Actor actor_;
    ChunkManager ckMgr_;
};
