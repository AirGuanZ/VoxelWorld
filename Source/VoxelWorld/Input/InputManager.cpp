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

    showCursor_ = true;
}

InputManager::~InputManager(void)
{
    this->ShowCursor(true);
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
