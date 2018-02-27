/*================================================================
Filename: InputManager.h
Date: 2018.1.19
Created by AirGuanZ
================================================================*/
#pragma once

#include <memory>

#include <Windows.h>
#include <DirectXTK/Keyboard.h>

#include <Utility/Singleton.h>

enum class MouseButton
{
    Left        = 0,
    Middle      = 1,
    Right       = 2
};

inline DirectX::Keyboard::Keys Int2Keys(int vk)
{
    return static_cast<DirectX::Keyboard::Keys>(vk);
}

class InputManager : public Singleton<InputManager>
{
public:
    InputManager(void);
    ~InputManager(void);

    //按键是否处于“按压”的状态
    bool IsKeyDown    (int keyCode)
    {
        return kbState_.IsKeyDown(Int2Keys(keyCode));
    }

    //按键是否刚刚经历“按下”的动作
    bool IsKeyPressed (int keyCode)
    {
        return kbTracker_.IsKeyPressed(Int2Keys(keyCode));
    }

    //按键是否刚刚经历“释放”的动作
    bool IsKeyReleased(int keyCode)
    {
        return kbTracker_.IsKeyReleased(Int2Keys(keyCode));
    }

    int GetCursorPosX(void)
    {
        return curPosX_;
    }

    int GetCursorPosY(void)
    {
        return curPosY_;
    }

    int GetCursorMovX(void)
    {
        return curMovX_;
    }

    int GetCursorMovY(void)
    {
        return curMovY_;
    }

    int GetMouseWheel(void)
    {
        return wheelMov_;
    }

    bool IsMouseButtonDown(MouseButton button)
    {
        return mbDown_[static_cast<int>(button)];
    }

    bool IsMouseButtonPressed(MouseButton button)
    {
        return mbPressed_[static_cast<int>(button)];
    }

    bool IsMouseButtonReleased(MouseButton button)
    {
        return mbReleased_[static_cast<int>(button)];
    }

    void LockCursor(bool lock, int x, int y)
    {
        curLocked_ = lock;
        curLockX_ = x, curLockY_ = y;
    }

    void LockCursor(bool lock)
    {
        curLocked_ = lock;
    }

    bool LockCursor(void) const
    {
        return curLocked_;
    }

    void ShowCursor(bool show)
    {
        //这里“showCursor_ = show”不是==，就是为了赋值
        ::ShowCursor((showCursor_ = show) ? TRUE : FALSE);
    }

    bool ShowCursor(void) const
    {
        return showCursor_;
    }

private:
    friend class Window;
    friend LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

    void Update(void);

private:
    std::unique_ptr<DirectX::Keyboard> keyboard_;
    DirectX::Keyboard::State kbState_;
    DirectX::Keyboard::KeyboardStateTracker kbTracker_;

    //消息循环需要手动设置mbDown和wheelMov
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

    bool showCursor_;
};
