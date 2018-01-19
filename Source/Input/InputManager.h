/*================================================================
Filename: InputManager.h
Date: 2018.1.19
Created by AirGuanZ
================================================================*/
#ifndef VW_INPUT_H
#define VW_INPUT_H

#include <memory>

#include <Windows.h>
#include <Keyboard.h>
#include <Mouse.h>

#include "../Utility/Singleton.h"

class InputManager : public Singleton<InputManager>
{
public:
    InputManager(void);

    DirectX::Keyboard &GetKeyboard(void);
    DirectX::Mouse &GetMouse(void);

private:
    std::unique_ptr<DirectX::Keyboard> keyboard_;
    std::unique_ptr<DirectX::Mouse> mouse_;
};

#endif //VW_INPUT_H
