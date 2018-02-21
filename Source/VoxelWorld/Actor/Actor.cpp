/*================================================================
Filename: Actor.cpp
Date: 2018.2.17
Created by AirGuanZ
================================================================*/
#include <array>
#include <cstring>
#include <exception>
#include <Windows.h>

#include "../Input/InputManager.h"
#include "Actor.h"

namespace
{
    template<typename T>
    T clamp(T v, T L, T R)
    {
        return (std::min)((std::max)(L, v), R);
    }
}

void ActorAux::UserInput::Clear(void)
{
    std::memset(keys, 0, sizeof(keys));
    msMov = Vector3(0.0f, 0.0f, 0.0f);
}

void ActorAux::EnvirInput::Clear(void)
{

}

void ActorAux::DefaultUserInput(UserInput &userInput)
{
    InputManager &input = InputManager::GetInstance();

    userInput.keys[ACTOR_INPUT_KEY_INDEX_FRONT] = input.IsKeyDown('W');
    userInput.keys[ACTOR_INPUT_KEY_INDEX_BACK]  = input.IsKeyDown('S');
    userInput.keys[ACTOR_INPUT_KEY_INDEX_LEFT]  = input.IsKeyDown('A');
    userInput.keys[ACTOR_INPUT_KEY_INDEX_RIGHT] = input.IsKeyDown('D');
    userInput.keys[ACTOR_INPUT_KEY_INDEX_JUMP]  = input.IsKeyPressed(VK_SPACE);
    userInput.keys[ACTOR_INPUT_KEY_INDEX_DOWN]  = input.IsKeyDown(VK_LSHIFT);
    userInput.keys[ACTOR_INPUT_KEY_INDEX_UP]    = input.IsKeyDown(VK_SPACE);
    userInput.keys[ACTOR_INPUT_KEY_INDEX_WALK]  = input.IsKeyDown(VK_MENU);

    userInput.msMov.x = input.GetCursorMovX();
    userInput.msMov.y = input.GetCursorMovY();
    userInput.msMov.z = static_cast<float>(input.GetMouseWheel());
}

bool Actor::Initialize(std::string &errMsg)
{
    state_ = State::Standing;

    pos_ = Vector3(0.0f, 100.0f, 0.0f);
    actYaw_ = dstYaw_ = 0.0f;

    params_ = Params();

    if(!model_.Initialize(errMsg))
        return false;

    return true;
}

void Actor::Render(void)
{
    model_.Render(camera_);
}

const Matrix &Actor::GetViewProjMatrix(void) const
{
    return camera_.GetViewProjMatrix();
}

const Vector3 &Actor::GetCameraPosition(void) const
{
    return camera_.GetPosition();
}

const Actor::Params &Actor::GetParams(void) const
{
    return params_;
}

void Actor::SetParams(const Params &params)
{
    params_ = params;
}

void Actor::Update(float dT, ChunkManager *ckMgr,
                   const UserInput &uI, const EnvirInput &eI)
{
    UpdateCameraDirection(uI);
    UpdateState(uI, eI);
    UpdateActorPosition(dT, ckMgr);
    UpdateCameraPosition(dT, ckMgr);

    model_.Update(dT);
    camera_.UpdateViewProjMatrix();
}

void Actor::UpdateCameraDirection(const UserInput &uI)
{
    float newYaw = camera_.GetYaw() - params_.camMovXSpeed * uI.msMov.x;
    float newPitch = camera_.GetPitch() - params_.camMovYSpeed * uI.msMov.y;
    newPitch = clamp(newPitch, -PIDIV2 + params_.camDownReOffset, PIDIV2 - params_.camUpReOffset);

    while(newYaw > _2PI)
        newYaw -= _2PI;
    while(newYaw < 0)
        newYaw += _2PI;

    camera_.SetYaw(newYaw);
    camera_.SetPitch(newPitch);
}

