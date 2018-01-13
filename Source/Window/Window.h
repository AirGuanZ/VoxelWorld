/*================================================================
Filename: Window.h
Date: 2018.1.12
Created by AirGuanZ
================================================================*/
#ifndef VW_WINDOW_H
#define VW_WINDOW_H

#include <string>

#include <Keyboard.h>
#include <Mouse.h>

#include "../Utility/Singleton.h"

class Window : public Singleton<Window>
{
public:
    bool InitWindow(int clientWidth, int clientHeight, const char *windowTitle, std::string &errMsg);
    bool InitD3D(int sampleCount, int sampleQuality, std::string &errMsg);

    bool IsWindowAvailable(void);
    bool IsD3DAvailable(void);

    void Destroy(void);

    int GetClientWidth(void);
    int GetClientHeight(void);
    float GetClientAspectRatio(void);

    void SetVsync(bool vsync);

    void SetBackgroundColor(float r, float g, float b, float a);
    void ClearRenderTarget(void);
    void ClearDepthStencil(void);

    void DoEvents(void);
    void Present(void);
};

#endif //VW_WINDOW_H
