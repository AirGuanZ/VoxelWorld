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
    mouse_ = std::make_unique<DirectX::Mouse>();
}

DirectX::Keyboard &InputManager::GetKeyboard(void)
{
    return *keyboard_;
}

DirectX::Mouse &InputManager::GetMouse(void)
{
    return *mouse_;
}
