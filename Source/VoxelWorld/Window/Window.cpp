/*================================================================
Filename: Window.cpp
Date: 2018.1.12
Created by AirGuanZ
================================================================*/
#include <algorithm>
#include <cassert>
#include <memory>
#include <string>

#include <Utility/D3D11Header.h>
#include <Utility/HelperFunctions.h>

#include <Windows.h>
#include <DirectXTK/Keyboard.h>

#include <Screen/GUISystem.h>
#include <Input/InputManager.h>
#include "Window.h"

SINGLETON_CLASS_DEFINITION(Window);

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

namespace
{
    constexpr DXGI_FORMAT SWAPCHAIN_BUFFER_FORMAT    = DXGI_FORMAT_R8G8B8A8_UNORM;
    constexpr DXGI_FORMAT DEPTHSTENCIL_BUFFER_FORMAT = DXGI_FORMAT_D24_UNORM_S8_UINT;

    //Win32窗口
    namespace Win
    {
        HWND hWnd = NULL;
        HINSTANCE hInstance = NULL;

        const std::wstring windowClassName = L"VoxelWorldWindow";
        std::wstring windowTitle = L"Voxel World";

        int clientLeft   = 0;
        int clientTop    = 0;
        int clientWidth  = 0;
        int clientHeight = 0;
        int clientCenX   = 0;
        int clientCenY   = 0;
    }

    //D3D上下文
    namespace D3D
    {
        IDXGISwapChain *swapChain = nullptr;

        ID3D11Device *device               = nullptr;
        ID3D11DeviceContext *deviceContext = nullptr;

        ID3D11RenderTargetView *renderTargetView = nullptr;

        ID3D11Texture2D *depthStencilBuffer      = nullptr;
        ID3D11DepthStencilView *depthStencilView = nullptr;

        bool isFullscreen = false;
        bool vsync        = true;

        float background[4];
    }
}

void UpdateClientPos(void)
{
    RECT rect;
    GetClientRect(Win::hWnd, &rect);
    POINT LT = { rect.left, rect.top }, RB = { rect.right, rect.bottom };
    ClientToScreen(Win::hWnd, &LT); ClientToScreen(Win::hWnd, &RB);

    Win::clientLeft = LT.x; Win::clientTop = LT.y;
    Win::clientWidth = RB.x - LT.x;
    Win::clientHeight = RB.y - LT.y;
    Win::clientCenX = Win::clientLeft + Win::clientWidth / 2;
    Win::clientCenY = Win::clientTop + Win::clientHeight / 2;
}

bool Window::InitWindow(int clientWidth, int clientHeight, const wchar_t *windowTitle, std::string &errMsg)
{
    assert(!IsWindowAvailable());
    assert(clientWidth > 0 && clientHeight > 0 && windowTitle);
    errMsg = "";

    if(!Win::hInstance)
        Win::hInstance = GetModuleHandle(NULL);

    WNDCLASSEX wc;
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
    if(!RegisterClassEx(&wc))
    {
        errMsg = u8"Failed to register window class";
        return false;
    }

    DWORD dwStyle = 0;
    dwStyle |= WS_VISIBLE;
    dwStyle &= ~WS_SIZEBOX;

    RECT winRect = { 0, 0, clientWidth, clientHeight };
    if(!AdjustWindowRect(&winRect, dwStyle, FALSE))
    {
        errMsg = u8"Failed to adjust window size";
        UnregisterClass(Win::windowClassName.c_str(), Win::hInstance);
        return false;
    }

    int scrWidth      = GetSystemMetrics(SM_CXSCREEN);
    int scrHeight     = GetSystemMetrics(SM_CYSCREEN);
    int realWinWidth  = winRect.right - winRect.left;
    int realWinHeight = winRect.bottom - winRect.top;
    int realWinLeft   = (scrWidth - realWinWidth) / 2;
    int realWinTop    = (scrHeight - realWinHeight) / 2;

    Win::windowTitle = windowTitle;
    Win::hWnd = CreateWindow(
        Win::windowClassName.c_str(), Win::windowTitle.c_str(),
        dwStyle, realWinLeft, realWinTop,
        realWinWidth, realWinHeight,
        NULL, NULL, Win::hInstance, NULL);
    if(!Win::hWnd)
    {
        errMsg = u8"Failed to create WIN32 window";
        UnregisterClass(Win::windowClassName.c_str(), Win::hInstance);
        return false;
    }

    ShowWindow(Win::hWnd, SW_SHOW);
    UpdateWindow(Win::hWnd);
    SetForegroundWindow(Win::hWnd);
    SetFocus(Win::hWnd);

    UpdateClientPos();

    return true;
}

