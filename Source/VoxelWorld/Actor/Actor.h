/*================================================================
Filename: Actor.h
Date: 2018.2.17
Created by AirGuanZ
================================================================*/
#ifndef VW_ACTOR_H
#define VW_ACTOR_H

#include "../Chunk/ChunkManager.h"
#include "ActorModel.h"
#include "Camera.h"

class Actor
{
public:
    bool Initialize(std::string &errMsg);

    bool Update(float deltaT, ChunkManager *ckMgr);

    void Render(void);

    const Matrix &GetViewProjMatrix(void) const;

    const Camera &GetCamera(void) const;

    const Vector3 &GetCameraPosition(void) const;

private:

};

#endif //VW_ACTOR_H
