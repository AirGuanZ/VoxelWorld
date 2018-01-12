/*================================================================
Filename: Window.cpp
Date: 2018.1.12
Created by AirGuanZ
================================================================*/
#include <algorithm>
#include <memory>
#include <string>

#include <Windows.h>
#include <Keyboard.h>
#include <Mouse.h>

#include "../Utility/D3D11Header.h"
#include "../Utility/HelperFunctions.h"
#include "Window.h"

SINGLETON_CLASS_DEFINITION(Window);

LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

namespace
{
    //键鼠输入
    namespace Input
    {
        //Keyboard和Mouse都是单例
        std::unique_ptr<DirectX::Keyboard> keyboard = std::make_unique<DirectX::Keyboard>();
        std::unique_ptr<DirectX::Mouse>    mouse = std::make_unique<DirectX::Mouse>();
    }

    //Win32窗口
    namespace Win
    {
        HWND hWnd = NULL;
        HINSTANCE hInstance = NULL;

        const std::string windowClassName = "VoxelWorldWindow";
        std::string windowTitle = "Voxel World";

        int clientLeft;
        int clientTop;
        int clientWidth;
        int clientHeight;
    }

    //D3D上下文
    namespace D3D
    {
        IDXGISwapChain *swapChain;

        ID3D11Device *device;
        ID3D11DeviceContext *deviceContext;

        bool isFullscreen;
    }
}

bool Window::InitWindow(int clientWidth, int clientHeight, const char *windowTitle, std::string &errMsg)
{
    assert(!IsWindowAvailable());
    assert(clientWidth > 0 && clientHeight > 0 && windowTitle);
    errMsg = "";

    if(!Win::hInstance)
        Win::hInstance = GetModuleHandle(NULL);

    WNDCLASSEXA wc;
    wc.cbSize        = sizeof(WNDCLASSEXA);
    wc.style         = CS_HREDRAW | CS_VREDRAW | CS_NOCLOSE;
    wc.lpfnWndProc   = WndProc;
    wc.cbClsExtra    = 0;
    wc.cbWndExtra    = 0;
    wc.hInstance     = Win::hInstance;
    wc.hIcon         = NULL;
    wc.hCursor       = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_BACKGROUND + 1);
    wc.lpszMenuName  = NULL;
    wc.lpszClassName = Win::windowClassName.c_str();
    wc.hIconSm       = NULL;
    if(!RegisterClassExA(&wc))
    {
        errMsg = "Failed to register window class";
        return false;
    }

    DWORD dwStyle = 0;
    dwStyle |= WS_OVERLAPPED;
    dwStyle |= WS_POPUP;
    dwStyle |= WS_SIZEBOX;
    dwStyle |= WS_VISIBLE;

    RECT winRect = { 0, 0, clientWidth, clientHeight };
    if(!AdjustWindowRect(&winRect, dwStyle, FALSE))
    {
        errMsg = "Failed to adjust window size";
        UnregisterClassA(Win::windowClassName.c_str(), Win::hInstance);
        return false;
    }

    int scrWidth      = GetSystemMetrics(SM_CXSCREEN);
    int scrHeight     = GetSystemMetrics(SM_CYSCREEN);
    int realWinWidth  = winRect.right - winRect.left;
    int realWinHeight = winRect.bottom - winRect.top;
    int realWinLeft   = (scrWidth - realWinWidth) / 2;
    int realWinTop    = (scrHeight - realWinHeight) / 2;

    Win::windowTitle = windowTitle;
    Win::hWnd = CreateWindowA(
        Win::windowClassName.c_str(), Win::windowTitle.c_str(),
        dwStyle, realWinLeft, realWinTop,
        realWinWidth, realWinHeight,
        NULL, NULL, Win::hInstance, NULL);
    if(!Win::hWnd)
    {
        errMsg = "Failed to create WIN32 window";
        UnregisterClassA(Win::windowClassName.c_str(), Win::hInstance);
        return false;
    }

    ShowWindow(Win::hWnd, SW_SHOW);
    UpdateWindow(Win::hWnd);
    SetForegroundWindow(Win::hWnd);
    SetFocus(Win::hWnd);

    RECT clientRect;
    GetClientRect(Win::hWnd, &clientRect);
    Win::clientLeft   = static_cast<int>(clientRect.left);
    Win::clientTop    = static_cast<int>(clientRect.top);
    Win::clientWidth  = static_cast<int>(clientRect.right - clientRect.left);
    Win::clientHeight = static_cast<int>(clientRect.bottom - clientRect.top);

    return true;
}

static void DestroyD3D(void)
{
    using Helper::ReleaseCOMObjects;
    using namespace D3D;

    ReleaseCOMObjects(swapChain, device, deviceContext);
}

