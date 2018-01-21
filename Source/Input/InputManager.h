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

#include "../Utility/Singleton.h"

enum class MouseButton
{
    Left        = 0,
    Middle      = 1,
    Right       = 2
};

class InputManager : public Singleton<InputManager>
{
public:
    InputManager(void);
    ~InputManager(void);

    bool IsKeyDown    (int keyCode);
    bool IsKeyPressed (int keyCode);
    bool IsKeyReleased(int keyCode);

    int GetCursorPosX(void);
    int GetCursorPosY(void);

    int GetCursorMovX(void);
    int GetCursorMovY(void);

    int GetMouseWheel(void);

    bool IsMouseButtonDown(MouseButton button);
    bool IsMouseButtonPressed(MouseButton button);
    bool IsMouseButtonReleased(MouseButton button);

    void LockCursor(bool lock, int x = 0, int y = 0);
    void ShowCursor(bool show);

private:
    friend class Window;
    friend LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

    void Update(void);

private:
    std::unique_ptr<DirectX::Keyboard> keyboard_;
    DirectX::Keyboard::State kbState_;
    DirectX::Keyboard::KeyboardStateTracker kbTracker_;

    //消息循环需要设置mbDown和wheelMov
    //其他状态更新都由Update完成

    int curPosX_, curPosY_;
    int curMovX_, curMovY_;
    int wheelMov_;

    bool curLocked_;
    int curLockX_, curLockY_;

    bool mbDown_      [3];
    bool mbLast_      [3];
    bool mbPressed_   [3];
    bool mbReleased_  [3];
};

#endif //VW_INPUT_H