static bool CreateD3DDevice(void)
{
    D3D_FEATURE_LEVEL featureLevel = D3D_FEATURE_LEVEL_11_0;
    UINT createDeviceFlag = 0;
#ifdef _DEBUG
    createDeviceFlag |= D3D11_CREATE_DEVICE_DEBUG;
#endif
    HRESULT hr = D3D11CreateDevice(
        nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr,
        createDeviceFlag,
        &featureLevel, 1,
        D3D11_SDK_VERSION,
        &D3D::device,
        nullptr,
        &D3D::deviceContext);
    if(FAILED(hr))
        return false;
    return true;
}

static bool CreateSwapChain(int sampleCount, int sampleQuality)
{
    DXGI_MODE_DESC scBuf;
    scBuf.Width                   = Win::clientWidth;
    scBuf.Height                  = Win::clientHeight;
    scBuf.RefreshRate.Numerator   = 60;
    scBuf.RefreshRate.Denominator = 1;
    scBuf.Format                  = SWAPCHAIN_BUFFER_FORMAT;
    scBuf.ScanlineOrdering        = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
    scBuf.Scaling                 = DXGI_MODE_SCALING_UNSPECIFIED;

    DXGI_SWAP_CHAIN_DESC sc;
    sc.BufferDesc         = scBuf;
    sc.BufferUsage        = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    sc.BufferCount        = 1;
    sc.OutputWindow       = Win::hWnd;
    sc.Windowed           = true;
    sc.SwapEffect         = DXGI_SWAP_EFFECT_DISCARD;
    sc.Flags              = 0;
    sc.SampleDesc.Count   = sampleCount;
    sc.SampleDesc.Quality = sampleQuality;

    IDXGIDevice *dxgiDevice = nullptr;
    HRESULT hr = D3D::device->QueryInterface<IDXGIDevice>(&dxgiDevice);
    if(FAILED(hr))
        return false;

    IDXGIAdapter *dxgiAdapter = nullptr;
    hr = dxgiDevice->GetParent(__uuidof(IDXGIAdapter), (void**)&dxgiAdapter);
    Helper::ReleaseCOMObjects(dxgiDevice);
    if(FAILED(hr))
        return false;

    IDXGIFactory *dxgiFactory = nullptr;
    hr = dxgiAdapter->GetParent(__uuidof(IDXGIFactory), (void**)&dxgiFactory);
    Helper::ReleaseCOMObjects(dxgiAdapter);
    if(FAILED(hr))
        return false;

    hr = dxgiFactory->CreateSwapChain(D3D::device, &sc, &D3D::swapChain);
    Helper::ReleaseCOMObjects(dxgiFactory);
    if(FAILED(hr))
        return false;

    return true;
}

static bool CreateRenderTargetView(void)
{
    assert(D3D::swapChain != nullptr);

    ID3D11Texture2D *backbuffer;
    HRESULT hr = D3D::swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&backbuffer));
    if(FAILED(hr))
        return false;

    hr = D3D::device->CreateRenderTargetView(backbuffer, nullptr, &D3D::renderTargetView);
    backbuffer->Release();
    if(FAILED(hr))
        return false;

    return true;
}

static bool CreateDepthStencilBuffer(int sampleCount, int sampleQuality)
{
    assert(D3D::swapChain != nullptr);

    D3D11_TEXTURE2D_DESC depthStencilBufDesc;
    depthStencilBufDesc.ArraySize          = 1;
    depthStencilBufDesc.BindFlags          = D3D11_BIND_DEPTH_STENCIL;
    depthStencilBufDesc.CPUAccessFlags     = 0;
    depthStencilBufDesc.Format             = DEPTHSTENCIL_BUFFER_FORMAT;
    depthStencilBufDesc.Width              = Win::clientWidth;
    depthStencilBufDesc.Height             = Win::clientHeight;
    depthStencilBufDesc.MipLevels          = 1;
    depthStencilBufDesc.MiscFlags          = 0;
    depthStencilBufDesc.SampleDesc.Count   = sampleCount;
    depthStencilBufDesc.SampleDesc.Quality = sampleQuality;
    depthStencilBufDesc.Usage              = D3D11_USAGE_DEFAULT;

    HRESULT hr = D3D::device->CreateTexture2D(
        &depthStencilBufDesc, nullptr,
        &D3D::depthStencilBuffer);
    if(FAILED(hr))
        return false;

    D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
    depthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    depthStencilViewDesc.ViewDimension = sampleCount > 1 ? D3D11_DSV_DIMENSION_TEXTURE2DMS :
                                                           D3D11_DSV_DIMENSION_TEXTURE2D;
    depthStencilViewDesc.Flags = 0;
    depthStencilViewDesc.Texture2D.MipSlice = 0;

    hr = D3D::device->CreateDepthStencilView(
        D3D::depthStencilBuffer,
        &depthStencilViewDesc,
        &D3D::depthStencilView);
    if(FAILED(hr))
        return false;

    return true;
}

