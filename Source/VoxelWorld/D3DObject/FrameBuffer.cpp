/*================================================================
Filename: FrameBuffer.cpp
Date: 2018.3.18
Created by AirGuanZ
================================================================*/
#include <cassert>

#include <Utility\HelperFunctions.h>

#include <Window\Window.h>
#include "FrameBuffer.h"

BasicFrameBuffer::BasicFrameBuffer(void)
{
    width_ = height_ = 0;
    texRTV_ = nullptr;
    texSRV_ = nullptr;
    depthStencilView_ = nullptr;
}

BasicFrameBuffer::~BasicFrameBuffer(void)
{
    Destroy();
}

bool BasicFrameBuffer::Initialize(int width, int height)
{
    assert(width > 0 && height > 0);
    Destroy();
    HRESULT hr;
    ID3D11Texture2D *tex = nullptr, *depth = nullptr;
    ID3D11Device *dev = Window::GetInstance().GetD3DDevice();

    width_ = width;
    height_ = height;

    //render target viewºÍshader resource view

    D3D11_TEXTURE2D_DESC texDesc;
    texDesc.Width              = static_cast<UINT>(width);
    texDesc.Height             = static_cast<UINT>(height);
    texDesc.MipLevels          = 1;
    texDesc.ArraySize          = 1;
    texDesc.Format             = DXGI_FORMAT_R8G8B8A8_UNORM;
    texDesc.SampleDesc.Count   = 1;
    texDesc.SampleDesc.Quality = 0;
    texDesc.Usage              = D3D11_USAGE_DEFAULT;
    texDesc.BindFlags          = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
    texDesc.CPUAccessFlags     = 0;
    texDesc.MiscFlags          = 0;

    hr = dev->CreateTexture2D(&texDesc, nullptr, &tex);
    if(FAILED(hr))
        goto FAILED;

    hr = dev->CreateShaderResourceView(tex, nullptr, &texSRV_);
    if(FAILED(hr))
        goto FAILED;

    hr = dev->CreateRenderTargetView(tex, nullptr, &texRTV_);
    if(FAILED(hr))
        goto FAILED;

    Helper::ReleaseCOMObjects(tex);

    //depth stencil view

    D3D11_TEXTURE2D_DESC depthDesc;
    depthDesc.Width = static_cast<UINT>(width);
    depthDesc.Height = static_cast<UINT>(height);
    depthDesc.MipLevels = 1;
    depthDesc.ArraySize = 1;
    depthDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    depthDesc.SampleDesc.Count = 1;
    depthDesc.SampleDesc.Quality = 0;
    depthDesc.Usage = D3D11_USAGE_DEFAULT;
    depthDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
    depthDesc.CPUAccessFlags = 0;
    depthDesc.MiscFlags = 0;

    hr = dev->CreateTexture2D(&depthDesc, nullptr, &depth);
    if(FAILED(hr))
        goto FAILED;

    D3D11_DEPTH_STENCIL_VIEW_DESC DSVDesc;
    DSVDesc.Flags = 0;
    DSVDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    DSVDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
    DSVDesc.Texture2D.MipSlice = 0;

    hr = dev->CreateDepthStencilView(depth, &DSVDesc, &depthStencilView_);
    if(FAILED(hr))
        goto FAILED;

    Helper::ReleaseCOMObjects(depth);

    return true;

FAILED:
    Helper::ReleaseCOMObjects(tex, depth);
    Destroy_Unchecked();
    return false;
}

bool BasicFrameBuffer::IsAvailable(void) const
{
    return width_ > 0;
}

void BasicFrameBuffer::Destroy(void)
{
    if(IsAvailable())
        Destroy_Unchecked();
}

void BasicFrameBuffer::Begin(void)
{
    ID3D11DeviceContext *DC = Window::GetInstance().GetD3DDeviceContext();

    D3D11_VIEWPORT viewport;
    viewport.TopLeftX = 0.0f;
    viewport.TopLeftY = 0.0f;
    viewport.Height   = static_cast<float>(height_);
    viewport.Width    = static_cast<float>(width_);
    viewport.MaxDepth = 1.0f;
    viewport.MinDepth = 0.0f;

    DC->OMSetRenderTargets(1, &texRTV_, depthStencilView_);
    DC->RSSetViewports(1, &viewport);
}

void BasicFrameBuffer::End(void)
{
    Window::GetInstance().SetDefaultRTVAndDSV();
    Window::GetInstance().SetDefaultViewport();
}

void BasicFrameBuffer::ClearRenderTargetView(float r, float g, float b, float a)
{
    FLOAT backgroundColor[] = { r, g, b, a };
    Window::GetInstance().GetD3DDeviceContext()->ClearRenderTargetView(texRTV_, backgroundColor);
}

void BasicFrameBuffer::ClearDepthStencilView(float depth)
{
    Window::GetInstance().GetD3DDeviceContext()->ClearDepthStencilView(depthStencilView_, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, depth, 0);
}

ID3D11ShaderResourceView *BasicFrameBuffer::GetSRV(void)
{
    return texSRV_;
}

void BasicFrameBuffer::Destroy_Unchecked(void)
{
    width_ = height_ = 0;
    Helper::ReleaseCOMObjects(texRTV_, texSRV_, depthStencilView_);
}
