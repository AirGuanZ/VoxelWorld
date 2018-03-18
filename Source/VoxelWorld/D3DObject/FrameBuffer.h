/*================================================================
Filename: FrameBuffer.h
Date: 2018.3.18
Created by AirGuanZ
================================================================*/
#pragma once

#include <Utility\D3D11Header.h>
#include <Utility\Uncopiable.h>

class BasicFrameBuffer : public Uncopiable
{
public:
    BasicFrameBuffer(void);
    ~BasicFrameBuffer(void);

    bool Initialize(int width, int height);
    bool IsAvailable(void) const;
    void Destroy(void);

    void Begin(void);
    void End(void);

    void ClearRenderTargetView(float r = 0.0f, float g = 0.0f, float b = 0.0f, float a = 0.0f);
    void ClearDepthStencilView(float depth = 1.0f);

    ID3D11ShaderResourceView *GetSRV(void);

private:
    void Destroy_Unchecked(void);

private:
    int width_;
    int height_;

    ID3D11RenderTargetView *texRTV_;
    ID3D11ShaderResourceView *texSRV_;
    ID3D11DepthStencilView *depthStencilView_;
};
