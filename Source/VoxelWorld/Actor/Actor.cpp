/*================================================================
Filename: Actor.cpp
Date: 2018.2.17
Created by AirGuanZ
================================================================*/
#include <array>
#include <cstring>
#include <exception>
#include <Windows.h>

#include <Input\InputManager.h>
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
    userInput.keys[ACTOR_INPUT_KEY_INDEX_JUMP]  = input.IsKeyDown(VK_SPACE);
    userInput.keys[ACTOR_INPUT_KEY_INDEX_DOWN]  = input.IsKeyDown(VK_LSHIFT);
    userInput.keys[ACTOR_INPUT_KEY_INDEX_UP]    = input.IsKeyDown(VK_SPACE);
    userInput.keys[ACTOR_INPUT_KEY_INDEX_WALK]  = input.IsKeyDown(VK_MENU);

    userInput.msMov.x = static_cast<float>(input.GetCursorMovX());
    userInput.msMov.y = static_cast<float>(input.GetCursorMovY());
    userInput.msMov.z = static_cast<float>(input.GetMouseWheel());
}

bool Actor::Initialize(std::string &errMsg)
{
    state_ = State::Standing;

    onGround_ = false;

    pos_ = Vector3(0.0f, 100.0f, 0.0f);
    actYaw_ = dstYaw_ = 0.0f;

    vel_ = Vector3(0.0f, 0.0f, 0.0f);

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

const Camera &Actor::GetCamera(void) const
{
    return camera_;
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
    UpdateState(dT, uI, eI);
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

void Actor::UpdateState(float dT, const UserInput &uI, const EnvirInput &eI)
{
    using ActorInitStateFunc   = void (Actor::*)(const Actor::UserInput&, const Actor::EnvirInput&);
    using ActorUpdateStateFunc = Actor::State (Actor::*)(const Actor::UserInput&, const Actor::EnvirInput&);
    using ActorApplyStateFunc  = void (Actor::*)(float, const Actor::UserInput&, const Actor::EnvirInput&);

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
        (this->*actorInitStateFuncs[static_cast<int>(newState)])(uI, eI);

    //根据状态更新actor其他属性
    (this->*actorApplyStateFuncs[static_cast<int>(state_)])(dT, uI, eI);
}

//IMPROVE：碰撞检测恢复时搜索合适位置目前是暴力的
void Actor::UpdateActorPosition(float dT, ChunkManager *ckMgr)
{
    Vector3 oldPos = pos_, newPos = pos_ + dT * vel_;

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
        {
            return false;
        }
        pos_ = p;
        return true;
    };

    onGround_ = false;
    size_t pIdx = 0;
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
        if(newPos.y <= oldPos.y)
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
        assert(TryMoveTo({ pos_.x, finalY, pos_.z }) == true);
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
            actYaw_ = (std::min)(actYaw_ + dT * params_.turningSpeed, actYaw_ + deltaYaw);
        else
            actYaw_ = (std::max)(actYaw_ - dT * params_.turningSpeed, actYaw_ + deltaYaw);
    }

    model_.SetTransform(
        Matrix::CreateFromAxisAngle({ 0.0f, 1.0f, 0.0f }, -actYaw_) *
        Matrix::CreateTranslation(Vector3(pos_.x, pos_.y + params_.modelYOffset, pos_.z)));
}

void Actor::UpdateCameraPosition(float deltaT, ChunkManager *ckMgr)
{
    camera_.SetPosition(Vector3(pos_.x, pos_.y + params_.camDstYOffset, pos_.z) -
                        params_.camDistance * camera_.GetDirection());
    camera_.UpdateViewProjMatrix();
}

namespace
{
    const std::string ACTOR_ANINAME_STANDING = "Moving";
    const std::string ACTOR_ANINAME_RUNNING  = "Moving";
    const std::string ACTOR_ANINAME_WALKING  = "Moving";
    const std::string ACTOR_ANINAME_JUMPING  = "Moving";
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
        vel_.y = params_.jumpInitVel;

    model_.SetAnimationClip(ACTOR_ANINAME_JUMPING, true);
}

namespace
{
    inline std::pair<int, int> Get_FB_LR_Move(const Actor::UserInput &uI)
    {
        return std::make_pair(
            uI.keys[ACTOR_INPUT_KEY_INDEX_FRONT] - uI.keys[ACTOR_INPUT_KEY_INDEX_BACK],
            uI.keys[ACTOR_INPUT_KEY_INDEX_LEFT]  - uI.keys[ACTOR_INPUT_KEY_INDEX_RIGHT]);
    }

    inline bool HasMoving(const Actor::UserInput &uI)
    {
        return (uI.keys[ACTOR_INPUT_KEY_INDEX_FRONT] ^ uI.keys[ACTOR_INPUT_KEY_INDEX_BACK]) ||
               (uI.keys[ACTOR_INPUT_KEY_INDEX_LEFT]  ^ uI.keys[ACTOR_INPUT_KEY_INDEX_RIGHT]);
    }
}

Actor::State Actor::UpdateState_Standing(const UserInput &uI, const EnvirInput &eI)
{
    if(!onGround_)
        return State::Jumping;

    if(uI.keys[ACTOR_INPUT_KEY_INDEX_JUMP])
        return State::Jumping;

    if(HasMoving(uI))
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

    if(HasMoving(uI))
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

    if(HasMoving(uI))
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

    if(HasMoving(uI))
    {
        if(uI.keys[ACTOR_INPUT_KEY_INDEX_WALK])
            return State::Walking;
        return State::Running;
    }

    return State::Standing;
}

