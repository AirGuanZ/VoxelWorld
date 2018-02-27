/*================================================================
Filename: ImmediateScreen2D.cpp
Date: 2018.1.23
Created by AirGuanZ
================================================================*/
#include <cassert>
#include <string>

#include <Utility/HelperFunctions.h>

#include <Resource/ResourceName.h>
#include <Window/Window.h>
#include "ImmediateScreen2D.h"

ImmediateScreen2D::ImmediateScreen2D(void)
    : norDepthStencilState_(D3DObj_Noinit()),
      norSampler_(D3DObj_Noinit())
{

}

bool ImmediateScreen2D::Initialize(std::string &errMsg)
{
    assert(!IsAvailable() && Window::GetInstance().IsD3DAvailable());
    errMsg = "";

    std::string vsSrc, psSrc;
    if(!Helper::ReadFile(IMMEDIATE_SCREEN_2D_NOR_VERTEX_SHADER, vsSrc) ||
       !Helper::ReadFile(IMMEDIATE_SCREEN_2D_NOR_PIXEL_SHADER, psSrc))
    {
        errMsg = "Failed to load source file for ImmediateScreen2D";
        return false;
    }

    ID3D11Device *dev = Window::GetInstance().GetD3DDevice();
    if(!norShader_.InitStage<SS_VS>(dev, vsSrc, &errMsg) ||
       !norShader_.InitStage<SS_PS>(dev, psSrc, &errMsg))
    {
        Destroy();
        return false;
    }

    norUniforms_.reset(norShader_.CreateUniformManager());

    QuadVertex quadVtxData[] =
    {
        { { 0.0f, 0.0f } },
        { { 0.0f, 1.0f } },
        { { 1.0f, 1.0f } },
        { { 0.0f, 0.0f } },
        { { 1.0f, 1.0f } },
        { { 1.0f, 0.0f } }
    };
    if(!quadVtxBuf_.Initialize(quadVtxData, sizeof(QuadVertex) *
                                            Helper::ArraySize(quadVtxData)))
    {
        Destroy();
        return false;
    }

    D3D11_INPUT_ELEMENT_DESC inputDesc[] =
    {
        { "POSITION", 0, DXGI_FORMAT_R32G32_FLOAT, 0,
          Helper::MemOffset(&QuadVertex::pos), D3D11_INPUT_PER_VERTEX_DATA, 0 }
    };
    if(!(quadVtxNorLayout_.Initialize(
            inputDesc, static_cast<int>(Helper::ArraySize(inputDesc)),
            norShader_.GetShaderByteCodeWithInputSignature(),
            norShader_.GetShaderByteCodeSizeWithInputSignature())))
    {
        Destroy();
        return false;
    }

    norSampler_ = Sampler();
    norDepthStencilState_ = DepthStencilState(false, false);

    return true;
}

void ImmediateScreen2D::Destroy(void)
{
    norShader_.Destroy();
    norUniforms_.reset();
    quadVtxBuf_.Destroy();
}

bool ImmediateScreen2D::IsAvailable(void) const
{
    return norShader_.IsAllStagesAvailable();
}

void ImmediateScreen2D::DrawRectangle(const Vector2 &LB, const Vector2 &RT, Texture2D tex)
{
    ID3D11Device *dev = Window::GetInstance().GetD3DDevice();
    ID3D11DeviceContext *DC = Window::GetInstance().GetD3DDeviceContext();

    ID3D11Buffer *vtxBuf = quadVtxBuf_;
    UINT stride = sizeof(QuadVertex), offset = 0;

    DC->IASetInputLayout(quadVtxNorLayout_);
    DC->IASetVertexBuffers(0, 1, &vtxBuf, &stride, &offset);
    DC->OMSetDepthStencilState(norDepthStencilState_, 0);

    struct VSTrans
    {
        Vector2 LB;
        Vector2 WH;
    };
    norUniforms_->GetConstantBuffer<SS_VS, VSTrans>(dev, "Trans")
        ->SetBufferData(DC, { LB, RT - LB });
    norUniforms_->GetShaderResource<SS_PS>("tex")->SetShaderResource(tex);
    norUniforms_->GetShaderSampler<SS_PS>("sam")->SetSampler(norSampler_);

    norShader_.Bind(DC);
    norUniforms_->Bind(DC);

    DC->Draw(6, 0);

    norUniforms_->Unbind(DC);
    norShader_.Unbind(DC);

    vtxBuf = nullptr; stride = 0;
    DC->IASetInputLayout(nullptr);
    DC->IASetVertexBuffers(0, 1, &vtxBuf, &stride, &offset);
    DC->OMSetDepthStencilState(nullptr, 0);
}
