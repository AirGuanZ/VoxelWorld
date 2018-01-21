/*================================================================
Filename: Sample_MixTex.h
Date: 2017.12.11
Created by AirGuanZ
================================================================*/
#ifndef SAMPLE_MIX_TEX_H
#define SAMPLE_MIX_TEX_H

#include <fstream>
#include <string>

#include <DDSTextureLoader.h>
#include <SimpleMath.h>
#include <OWEShader.hpp>

#include "SampleApp.h"

namespace Sample_MixTex
{
    class App : public SampleApp
    {
        ID3D11InputLayout *inputLayout_ = nullptr;
        ID3D11Buffer *vtxBuf_ = nullptr;

        OWE::Shader<SS_VS, SS_PS> shader_;

        ID3D11Resource *tex1_ = nullptr;
        ID3D11Resource *tex2_ = nullptr;

        ID3D11ShaderResourceView *tex1View_ = nullptr;
        ID3D11ShaderResourceView *tex2View_ = nullptr;

        ID3D11SamplerState *sampler_ = nullptr;

        OWE::ShaderResourceManager<SS_PS> *PSSRs_ = nullptr;
        OWE::ShaderSamplerManager<SS_PS> *PSSSs_ = nullptr;

        struct Vertex
        {
            DirectX::SimpleMath::Vector2 pos;
            DirectX::SimpleMath::Vector2 uv;
        };

        bool InitScene(void)
        {
            HRESULT hr;

            //=============着色器编译=============

            try
            {
                shader_.InitStage<SS_VS>(D3D_, ReadFile("Data\\Sample_MixTex\\test.vs"));
                shader_.InitStage<SS_PS>(D3D_, ReadFile("Data\\Sample_MixTex\\test.ps"));
            }
            catch(const OWE::Error &err)
            {
                std::cout << err.what() << std::endl;
            }

            PSSRs_ = shader_.CreateShaderResourceManager<SS_PS>();
            PSSSs_ = shader_.CreateShaderSamplerManager<SS_PS>();

            //=============顶点缓存初始化=============

            Vertex vtxBufData[] =
            {
                { { -1.0f, -1.0f }, { 0.0f, 1.0f } },
                { { -1.0f, +1.0f }, { 0.0f, 0.0f } },
                { { +1.0f, +1.0f }, { 1.0f, 0.0f } },
                { { -1.0f, -1.0f }, { 0.0f, 1.0f } },
                { { +1.0f, +1.0f }, { 1.0f, 0.0f } },
                { { +1.0f, -1.0f }, { 1.0f, 1.0f } }
            };

            D3D11_BUFFER_DESC vtxBufDesc;
            vtxBufDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
            vtxBufDesc.ByteWidth = sizeof(vtxBufData);
            vtxBufDesc.CPUAccessFlags = 0;
            vtxBufDesc.MiscFlags = 0;
            vtxBufDesc.StructureByteStride = 0;
            vtxBufDesc.Usage = D3D11_USAGE_IMMUTABLE;

            D3D11_SUBRESOURCE_DATA vtxDataDesc = { vtxBufData, 0, 0 };

            hr = D3D_->CreateBuffer(&vtxBufDesc, &vtxDataDesc, &vtxBuf_);
            if(FAILED(hr))
                return false;

            //=============InputLayout=============

            D3D11_INPUT_ELEMENT_DESC inputDesc[] =
            {
                { "POSITION", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
                { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 8, D3D11_INPUT_PER_VERTEX_DATA, 0 }
            };
            hr = D3D_->CreateInputLayout(
                inputDesc, 2,
                shader_.GetStage<SS_VS>()->GetShaderByteCode(),
                shader_.GetStage<SS_VS>()->GetShaderByteCodeSize(),
                &inputLayout_);
            if(FAILED(hr))
                return false;

            //=============Textures=============

            if(FAILED(DirectX::CreateDDSTextureFromFile(
                D3D_, L"Data\\Sample_MixTex\\tex1.dds", &tex1_, &tex1View_)) ||
                FAILED(DirectX::CreateDDSTextureFromFile(
                    D3D_, L"Data\\Sample_MixTex\\tex2.dds", &tex2_, &tex2View_)))
                return false;

            //=============Sampler state=============

            D3D11_SAMPLER_DESC samplerDesc;
            samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
            samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
            samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
            samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
            samplerDesc.MipLODBias = 0.0f;
            samplerDesc.MaxAnisotropy = 1;
            samplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
            samplerDesc.MinLOD = -FLT_MAX;
            samplerDesc.MaxLOD = FLT_MAX;
            ZeroMemory(samplerDesc.BorderColor, 4 * sizeof(FLOAT));

            hr = D3D_->CreateSamplerState(&samplerDesc, &sampler_);
            if(FAILED(hr))
                return false;

            return true;
        }

        void DestroyScene(void)
        {
            using namespace OWEShaderAux;

            ReleaseCOMObjects(inputLayout_, vtxBuf_);
            ReleaseCOMObjects(tex1_, tex2_);
            ReleaseCOMObjects(tex1View_, tex2View_);
            shader_.Destroy();
            SafeDeleteObjects(PSSRs_, PSSSs_);
        }

    public:
        void Run(void)
        {
            if(!InitD3DContext() || !InitScene())
            {
                DestroyScene();
                DestroyD3DContext();
                std::cout << "Failed to initialize render context" << std::endl;
                return;
            }

            while(!(GetKeyState(VK_ESCAPE) & 0x8000))
            {
                float backgroundColor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
                DC_->ClearRenderTargetView(renderTargetView_, backgroundColor);
                DC_->ClearDepthStencilView(depthStencilView_, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

                PSSRs_->GetShaderResourceObject("tex1")->SetShaderResource(tex1View_);
                PSSRs_->GetShaderResourceObject("tex2")->SetShaderResource(tex2View_);
                PSSSs_->GetShaderSamplerObject("sam")->SetSampler(sampler_);

                UINT vtxStride = sizeof(Vertex), offset = 0;
                DC_->IASetInputLayout(inputLayout_);
                DC_->IASetVertexBuffers(0, 1, &vtxBuf_, &vtxStride, &offset);

                shader_.Bind(DC_);
                PSSRs_->Bind(DC_);
                PSSSs_->Bind(DC_);

                DC_->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
                DC_->Draw(6, 0);

                PSSRs_->Unbind(DC_);
                PSSSs_->Unbind(DC_);
                shader_.Unbind(DC_);

                swapChain_->Present(1, 0);

                MSG msg;
                while(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
                {
                    TranslateMessage(&msg);
                    DispatchMessage(&msg);
                }
            }

            DestroyScene();
            DestroyD3DContext();
        }
    };
}

#endif //SAMPLE_MIX_TEX_H