static void SetDefaultViewport(void)
{
    assert(D3D::deviceContext != nullptr);

    D3D11_VIEWPORT vp;
    vp.TopLeftX = 0;
    vp.TopLeftY = 0;
    vp.Width    = static_cast<FLOAT>(Win::clientWidth);
    vp.Height   = static_cast<FLOAT>(Win::clientHeight);
    vp.MaxDepth = 1.0f;
    vp.MinDepth = 0.0f;
    D3D::deviceContext->RSSetViewports(1, &vp);
}

static void DestroyGUISystem(void)
{
    GUI::Destroy();
}

static void DestroyD3D(void)
{
    using Helper::ReleaseCOMObjects;
    using namespace D3D;

    GUI::Destroy();

    isFullscreen = false;
    vsync = true;

    background[0] = background[1] =
    background[2] = background[3] = 0.0f;

    ReleaseCOMObjects(swapChain, deviceContext);
    ReleaseCOMObjects(renderTargetView);
    ReleaseCOMObjects(depthStencilBuffer, depthStencilView);

#ifdef _DEBUG
    if(device)
    {
        ID3D11Debug *debug = nullptr;
        HRESULT hr = device->QueryInterface<ID3D11Debug>(&debug);
        if(SUCCEEDED(hr))
            debug->ReportLiveDeviceObjects(D3D11_RLDO_DETAIL);
        ReleaseCOMObjects(debug);
    }
#endif
    ReleaseCOMObjects(device);
}

static void DestroyWin(void)
{
    using namespace Win;

    clientLeft = clientTop = 0;
    clientWidth = clientHeight = 0;
    if(hWnd)
    {
        DestroyWindow(hWnd);
        hWnd = NULL;
    }

    UnregisterClass(windowClassName.c_str(), hInstance);
}

bool Window::InitD3D(int sampleCount, int sampleQuality, std::string &errMsg)
{
    assert(IsWindowAvailable() && !IsD3DAvailable());
    HRESULT hr;
    errMsg = u8"";

    // device & device context

    if(!CreateD3DDevice())
    {
        errMsg = u8"Failed to create D3D device";
        return false;
    }

    // MSAA

    UINT maxMSAAQuality = 0;
    hr = D3D::device->CheckMultisampleQualityLevels(
        SWAPCHAIN_BUFFER_FORMAT, sampleCount, &maxMSAAQuality);
    if(FAILED(hr) || static_cast<UINT>(sampleQuality) >= maxMSAAQuality)
    {
        errMsg = u8"MSAA quality unsupported. SampleCount = " +
                 std::to_string(sampleCount) +
                 u8", SampleQuality = " +
                 std::to_string(sampleQuality) +
                 u8", SampleQuality Suppported is less than " +
                 std::to_string(maxMSAAQuality);
        DestroyD3D();
        return false;
    }

    // swap chain

    if(!CreateSwapChain(sampleCount, sampleQuality))
    {
        errMsg = u8"Failed to create swap chain";
        DestroyD3D();
        return false;
    }

    // render target view

    if(!CreateRenderTargetView())
    {
        errMsg = u8"Failed to create render target view";
        DestroyD3D();
        return false;
    }

    // depth stencil buffer & view

    if(!CreateDepthStencilBuffer(sampleCount, sampleQuality))
    {
        errMsg = u8"Failed to create depth stencil buffer";
        DestroyD3D();
        return false;
    }
    D3D::deviceContext->OMSetRenderTargets(1, &D3D::renderTargetView, D3D::depthStencilView);

    // viewport

    SetDefaultViewport();

    //primitive topology

    D3D::deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    return true;
}

bool Window::InitGUI(const std::vector<GUI::ImFontSpec> &fonts, std::string &errMsg)
{
    return GUI::Initialize(fonts, errMsg);
}

bool Window::IsWindowAvailable(void)
{
    return Win::hWnd != NULL;
}

bool Window::IsD3DAvailable(void)
{
    return D3D::device != nullptr;
}

void Window::Destroy(void)
{
    DestroyGUISystem();
    DestroyD3D();
    DestroyWin();
}

