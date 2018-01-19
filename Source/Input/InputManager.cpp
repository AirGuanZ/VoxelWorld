/*================================================================
Filename: InputManager.cpp
Date: 2018.1.19
Created by AirGuanZ
================================================================*/
#include "InputManager.h"

SINGLETON_CLASS_DEFINITION(InputManager);

InputManager::InputManager(void)
{
    keyboard_ = std::make_unique<DirectX::Keyboard>();
    kbState_ = keyboard_->GetState();
    kbTracker_.Reset();

    POINT curPos;
    GetCursorPos(&curPos);
    curPosX_ = curPos.x, curPosY_ = curPos.y;
    curMovX_ = curMovY_ = 0;
    wheelMov_ = 0;

    curLocked_ = false;
    curLockX_ = curLockY_ = 0;

    mbDown_    [0] = mbDown_    [1] = mbDown_    [2] = false;
    mbLast_    [0] = mbLast_    [1] = mbLast_    [2] = false;
    mbPressed_ [0] = mbPressed_ [1] = mbPressed_ [2] = false;
    mbReleased_[0] = mbReleased_[1] = mbReleased_[2] = false;
}

namespace
{
    inline DirectX::Keyboard::Keys Int2Keys(int vk)
    {
        return static_cast<DirectX::Keyboard::Keys>(vk);
    }
}

bool InputManager::IsKeyDown(int keyCode)
{
    return kbState_.IsKeyDown(Int2Keys(keyCode));
}

bool InputManager::IsKeyPressed(int keyCode)
{
    return kbTracker_.IsKeyPressed(Int2Keys(keyCode));
}

bool InputManager::IsKeyReleased(int keyCode)
{
    return kbTracker_.IsKeyReleased(Int2Keys(keyCode));
}

int InputManager::GetCursorPosX(void)
{
    return curPosX_;
}

int InputManager::GetCursorPosY(void)
{
    return curPosY_;
}

int InputManager::GetCursorMovX(void)
{
    return curMovX_;
}

int InputManager::GetCursorMovY(void)
{
    return curMovY_;
}

int InputManager::GetMouseWheel(void)
{
    return wheelMov_;
}

bool InputManager::IsMouseButtonDown(MouseButton button)
{
    return mbDown_[static_cast<int>(button)];
}

bool InputManager::IsMouseButtonPressed(MouseButton button)
{
    return mbPressed_[static_cast<int>(button)];
}

bool InputManager::IsMouseButtonReleased(MouseButton button)
{
    return mbReleased_[static_cast<int>(button)];
}

void InputManager::LockCursor(bool lock, int x, int y)
{
    curLocked_ = lock;
    curLockX_ = x, curLockY_ = y;
}

void InputManager::Update(void)
{
    kbState_ = keyboard_->GetState();
    kbTracker_.Update(kbState_);

    POINT cPos;
    GetCursorPos(&cPos);
    curMovX_ = cPos.x - curPosX_, curMovY_ = cPos.y - curPosY_;
    curPosX_ = cPos.x, curPosY_ = cPos.y;

    mbPressed_ [0] = !mbLast_[0] && mbDown_ [0];
    mbReleased_[0] = mbLast_ [0] && !mbDown_[0];

    mbPressed_ [1] = !mbLast_[1] && mbDown_ [1];
    mbReleased_[1] = mbLast_ [1] && !mbDown_[1];

    mbPressed_ [2] = !mbLast_[2] && mbDown_ [2];
    mbReleased_[2] = mbLast_ [2] && !mbDown_[2];

    std::memcpy(mbLast_, mbDown_, sizeof(mbLast_));

    if(curLocked_)
    {
        SetCursorPos(curLockX_, curLockY_);
        POINT curPos;
        GetCursorPos(&curPos);

        curPosX_ = curPos.x;
        curPosY_ = curPos.y;
    }
}
