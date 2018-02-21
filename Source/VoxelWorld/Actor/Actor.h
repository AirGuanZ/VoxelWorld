/*================================================================
Filename: Actor.h
Date: 2018.2.17
Created by AirGuanZ
================================================================*/
#ifndef VW_ACTOR_H
#define VW_ACTOR_H

#include <string>

#include "../Chunk/ChunkManager.h"
#include "ActorModel.h"
#include "Camera.h"

constexpr int ACTOR_INPUT_KEY_INDEX_FRONT = 0;
constexpr int ACTOR_INPUT_KEY_INDEX_BACK  = 1 + ACTOR_INPUT_KEY_INDEX_FRONT;
constexpr int ACTOR_INPUT_KEY_INDEX_LEFT  = 1 + ACTOR_INPUT_KEY_INDEX_BACK;
constexpr int ACTOR_INPUT_KEY_INDEX_RIGHT = 1 + ACTOR_INPUT_KEY_INDEX_LEFT;
constexpr int ACTOR_INPUT_KEY_INDEX_JUMP  = 1 + ACTOR_INPUT_KEY_INDEX_RIGHT;
constexpr int ACTOR_INPUT_KEY_INDEX_UP    = 1 + ACTOR_INPUT_KEY_INDEX_JUMP;
constexpr int ACTOR_INPUT_KEY_INDEX_DOWN  = 1 + ACTOR_INPUT_KEY_INDEX_UP;
constexpr int ACTOR_INPUT_KEY_INDEX_WALK  = 1 + ACTOR_INPUT_KEY_INDEX_DOWN;

constexpr int ACTOR_INPUT_KEY_NUM         = 1 + ACTOR_INPUT_KEY_INDEX_WALK;

namespace ActorAux
{
    struct UserInput
    {
        void Clear(void);

        bool keys[ACTOR_INPUT_KEY_NUM];
        Vector3 msMov;
    };

    void DefaultUserInput(UserInput &userInput);

    struct EnvirInput
    {
        void Clear(void);
    };

    //这里改了的话Actor::UpdateState里的跳转表也要改
    enum class ActorState
    {
        Standing = 0,
        Running,
        Walking,
        Jumping,
    };

    struct ActorParam
    {
        float runningSpeed = 0.0085f;
        float walkingSpeed = 0.003f;
        float flyingSpeed  = 0.0f;

        float turningSpeed = 0.166667f;

        float camMovXSpeed = 0.0001f;
        float camMovYSpeed = 0.0001f;

        float camDistance  = 6.5f;
        float camDstYOffset = 1.5f;

        float camDownReOffset = 0.02f;
        float camUpReOffset   = 0.02f;

        float collisionRadius = 0.2f;
        float collisionHeight = 1.6f;
        float modelYOffset    = 0.4f;

        float gravityAcl      = 0.002f;

        float jumpVel         = 0.1f;

        float standingFricAcl = 0.0005f;
        
    };
}

class Actor
{
public:
    using UserInput  = ActorAux::UserInput;
    using EnvirInput = ActorAux::EnvirInput;
    using Params     = ActorAux::ActorParam;
    using State      = ActorAux::ActorState;

    bool Initialize(std::string &errMsg);

    void Render(void);

    const Matrix &GetViewProjMatrix(void) const;

    const Camera &GetCamera(void) const;

    const Vector3 &GetCameraPosition(void) const;

    const Params &GetParams(void) const;
    void SetParams(const Params &params);

    void Update(float deltaT, ChunkManager *ckMgr,
                const UserInput &uI, const EnvirInput &eI);

private:
    //摄像机视角旋转
    void UpdateCameraDirection(const UserInput &uI);
    //状态更新与应用
    void UpdateState(const UserInput &uI, const EnvirInput &eI);
    //根据速度和碰撞更新角色位置
    void UpdateActorPosition(float deltaT, ChunkManager *ckMgr);
    //根据角色位置、摄像机视角等更新摄像机位置
    void UpdateCameraPosition(float deltaT, ChunkManager *ckMgr);

    //状态初始化、转换等

    void InitState_Standing(const UserInput &uI, const EnvirInput &eI);
    void InitState_Running (const UserInput &uI, const EnvirInput &eI);
    void InitState_Walking (const UserInput &uI, const EnvirInput &eI);
    void InitState_Jumping (const UserInput &uI, const EnvirInput &eI);

    State UpdateState_Standing(const UserInput &uI, const EnvirInput &eI);
    State UpdateState_Running (const UserInput &uI, const EnvirInput &eI);
    State UpdateState_Walking (const UserInput &uI, const EnvirInput &eI);
    State UpdateState_Jumping (const UserInput &uI, const EnvirInput &eI);

    void ApplyState_Standing(const UserInput &uI, const EnvirInput &eI);
    void ApplyState_Running (const UserInput &uI, const EnvirInput &eI);
    void ApplyState_Walking (const UserInput &uI, const EnvirInput &eI);
    void ApplyState_Jumping (const UserInput &uI, const EnvirInput &eI);

private:
    State state_;

    //落地否
    bool onGround_;

    //位置和朝向
    Vector3 pos_;
    float actYaw_, dstYaw_;

    //速度和加速度
    Vector3 vel_, acl_;

    Params params_;

    Camera camera_;
    ActorModel model_;
};

#endif //VW_ACTOR_H
