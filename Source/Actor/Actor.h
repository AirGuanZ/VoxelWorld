/*================================================================
Filename: Actor.h
Date: 2018.1.19
Created by AirGuanZ
================================================================*/
#ifndef VW_ACTOR_H
#define VW_ACTOR_H

#include "../Camera/Camera.h"
#include "../Chunk/ChunkManager.h"
#include "../Utility/Math.h"
#include "../Window/Window.h"

class Actor
{
public:
    void UpdateCamera(float deltaT, ChunkManager *ckMgr);

    const Matrix &GetViewProjMatrix(void) const
    {
        return camera_.GetViewProjMatrix();
    }

    const Camera &GetCamera(void) const
    {
        return camera_;
    }

    const Vector3 &GetCameraPosition(void) const
    {
        return camera_.GetPosition();
    }

private:
    float flyUpSpeed_ = 0.009f;
    float flyDownSpeed_ = 0.009f;
    float horMoveSpeed_ = 0.0085f;

    float mouseXSpeed_ = 0.00014f;
    float mouseYSpeed_ = 0.00010f;

    Camera camera_;
};

#endif //VW_ACTOR_H
