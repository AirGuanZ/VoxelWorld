/*================================================================
Filename: CarveRenderer.cpp
Date: 2018.1.22
Created by AirGuanZ
================================================================*/
#include <Utility/FileSystem.h>
#include <Utility/HelperFunctions.h>

#include <Resource/ResourceNameManager.h>
#include "CarveRenderer.h"

CarveRenderer::CarveRenderer(void)
{

}

CarveRenderer::~CarveRenderer(void)
{
    Destroy();
}

bool CarveRenderer::Initialize(std::string &errMsg)
{
    RscNameMgr &rM = RscNameMgr::GetInstance();
    errMsg = "";
    ID3D11Device *dev = Window::GetInstance().GetD3DDevice();

    std::string vsSrc, psSrc;
    if(!FileSystem::ReadFile(rM("CarveRenderer", "VertexShader"), vsSrc) ||
       !FileSystem::ReadFile(rM("CarveRenderer", "PixelShader"), psSrc))
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
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,
            Helper::MemOffset(&Vertex::pos), D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0,
            Helper::MemOffset(&Vertex::texCoord), D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "LIGHTCOLOR", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,
            Helper::MemOffset(&Vertex::lightColor), D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "SUNLIGHT", 0, DXGI_FORMAT_R32_FLOAT, 0,
            Helper::MemOffset(&Vertex::sunlight), D3D11_INPUT_PER_VERTEX_DATA, 0 }
    };
    inputLayout_.Initialize(
        inputLayoutDesc, static_cast<int>(Helper::ArraySize(inputLayoutDesc)),
        shader_.GetShaderByteCodeWithInputSignature(),
        shader_.GetShaderByteCodeSizeWithInputSignature());
    if(!inputLayout_)
    {
        Destroy();
        return false;
    }

    raster_ = std::make_unique<RasterState>(D3D11_FILL_SOLID, D3D11_CULL_NONE);

    return true;
}

void CarveRenderer::Destroy(void)
{
    inputLayout_.Destroy();
    shader_.Destroy();
    raster_.reset();
}

bool CarveRenderer::IsAvailable(void) const
{
    return inputLayout_.IsAvailable();
}

void CarveRenderer::Begin(void)
{
    ID3D11DeviceContext *DC = Window::GetInstance().GetD3DDeviceContext();
    shader_.Bind(DC);
    DC->IASetInputLayout(inputLayout_);
    DC->RSSetState(*raster_);
}

void CarveRenderer::End(void)
{
    ID3D11DeviceContext *DC = Window::GetInstance().GetD3DDeviceContext();
    shader_.Unbind(DC);
    DC->IASetInputLayout(nullptr);
    DC->RSSetState(nullptr);
}

CarveRenderer::ShaderType &CarveRenderer::GetShader(void)
{
    return shader_;
}