namespace
{
    const std::array<float, 9> movingYawOffsets =
    {
        1.5f * PIDIV2, PI,   -1.5f * PIDIV2,
        PIDIV2,        0.0f, -PIDIV2,
        0.5f * PIDIV2, 0.0,  -0.5f * PIDIV2
    };
    inline float GetMovingYawOffsets(int FB, int LR)
    {
        assert(-1 <= FB && FB <= 1 && -1 <= LR && LR <= 1);
        return movingYawOffsets[3 * (FB + 1) + (LR + 1)];
    }

    //将origin长度减少fric
    inline Vector3 ApplyFric(const Vector3 &origin, float fric)
    {
        Vector3 rt;
        origin.Normalize(rt);
        return (std::max)(0.0f, origin.Length() - fric) * rt;
    }

    //将origin在xz平面上分量的长度减少fric
    inline Vector3 ApplyFricXZ(const Vector3 &ori, float fric)
    {
        Vector3 rtHor = ApplyFric(Vector3(ori.x, 0.0f, ori.z), fric);
        return Vector3(rtHor.x, ori.y, rtHor.z);
    }
}

void Actor::ApplyState_Standing(float dT, const UserInput &uI, const EnvirInput &eI)
{
    //重力加速度
    vel_ = CombineAcc(vel_, params_.gravityDir, dT * params_.gravityAcl, params_.gravityMaxSpeed);

    //站立时的水平阻力减速
    vel_ = ApplyFricXZ(vel_, dT * params_.standingFricAcl);
}

void Actor::ApplyState_Running(float dT, const UserInput &uI, const EnvirInput &eI)
{
    //重力加速度
    vel_ = CombineAcc(vel_, params_.gravityDir, dT * params_.gravityAcl, params_.gravityMaxSpeed);

    assert(HasMoving(uI));
    auto [FB, LR] = Get_FB_LR_Move(uI);

    //新的dstYaw
    dstYaw_ = -camera_.GetYaw() + GetMovingYawOffsets(FB, LR);

    //移动方向
    
    const Vector3 &camDir = camera_.GetDirection();
    Vector3 horMove = static_cast<float>(FB) * Vector3(camDir.x, 0.0f, camDir.z) +
                      static_cast<float>(LR) * Vector3(camDir.z, 0.0f, -camDir.x);
    horMove.Normalize();

    //将移动加速度和阻力叠加到速度上
    //移动加速作用到horMove方向上

    //阻力叠加
    Vector3 fricDir = Vector3(vel_.x, 0.0f, vel_.z);
    if(fricDir.Length() > 1e-5f)
    {
        fricDir.Normalize();
        vel_ = CombineFric(vel_, fricDir, dT * params_.runningFricAcl, 0.0f);
    }

    //移动速度叠加
    vel_ = CombineAcc(vel_, horMove, dT * params_.runningAcl, params_.runningSpeed);
}

void Actor::ApplyState_Walking(float dT, const UserInput &uI, const EnvirInput &eI)
{
    //重力加速度
    vel_ = CombineAcc(vel_, params_.gravityDir, dT * params_.gravityAcl, params_.gravityMaxSpeed);

    assert(HasMoving(uI));
    auto [FB, LR] = Get_FB_LR_Move(uI);

    //新的dstYaw
    dstYaw_ = -camera_.GetYaw() + GetMovingYawOffsets(FB, LR);

    //移动速度增量

    const Vector3 &camDir = camera_.GetDirection();
    Vector3 horMove = static_cast<float>(FB) * Vector3(camDir.x, 0.0f, camDir.z) +
                      static_cast<float>(LR) * Vector3(camDir.z, 0.0f, -camDir.x);

    //将移动加速度和阻力叠加到速度上
    //移动加速作用到horMove方向上

    //阻力叠加
    Vector3 fricDir = Vector3(vel_.x, 0.0f, vel_.z);
    if(fricDir.Length() > 1e-5f)
    {
        fricDir.Normalize();
        vel_ = CombineFric(vel_, fricDir, dT * params_.walkingFricAcl, 0.0f);
    }

    //移动速度叠加
    horMove.Normalize();
    vel_ = CombineAcc(vel_, horMove, dT * params_.walkingAcl, params_.walkingSpeed);
}

void Actor::ApplyState_Jumping(float dT, const UserInput &uI, const EnvirInput &eI)
{
    //重力加速度
    vel_ = CombineAcc(vel_, params_.gravityDir, dT * params_.gravityAcl, params_.gravityMaxSpeed);

    auto [FB, LR] = Get_FB_LR_Move(uI);

    //新的dstYaw
    if(FB || LR)
        dstYaw_ = -camera_.GetYaw() + GetMovingYawOffsets(FB, LR);

    //移动速度增量

    const Vector3 &camDir = camera_.GetDirection();
    Vector3 horMove = static_cast<float>(FB) * Vector3(camDir.x, 0.0f, camDir.z) +
                      static_cast<float>(LR) * Vector3(camDir.z, 0.0f, -camDir.x);

    //将移动加速度和阻力叠加到速度上
    //移动加速作用到horMove方向上

    //阻力叠加
    Vector3 fricDir = Vector3(vel_.x, 0.0f, vel_.z);
    if(fricDir.Length() > 1e-5f)
    {
        fricDir.Normalize();
        vel_ = CombineFric(vel_, fricDir, dT * params_.jumpingFricAcl, 0.0f);
    }

    //移动速度叠加
    horMove.Normalize();
    vel_ = CombineAcc(vel_, horMove, dT * params_.jumpingAcl, params_.jumpingSpeed);
}
