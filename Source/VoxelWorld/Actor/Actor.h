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
    struct ActorParam
    {
        //跑动的自给加速度
        float runningAcl = 0.0008f;
        //行走的自给加速度
        float walkingAcl = 0.0007f;
        //跳跃的自给加速度
        float jumpingAcl = 0.0007f;

        //跑动自给速度上限
        float runningSpeed = 0.0085f;
        //行走自给速度上限
        float walkingSpeed = 0.0045f;
        //跳跃自己速度上限
        float jumpingSpeed = 0.0075f;
        //飞行自给速度上限
        float flyingSpeed = 0.0f;

        //站立阻力加速度
        float standingFricAcl = 0.001f;
        //跑动阻力加速度
        float runningFricAcl = 0.001f;
        //行走阻力加速度
        float walkingFricAcl = 0.001f;
        //跳跃阻力加速度
        float jumpingFricAcl = 0.001f;

        //角色朝向转动速度
        float turningSpeed = 0.0166667f;

        //摄像机旋转水平灵敏度
        float camMovXSpeed = 0.00075f;
        //摄像机旋转垂直灵敏度
        float camMovYSpeed = 0.00075f;

        //摄像机到角色视点距离
        float camDistance = 7.5f;
        //角色视点距角色位置的高度偏移
        float camDstYOffset = 1.5f;

        //摄像机看向脚下时的最小垂直夹角
        float camDownReOffset = 0.02f;
        //摄像机看向天空时的最小垂直夹角
        float camUpReOffset = 0.02f;

        //角色碰撞半径
        float collisionRadius = 0.2f;
        //角色碰撞高度
        float collisionHeight = 1.6f;
        //模型位置和角色位置的垂直偏移
        float modelYOffset = 0.4f;

        //跳跃初速度
        float jumpInitVel = 0.035f;

        //重力加速度大小
        float gravityAcl = 0.0002f;
        //重力加速度方向
        Vector3 gravityDir = Vector3(0.0f, -1.0f, 0.0f);
        //重力带来的最大速度
        float gravityMaxSpeed = 0.01f;
    };

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
    //IMPROVE：把状态机抽象出来
    enum class ActorState
    {
        Standing = 0,
        Running,
        Walking,
        Jumping,
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
    void UpdateState(float dT, const UserInput &uI, const EnvirInput &eI);
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

    void ApplyState_Standing(float dT, const UserInput &uI, const EnvirInput &eI);
    void ApplyState_Running (float dT, const UserInput &uI, const EnvirInput &eI);
    void ApplyState_Walking (float dT, const UserInput &uI, const EnvirInput &eI);
    void ApplyState_Jumping (float dT, const UserInput &uI, const EnvirInput &eI);

private:
    State state_;

    //落地否
    bool onGround_;

    //位置和朝向
    Vector3 pos_;
    float actYaw_, dstYaw_;

    //速度
    Vector3 vel_;

    Params params_;

    Camera camera_;
    ActorModel model_;
};

#endif //VW_ACTOR_H