int Window::GetClientWidth(void)
{
    return Win::clientWidth;
}

int Window::GetClientHeight(void)
{
    return Win::clientHeight;
}

float Window::GetClientAspectRatio(void)
{
    assert(Win::clientWidth > 0 && Win::clientHeight > 0);
    return static_cast<float>(Win::clientWidth) / Win::clientHeight;
}

void Window::SetVsync(bool vsync)
{
    D3D::vsync = vsync;
}

void Window::SetBackgroundColor(float r, float g, float b, float a)
{
    using D3D::background;
    background[0] = r; background[1] = g;
    background[2] = b; background[3] = a;
}

void Window::ClearRenderTarget(void)
{
    using namespace D3D;
    assert(deviceContext != nullptr && renderTargetView != nullptr);
    deviceContext->ClearRenderTargetView(renderTargetView, background);
}

void Window::ClearDepthStencil(void)
{
    using namespace D3D;
    assert(deviceContext != nullptr && depthStencilView != nullptr);
    deviceContext->ClearDepthStencilView(
        depthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
}

int Window::ClientCentreX(void)
{
    return Win::clientCenX;
}

int Window::ClientCentreY(void)
{
    return Win::clientCenY;
}

void Window::DoEvents(void)
{
    InputManager::GetInstance().wheelMov_ = 0;
    MSG msg;
    while(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    InputManager::GetInstance().Update();
}

void Window::Present(void)
{
    assert(D3D::swapChain != nullptr);
    D3D::swapChain->Present(D3D::vsync ? 1 : 0, 0);
}

void Window::SetFullscreen(bool f)
{
    D3D::swapChain->SetFullscreenState(f ? TRUE : FALSE, nullptr);
}

ID3D11Device *Window::GetD3DDevice(void)
{
    return D3D::device;
}

ID3D11DeviceContext *Window::GetD3DDeviceContext(void)
{
    return D3D::deviceContext;
}

HWND Window::GetWindowHandle(void)
{
    return Win::hWnd;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    using DirectX::Keyboard;
    InputManager &input = InputManager::GetInstance();

    static bool paused = false;

    switch(msg)
    {
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    case WM_ACTIVATEAPP:
        Keyboard::ProcessMessage(msg, wParam, lParam);
        break;
    case WM_KEYDOWN:
    case WM_SYSKEYDOWN:
        GUI::KeyDown(static_cast<int>(wParam));
        Keyboard::ProcessMessage(msg, wParam, lParam);
        break;
    case WM_KEYUP:
    case WM_SYSKEYUP:
        GUI::KeyUp(static_cast<int>(wParam));
        Keyboard::ProcessMessage(msg, wParam, lParam);
        break;
    case WM_LBUTTONDOWN:
        input.mbDown_[0] = true;
        GUI::MouseButtonDown(MouseButton::Left);
        break;
    case WM_LBUTTONUP:
        input.mbDown_[0] = false;
        GUI::MouseButtonUp(MouseButton::Left);
        break;
    case WM_MBUTTONDOWN:
        input.mbDown_[1] = true;
        GUI::MouseButtonDown(MouseButton::Middle);
        break;
    case WM_MBUTTONUP:
        input.mbDown_[1] = false;
        GUI::MouseButtonUp(MouseButton::Middle);
        break;
    case WM_RBUTTONDOWN:
        input.mbDown_[2] = true;
        GUI::MouseButtonDown(MouseButton::Right);
        break;
    case WM_RBUTTONUP:
        input.mbDown_[2] = false;
        GUI::MouseButtonUp(MouseButton::Right);
        break;
    case WM_MOUSEWHEEL:
    {
        int wh = GET_WHEEL_DELTA_WPARAM(wParam) / WHEEL_DELTA;
        input.wheelMov_ = wh;
        GUI::MouseWheel(wh);
    }
        break;
    case WM_SETFOCUS:
        paused = false;
        break;
    case WM_KILLFOCUS:
    {
        InputManager &input = InputManager::GetInstance();

        paused = true;

        bool showCursor = input.ShowCursor();
        if(!showCursor)
            input.ShowCursor(true);

        bool lockCursor = input.LockCursor();
        if(lockCursor)
            input.LockCursor(false);

        while(paused)
            Window::GetInstance().DoEvents();

        if(!showCursor)
            input.ShowCursor(false);

        if(lockCursor)
            input.LockCursor(true);
    }
        break;
    case WM_CHAR:
        GUI::Char(static_cast<unsigned int>(wParam));
        break;
    }

    return DefWindowProc(hWnd, msg, wParam, lParam);
}
