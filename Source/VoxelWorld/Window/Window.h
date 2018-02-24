/*================================================================
Filename: Window.h
Date: 2018.1.12
Created by AirGuanZ
================================================================*/
#ifndef VW_WINDOW_H
#define VW_WINDOW_H

#include <string>

#include <Utility/D3D11Header.h>
#include <Utility/Singleton.h>

class Window : public Singleton<Window>
{
public:
    bool InitWindow(int clientWidth, int clientHeight, const wchar_t *windowTitle, std::string &errMsg);
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

    int ClientCentreX(void);
    int ClientCentreY(void);

    void DoEvents(void);
    void Present(void);

    void SetFullscreen(bool fullscreen);

    ID3D11Device *GetD3DDevice(void);
    ID3D11DeviceContext *GetD3DDeviceContext(void);

    HWND GetWindowHandle(void);
};

#endif //VW_WINDOW_H