bool Window::InitD3D(int sampleCount, int sampleQuality, std::string &errMsg)
{
    assert(IsWindowAvailable() && !IsD3DAvailable());
    HRESULT hr;
    errMsg = "";

    // device & device context

    D3D_FEATURE_LEVEL featureLevel = D3D_FEATURE_LEVEL_11_0;
    UINT createDeviceFlag = 0;
#ifdef _DEBUG
    createDeviceFlag |= D3D11_CREATE_DEVICE_DEBUG;
#endif
    hr = D3D11CreateDevice(
        nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr,
        createDeviceFlag,
        &featureLevel, 1,
        D3D11_SDK_VERSION,
        &D3D::device,
        nullptr,
        &D3D::deviceContext);
    if(FAILED(hr))
    {
        errMsg = "Failed to create D3D device";
        return false;
    }

    // MSAA

    UINT maxMSAAQuality;
    hr = D3D::device->CheckMultisampleQualityLevels(
        DXGI_FORMAT_R8G8B8A8_UNORM, sampleCount, &maxMSAAQuality);
    if(static_cast<UINT>(sampleQuality) >= maxMSAAQuality)
    {
        errMsg = "MSAA quality unsupported. SampleCount = " +
                 std::to_string(sampleCount) +
                 ", SampleQuality = " +
                 std::to_string(sampleQuality) +
                 ", SampleQuality Suppported is less than " +
                 std::to_string(maxMSAAQuality - 1);
        DestroyD3D();
        return false;
    }

    // swap chain

    DXGI_MODE_DESC scBuf;
    scBuf.Width = Win::clientWidth;
    scBuf.Height = Win::clientHeight;
    scBuf.RefreshRate.Numerator = 60;
    scBuf.RefreshRate.Denominator = 1;
    scBuf.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    scBuf.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
    scBuf.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

    DXGI_SWAP_CHAIN_DESC sc;
    sc.BufferDesc = scBuf;
    sc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    sc.BufferCount = 1;
    sc.OutputWindow = Win::hWnd;
    sc.Windowed = true;
    sc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
    sc.Flags = 0;
    sc.SampleDesc.Count = sampleCount;
    sc.SampleDesc.Quality = sampleQuality;

    IDXGIDevice *dxgiDevice = nullptr;
    hr = D3D::device->QueryInterface<IDXGIDevice>(&dxgiDevice);
    if(FAILED(hr))
    {
        errMsg = "Failed to query interface: IDXGIDevice";
        DestroyD3D();
        return false;
    }

    IDXGIAdapter *dxgiAdapter = nullptr;
    hr = dxgiDevice->GetParent(__uuidof(IDXGIAdapter), (void**)&dxgiAdapter);
    Helper::ReleaseCOMObjects(dxgiDevice);
    if(FAILED(hr))
    {
        errMsg = "Failed to query interface: IDXGIAdapter";
        DestroyD3D();
        return false;
    }

    IDXGIFactory *dxgiFactory = nullptr;
    hr = dxgiAdapter->GetParent(__uuidof(IDXGIFactory), (void**)&dxgiFactory);
    Helper::ReleaseCOMObjects(dxgiAdapter);
    if(FAILED(hr))
    {
        errMsg = "Failed to query interface: IDXGIFactory";
        DestroyD3D();
        return false;
    }

    hr = dxgiFactory->CreateSwapChain(D3D::device, &sc, &D3D::swapChain);
    Helper::ReleaseCOMObjects(dxgiFactory);
    if(FAILED(hr))
    {
        errMsg = "Failed to create swap chain";
        DestroyD3D();
        return false;
    }

    // render target view



    return true;
}

bool Window::IsWindowAvailable(void)
{
    return Win::hWnd != NULL;
}

bool Window::IsD3DAvailable(void)
{
    return D3D::device != nullptr;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    using DirectX::Keyboard;
    using DirectX::Mouse;

    switch(msg)
    {
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    case WM_ACTIVATEAPP:
        Keyboard::ProcessMessage(msg, wParam, lParam);
        Mouse::ProcessMessage(msg, wParam, lParam);
        break;
    case WM_KEYDOWN:
    case WM_SYSKEYDOWN:
    case WM_KEYUP:
    case WM_SYSKEYUP:
        Keyboard::ProcessMessage(msg, wParam, lParam);
        break;
    case WM_INPUT:
    case WM_MOUSEMOVE:
    case WM_LBUTTONDOWN:
    case WM_LBUTTONUP:
    case WM_RBUTTONDOWN:
    case WM_RBUTTONUP:
    case WM_MBUTTONDOWN:
    case WM_MBUTTONUP:
    case WM_MOUSEWHEEL:
    case WM_XBUTTONDOWN:
    case WM_XBUTTONUP:
    case WM_MOUSEHOVER:
        Mouse::ProcessMessage(msg, wParam, lParam);
        break;
    case WM_SIZE:
        Win::clientWidth = LOWORD(wParam);
        Win::clientHeight = HIWORD(wParam);
        if(D3D::swapChain)
            D3D::swapChain->ResizeBuffers(0, Win::clientWidth, Win::clientHeight,
                                          DXGI_FORMAT_R8G8B8A8_UNORM, 0);
        break;
    }

    return DefWindowProc(hWnd, msg, wParam, lParam);
}
