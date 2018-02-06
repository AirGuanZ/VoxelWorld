/*================================================================
Filename: World.h
Date: 2018.1.20
Created by AirGuanZ
================================================================*/
#ifndef VW_WORLD_H
#define VW_WORLD_H

#include "../Actor/Actor.h"
#include "../Chunk/ChunkManager.h"
#include "../Texture/Texture2D.h"

class World
{
public:
    World(int preloadDis, int renderDis, int unloadDis);
    ~World(void);

    void Initialize(int loaderCount);
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

#endif //VW_WORLD_H