void Actor::UpdateState(const UserInput &uI, const EnvirInput &eI)
{
    using ActorInitStateFunc = void (Actor::*)(const Actor::UserInput&, const Actor::EnvirInput&);
    using ActorUpdateStateFunc = Actor::State (Actor::*)(const Actor::UserInput&, const Actor::EnvirInput&);
    using ActorApplyStateFunc = void (Actor::*)(const Actor::UserInput&, const Actor::EnvirInput&);

    ActorInitStateFunc actorInitStateFuncs[] =
    {
        &Actor::InitState_Standing,
        &Actor::InitState_Running,
        &Actor::InitState_Walking,
        &Actor::InitState_Jumping
    };

    ActorUpdateStateFunc actorUpdateStateFuncs[] =
    {
        &Actor::UpdateState_Standing,
        &Actor::UpdateState_Running,
        &Actor::UpdateState_Walking,
        &Actor::UpdateState_Jumping
    };

    ActorApplyStateFunc actorApplyStateFuncs[] =
    {
        &Actor::ApplyState_Standing,
        &Actor::ApplyState_Running,
        &Actor::ApplyState_Walking,
        &Actor::ApplyState_Jumping
    };

    //新状态
    State newState = (this->*actorUpdateStateFuncs[static_cast<int>(state_)])(uI, eI);

    //处理状态改变
    if(newState != state_)
        (this->*actorInitStateFuncs[static_cast<int>(state_)])(uI, eI);

    //根据状态更新actor其他属性
    (this->*actorApplyStateFuncs[static_cast<int>(state_)])(uI, eI);
}

//IMPROVE：碰撞检测恢复时搜索合适位置目前是暴力的
void Actor::UpdateActorPosition(float dT, ChunkManager *ckMgr)
{
    //添加重力加速度
    acl_.y -= params_.gravityAcl;

    Vector3 vel = vel_ + dT * acl_;
    Vector3 oldPos = pos_, newPos = pos_ + dT * vel;

    std::vector<Vector3> tryList =
    {
        newPos,
        { oldPos.x, newPos.y, newPos.z },
        { newPos.x, newPos.y, oldPos.z },
        { newPos.x, oldPos.y, newPos.z },
        { oldPos.x, newPos.y, oldPos.z },
        { oldPos.x, oldPos.y, newPos.z },
        { newPos.x, oldPos.y, oldPos.z },
        oldPos
    };

    auto TryMoveTo = [&](const Vector3 &p) -> bool
    {
        if(ckMgr->DetectCollision(AABB{ p - Vector3(params_.collisionRadius,
                                                    0.0f,
                                                    params_.collisionRadius),
                                        p + Vector3(params_.collisionRadius,
                                                    params_.collisionHeight,
                                                    params_.collisionRadius) }))
            return false;

        pos_ = p;
        return true;
    };

    onGround_ = false;
    int pIdx = 0;
    while(pIdx < tryList.size())
    {
        if(TryMoveTo(tryList[pIdx]))
            break;
        ++pIdx;
    }

    if(pos_.y == oldPos.y) //直接赋值得到，可以精确比较
    {
        //置竖直速度为0
        //根据恢复方向设置onGround
        vel_.y = 0.0f;
        if(newPos.y < oldPos.y)
            onGround_ = true;

        //在oldPos和newPos之间搜索一个合适的位置
        constexpr int COLDEC_Y_SEGNUM = 20;
        float step = (newPos.y - oldPos.y) / COLDEC_Y_SEGNUM;
        float finalY = oldPos.y;
        for(int i = 0; i < COLDEC_Y_SEGNUM; ++i)
        {
            float newY = finalY + step;
            if(!TryMoveTo({ pos_.x, newY, pos_.z }))
                break;

            finalY = newY;
        }
#ifdef _DEBUG
        assert(TryMoveTo({ pos_.x, finalY, pos_.z }));
#else
        pos_.y = finalY;
#endif
    }

    //x和z方向的处理就没辣么精致啦(╯‵□′)╯︵┻━┻

    if(pos_.x == oldPos.x)
        vel_.x = 0.0f;
    if(pos_.z == oldPos.z)
        vel_.z = 0.0f;

    //根据dstYaw和aclYaw的差距计算新的aclYaw

    float deltaYaw = dstYaw_ - actYaw_;
    while(deltaYaw > +PI) deltaYaw -= _2PI;
    while(deltaYaw < -PI) deltaYaw += _2PI;

    if(std::abs(deltaYaw) > 0.97f * PI)
        actYaw_ = dstYaw_;
    else
    {
        if(deltaYaw > 0.0f)
            actYaw_ = (std::min)(actYaw_ + params_.turningSpeed, actYaw_ + deltaYaw);
        else
            actYaw_ = (std::max)(actYaw_ - params_.turningSpeed, actYaw_ + deltaYaw);
    }

    model_.SetTransform(
        Matrix::CreateFromAxisAngle({ 0.0f, 1.0f, 0.0f }, -actYaw_) *
        Matrix::CreateTranslation(pos_));
}

