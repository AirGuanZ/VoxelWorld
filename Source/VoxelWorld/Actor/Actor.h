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

    enum class ActorState
    {
        Standing,
        Running,
        Walking,
        Jumping,
    };

    struct ActorParam
    {
        float runningSpeed = 0.0f;
        float walkingSpeed = 0.0f;
        float flyingSpeed  = 0.0f;

        float turningSpeed = 0.0f;

        float camMovXSpeed = 0.0001f;
        float camMovYSpeed = 0.0001f;

        float camDistance  = 6.5f;

        float camDownReOffset = 0.02f;
        float camUpReOffset   = 0.02f;
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
                const UserInput &userInput,
                const EnvirInput &envirInput);

private:
    void UpdateCameraDirection(float deltaT, const UserInput &userInput);

    void UpdateState(const UserInput &userInput,
                     const EnvirInput &envirInput);
    void UpdateActorPosition(float deltaT, ChunkManager *ckMgr);
    void UpdateCameraPosition(float deltaT, ChunkManager *ckMgr);

    void InitState_Standing(void);
    void InitState_Running(void);
    void InitState_Walking(void);
    void InitState_Jumping(void);

    State UpdateState_Standing(const UserInput &userInput,
                               const EnvirInput &envirInput);
    State UpdateState_Running(const UserInput &userInput,
                              const EnvirInput &envirInput);
    State UpdateState_Walking(const UserInput &userInput,
                              const EnvirInput &envirInput);
    State UpdateState_Jumping(const UserInput &userInput,
                              const EnvirInput &envirInput);

private:
    State state_;

    //脚下落地否
    bool onGround_;

    //角色位置和朝向
    Vector3 pos_;
    float actYaw_, dstYaw_;

    //速度和加速度
    Vector3 vel_, acl_;

    Params params_;

    Camera camera_;
    ActorModel model_;
};

#endif //VW_ACTOR_H
