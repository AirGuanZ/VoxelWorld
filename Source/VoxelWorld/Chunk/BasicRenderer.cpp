/*================================================================
Filename: BasicRenderer.cpp
Date: 2018.1.13
Created by AirGuanZ
================================================================*/
#include <vector>

#include <Utility/HelperFunctions.h>

#include <Resource/ResourceName.h>
#include <Window/Window.h>
#include "BasicRenderer.h"

namespace
{
    ID3D11InputLayout *CreateInputLayout(const D3D11_INPUT_ELEMENT_DESC *desc,
        int num, const void *byteCode, int length)
    {
        assert(desc != nullptr && num > 0);
        assert(byteCode != nullptr && length > 0);
        assert(Window::GetInstance().IsD3DAvailable());

        ID3D11InputLayout *rt = nullptr;
        HRESULT hr = Window::GetInstance().GetD3DDevice()->CreateInputLayout(
            desc, static_cast<UINT>(num), byteCode, static_cast<SIZE_T>(length), &rt);
        return FAILED(hr) ? nullptr : rt;
    }
}

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
        errMsg = "Failed to load shader source for basic renderer";
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
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,
            Helper::MemOffset(&Vertex::pos), D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0,
            Helper::MemOffset(&Vertex::texCoord), D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "LIGHTCOLOR", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,
            Helper::MemOffset(&Vertex::lightColor), D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "SUNLIGHT", 0, DXGI_FORMAT_R32_FLOAT, 0,
            Helper::MemOffset(&Vertex::sunlight), D3D11_INPUT_PER_VERTEX_DATA, 0 }
    };
    inputLayout_ = CreateInputLayout(
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

BasicRenderer::ShaderType &BasicRenderer::GetShader(void)
{
    return shader_;
}
