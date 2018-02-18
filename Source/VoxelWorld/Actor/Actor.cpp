/*================================================================
Filename: Actor.cpp
Date: 2018.2.17
Created by AirGuanZ
================================================================*/
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
    UpdateCameraDirection(dT, uI);
    UpdateState(uI, eI);
    UpdateActorPosition(dT, ckMgr);
    UpdateCameraPosition(dT, ckMgr);

    model_.Update(dT);
    camera_.UpdateViewProjMatrix();
}

void Actor::UpdateCameraDirection(float dT, const UserInput &uI)
{
    float newYaw = camera_.GetYaw() - params_.camMovXSpeed * dT * uI.msMov.x;
    float newPitch = camera_.GetPitch() - params_.camMovYSpeed * dT * uI.msMov.y;
    newPitch = clamp(newPitch, -PIDIV2 + params_.camDownReOffset, PIDIV2 - params_.camUpReOffset);

    camera_.SetYaw(newYaw);
    camera_.SetPitch(newPitch);
}

void Actor::UpdateState(const UserInput &uI, const EnvirInput &eI)
{
    State newState = State::Standing;
    switch(state_)
    {
    case State::Standing:
        newState = UpdateState_Standing(uI, eI);
        break;
    case State::Running:
        newState = UpdateState_Running(uI, eI);
        break;
    case State::Walking:
        newState = UpdateState_Walking(uI, eI);
        break;
    case State::Jumping:
        newState = UpdateState_Jumping(uI, eI);
        break;
    default:
        std::abort();
    }

    if(newState != state_)
    {
        switch(newState)
        {
        case State::Standing:
            InitState_Standing();
            break;
        case State::Running:
            InitState_Running();
            break;
        case State::Walking:
            InitState_Walking();
            break;
        case State::Jumping:
            InitState_Jumping();
            break;
        default:
            std::abort();
        }
    }
}

void Actor::UpdateActorPosition(float dT, ChunkManager *ckMgr)
{
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
}
