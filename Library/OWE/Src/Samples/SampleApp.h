/*================================================================
Filename: SampleApp.h
Date: 2017.12.10
Created by AirGuanZ
================================================================*/
#ifndef SAMPLE_APP_H
#define SAMPLE_APP_H

#include <string>

#include <d3d11.h>
#include <DirectXMath.h>

class SampleApp
{
protected:
    HINSTANCE hInstance_ = NULL;
    HWND hWnd_ = NULL;

    IDXGISwapChain *swapChain_ = nullptr;

    ID3D11Device *D3D_ = nullptr;
    ID3D11DeviceContext *DC_ = nullptr;

    ID3D11Texture2D *depthStencilBuffer_ = nullptr;
    ID3D11DepthStencilState *depthStencilState_ = nullptr;
    ID3D11DepthStencilView *depthStencilView_ = nullptr;
    ID3D11RenderTargetView *renderTargetView_ = nullptr;

    bool InitD3DContext(void);
    void DestroyD3DContext(void);

    static std::string ReadFile(const std::string &filename);

    template<typename ClassType, typename MemType>
    static size_t MemOffset(MemType ClassType::* pMem)
    {
        return reinterpret_cast<size_t>(&(reinterpret_cast<ClassType*>(0)->*pMem));
    }
};

#endif //SAMPLE_APP_H
