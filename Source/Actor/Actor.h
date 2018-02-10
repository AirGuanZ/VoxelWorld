/*================================================================
Filename: Actor.h
Date: 2018.1.19
Created by AirGuanZ
================================================================*/
#ifndef VW_ACTOR_H
#define VW_ACTOR_H

#include <string>

#include "../Camera/Camera.h"
#include "../Chunk/ChunkManager.h"
#include "../Utility/Math.h"
#include "../Window/Window.h"
#include "ActorModel.h"

/*
    Actor״̬�Ͷ�������֯��ƣ�
        �ڲ���״̬����֯��ģ�Ͷ������л��͸��·�װ����
        �ڲ�״̬��ת����UserInput��EnvirInput���ƶ�������ǰ���ε�Monomorphismһ����
*/

class Actor
{
public:
    bool Initialize(std::string &errMsg);

    void Update(float deltaT, ChunkManager *ckMgr);

    void Render(void);

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
    void UpdateCameraDirection(float deltaT, ChunkManager *ckMgr);
    void UpdateActorPosition  (float deltaT, ChunkManager *ckMgr);
    void UpdateCameraApperance(float deltaT, ChunkManager *ckMgr);

private:
    float flyUpSpeed_ = 0.009f;
    float flyDownSpeed_ = 0.009f;
    float horMoveSpeed_ = 0.0085f;

    float mouseXSpeed_ = 0.00014f;
    float mouseYSpeed_ = 0.00010f;

    Vector3 pos_ = Vector3(0.0f, 80.0f, 0.0f);
    float yaw_ = 0.0f;

    Camera camera_;

    ActorModel model_;
};

#endif //VW_ACTOR_H