void Actor::UpdateCameraPosition(float deltaT, ChunkManager *ckMgr)
{
    camera_.SetPosition(pos_ + Vector3(0.0f, params_.camDstYOffset, 0.0f) -
                        params_.camDistance * camera_.GetDirection());
    camera_.UpdateViewProjMatrix();
}

namespace
{
    const std::string ACTOR_ANINAME_STANDING = "Standing";
    const std::string ACTOR_ANINAME_RUNNING  = "Running";
    const std::string ACTOR_ANINAME_WALKING  = "Walking";
    const std::string ACTOR_ANINAME_JUMPING  = "Jumping";
}

void Actor::InitState_Standing(const UserInput &uI, const EnvirInput &eI)
{
    state_ = State::Standing;
    vel_.y = 0.0f;

    model_.SetAnimationClip(ACTOR_ANINAME_STANDING, true);
}

void Actor::InitState_Running(const UserInput &uI, const EnvirInput &eI)
{
    state_ = State::Running;
    vel_.y = 0.0f;

    model_.SetAnimationClip(ACTOR_ANINAME_RUNNING, true);
}

void Actor::InitState_Walking(const UserInput &uI, const EnvirInput &eI)
{
    state_ = State::Walking;
    vel_.y = 0.0f;

    model_.SetAnimationClip(ACTOR_ANINAME_WALKING, true);
}

void Actor::InitState_Jumping(const UserInput &uI, const EnvirInput &eI)
{
    state_ = State::Jumping;

    if(onGround_ && uI.keys[ACTOR_INPUT_KEY_INDEX_JUMP])
        vel_.y = params_.jumpVel;

    model_.SetAnimationClip(ACTOR_ANINAME_JUMPING, true);
}

namespace
{
    inline std::pair<int, int> Get_FB_LR_Move(const Actor::UserInput &uI)
    {
        return std::make_pair(
            uI.keys[ACTOR_INPUT_KEY_INDEX_FRONT] - uI.keys[ACTOR_INPUT_KEY_INDEX_BACK],
            uI.keys[ACTOR_INPUT_KEY_INDEX_LEFT] - uI.keys[ACTOR_INPUT_KEY_INDEX_RIGHT]);
    }
}

Actor::State Actor::UpdateState_Standing(const UserInput &uI, const EnvirInput &eI)
{
    if(!onGround_)
        return State::Jumping;

    if(uI.keys[ACTOR_INPUT_KEY_INDEX_JUMP])
        return State::Jumping;

    if(Get_FB_LR_Move(uI) != std::make_pair<int, int>(0, 0))
    {
        if(uI.keys[ACTOR_INPUT_KEY_INDEX_WALK])
            return State::Walking;
        return State::Running;
    }

    return State::Standing;
}

Actor::State Actor::UpdateState_Running(const UserInput &uI, const EnvirInput &eI)
{
    if(!onGround_)
        return State::Jumping;

    if(uI.keys[ACTOR_INPUT_KEY_INDEX_JUMP])
        return State::Jumping;

    if(Get_FB_LR_Move(uI) != std::make_pair<int, int>(0, 0))
    {
        if(uI.keys[ACTOR_INPUT_KEY_INDEX_WALK])
            return State::Walking;
        return State::Running;
    }

    return State::Standing;
}

Actor::State Actor::UpdateState_Walking(const UserInput &uI, const EnvirInput &eI)
{
    if(!onGround_)
        return State::Jumping;

    if(uI.keys[ACTOR_INPUT_KEY_INDEX_JUMP])
        return State::Jumping;

    if(Get_FB_LR_Move(uI) != std::make_pair<int, int>(0, 0))
    {
        if(uI.keys[ACTOR_INPUT_KEY_INDEX_WALK])
            return State::Walking;
        return State::Running;
    }

    return State::Standing;
}

Actor::State Actor::UpdateState_Jumping(const UserInput &uI, const EnvirInput &eI)
{
    if(!onGround_)
        return State::Jumping;

    if(uI.keys[ACTOR_INPUT_KEY_INDEX_JUMP])
        return State::Jumping;

    if(Get_FB_LR_Move(uI) != std::make_pair<int, int>(0, 0))
    {
        if(uI.keys[ACTOR_INPUT_KEY_INDEX_WALK])
            return State::Walking;
        return State::Running;
    }

    return State::Standing;
}


