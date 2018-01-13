/*================================================================
Filename: BasicRenderer.cpp
Date: 2018.1.13
Created by AirGuanZ
================================================================*/
#include <vector>

#include "../Resource/ResourceName.h"
#include "../Utility/HelperFunctions.h"
#include "../Window/Window.h"
#include "BasicRenderer.h"

BasicRenderer::BasicRenderer(void)
    : inputLayout_(nullptr)
{

}

BasicRenderer::~BasicRenderer(void)
{
    Destroy();
}

bool BasicRenderer::Initialize(std::string &errMsg)
{
    assert(Window::GetInstance().IsD3DAvailable());
    errMsg = "";
    ID3D11Device *dev = Window::GetInstance().GetD3DDevice();

    std::string vsSrc, psSrc;
    if(!Helper::ReadFile(BASIC_RENDERER_VERTEX_SHADER, vsSrc) ||
       !Helper::ReadFile(BASIC_RENDERER_PIXEL_SHADER, psSrc))
    {
        errMsg = "Failed to load shader source";
        return false;
    }

    if(!shader_.InitStage<SS_VS>(dev, vsSrc, &errMsg) ||
       !shader_.InitStage<SS_PS>(dev, psSrc, &errMsg))
    {
        Destroy();
        return false;
    }

    D3D11_INPUT_ELEMENT_DESC inputLayoutDesc[] =
    {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, Helper::MemOffset(&Vertex::pos), D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, Helper::MemOffset(&Vertex::texCoord), D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, Helper::MemOffset(&Vertex::normal), D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "LIGHTCOLOR", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, Helper::MemOffset(&Vertex::lightColor), D3D11_INPUT_PER_VERTEX_DATA, 0 }
    };
    inputLayout_ = Helper::CreateInputLayout(
        inputLayoutDesc, Helper::ArraySize(inputLayoutDesc),
        shader_.GetShaderByteCodeWithInputSignature(),
        shader_.GetShaderByteCodeSizeWithInputSignature());
    if(!inputLayout_)
    {
        Destroy();
        return false;
    }

    return true;
}

void BasicRenderer::Destroy(void)
{
    Helper::ReleaseCOMObjects(inputLayout_);
    shader_.Destroy();
}

bool BasicRenderer::IsAvailable(void) const
{
    return inputLayout_ && shader_.IsAllStagesAvailable();
}

void BasicRenderer::Begin(void)
{
    assert(Window::GetInstance().IsD3DAvailable());
    ID3D11DeviceContext *DC = Window::GetInstance().GetD3DDeviceContext();
    DC->IASetInputLayout(inputLayout_);
    shader_.Bind(DC);
}

void BasicRenderer::End(void)
{
    assert(Window::GetInstance().IsD3DAvailable());
    ID3D11DeviceContext *DC = Window::GetInstance().GetD3DDeviceContext();
    DC->IASetInputLayout(nullptr);
    shader_.Unbind(DC);
}
