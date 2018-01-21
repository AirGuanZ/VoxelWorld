/*================================================================
Filename: Sample_TextureCube
Date: 2017.12.12
Created by AirGuanZ
================================================================*/
#ifndef SAMPLE_TEXTURE_CUBE_H
#define SAMPLE_TEXTURE_CUBE_H

#include <fstream>
#include <iterator>
#include <string>

#include <DDSTextureLoader.h>
#include <OWEShader.hpp>
#include <SimpleMath.h>

#include "SampleApp.h"

namespace Sample_TextureCube
{
    class App : public SampleApp
    {
        struct Vertex
        {
            DirectX::SimpleMath::Vector3 pos;
            DirectX::SimpleMath::Vector2 uv;
        };

        struct VSCB
        {
            DirectX::SimpleMath::Matrix WVP;
        };

        ID3D11Buffer *vtxBuf_ = nullptr;
        ID3D11InputLayout *inputLayout_ = nullptr;

        OWE::Shader<SS_VS, SS_PS> shader_;

        OWE::ConstantBufferManager<SS_VS> *VSCBs_ = nullptr;
        OWE::ShaderResourceManager<SS_PS> *PSSRs_ = nullptr;
        OWE::ShaderSamplerManager<SS_PS> *PSSSs_ = nullptr;

        ID3D11ShaderResourceView *texView_ = nullptr;
        ID3D11Resource *tex_ = nullptr;

        ID3D11SamplerState *sampler_ = nullptr;

        ID3D11RasterizerState *raster_ = nullptr;

        bool InitScene(void)
        {
            HRESULT hr;

            //=============Shader=============

            shader_.InitStage<SS_VS>(D3D_, ReadFile("Data\\Sample_TextureCube\\test.vs"));
            shader_.InitStage<SS_PS>(D3D_, ReadFile("Data\\Sample_TextureCube\\test.ps"));

            VSCBs_ = shader_.CreateConstantBufferManager<SS_VS>();
            PSSRs_ = shader_.CreateShaderResourceManager<SS_PS>();
            PSSSs_ = shader_.CreateShaderSamplerManager<SS_PS>();

            //=============Input Layout=============

            D3D11_INPUT_ELEMENT_DESC inputDesc[] =
            {
                { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, MemOffset(&Vertex::pos), D3D11_INPUT_PER_VERTEX_DATA, 0 },
                { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, MemOffset(&Vertex::uv), D3D11_INPUT_PER_VERTEX_DATA, 0 }
            };

            hr = D3D_->CreateInputLayout(inputDesc, 2, shader_.GetShaderByteCodeWithInputSignature(),
                shader_.GetShaderByteCodeSizeWithInputSignature(), &inputLayout_);
            if(FAILED(hr))
                throw OWE::Error("Failed to create input layout");

            //=============Vertex Buffer=============

            Vertex vtxBufData[] =
            {
                //x+
                { { +0.5f, -0.5f, +0.5f }, { 0.0f, 1.0f } },
                { { +0.5f, +0.5f, -0.5f }, { 1.0f, 0.0f } },
                { { +0.5f, +0.5f, +0.5f }, { 0.0f, 0.0f } },
                { { +0.5f, -0.5f, +0.5f }, { 0.0f, 1.0f } },
                { { +0.5f, -0.5f, -0.5f }, { 1.0f, 1.0f } },
                { { +0.5f, +0.5f, -0.5f }, { 0.0f, 0.0f } },

                //x-
                { { -0.5f, -0.5f, -0.5f }, { 0.0f, 1.0f } },
                { { -0.5f, +0.5f, +0.5f }, { 1.0f, 0.0f } },
                { { -0.5f, +0.5f, -0.5f }, { 0.0f, 0.0f } },
                { { -0.5f, -0.5f, -0.5f }, { 0.0f, 1.0f } },
                { { -0.5f, -0.5f, +0.5f }, { 1.0f, 1.0f } },
                { { -0.5f, +0.5f, +0.5f }, { 1.0f, 0.0f } },

                //y+
                { { -0.5f, +0.5f, +0.5f }, { 0.0f, 1.0f } },
                { { +0.5f, +0.5f, -0.5f }, { 1.0f, 0.0f } },
                { { -0.5f, +0.5f, -0.5f }, { 0.0f, 0.0f } },
                { { -0.5f, +0.5f, +0.5f }, { 0.0f, 1.0f } },
                { { +0.5f, +0.5f, +0.5f }, { 1.0f, 1.0f } },
                { { +0.5f, +0.5f, -0.5f }, { 1.0f, 0.0f } },

                //y-
                { { -0.5f, -0.5f, -0.5f }, { 0.0f, 1.0f } },
                { { +0.5f, -0.5f, +0.5f }, { 1.0f, 0.0f } },
                { { -0.5f, -0.5f, +0.5f }, { 0.0f, 0.0f } },
                { { -0.5f, -0.5f, -0.5f }, { 0.0f, 1.0f } },
                { { +0.5f, -0.5f, -0.5f }, { 1.0f, 1.0f } },
                { { +0.5f, -0.5f, +0.5f }, { 1.0f, 0.0f } },

                //z-
                { { +0.5f, -0.5f, -0.5f }, { 0.0f, 1.0f } },
                { { -0.5f, +0.5f, -0.5f }, { 1.0f, 0.0f } },
                { { +0.5f, +0.5f, -0.5f }, { 0.0f, 0.0f } },
                { { +0.5f, -0.5f, -0.5f }, { 0.0f, 1.0f } },
                { { -0.5f, -0.5f, -0.5f }, { 1.0f, 1.0f } },
                { { -0.5f, +0.5f, -0.5f }, { 1.0f, 0.0f } },

                //z+
                { { -0.5f, -0.5f, +0.5f }, { 0.0f, 1.0f } },
                { { +0.5f, +0.5f, +0.5f }, { 1.0f, 0.0f } },
                { { -0.5f, +0.5f, +0.5f }, { 0.0f, 0.0f } },
                { { -0.5f, -0.5f, +0.5f }, { 0.0f, 1.0f } },
                { { +0.5f, -0.5f, +0.5f }, { 1.0f, 1.0f } },
                { { +0.5f, +0.5f, +0.5f }, { 1.0f, 0.0f } }
            };

            D3D11_BUFFER_DESC vtxBufDesc;
            vtxBufDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
            vtxBufDesc.ByteWidth = sizeof(vtxBufData);
            vtxBufDesc.CPUAccessFlags = 0;
            vtxBufDesc.MiscFlags = 0;
            vtxBufDesc.StructureByteStride = 0;
            vtxBufDesc.Usage = D3D11_USAGE_IMMUTABLE;

            D3D11_SUBRESOURCE_DATA vtxBufDataDesc = { vtxBufData, 0, 0 };
            hr = D3D_->CreateBuffer(&vtxBufDesc, &vtxBufDataDesc, &vtxBuf_);
            if(FAILED(hr))
                return false;

            //=============Texture=============

            if(FAILED(DirectX::CreateDDSTextureFromFile(
                D3D_, L"Data\\Sample_TextureCube\\tex.dds",
                &tex_, &texView_)))
                return false;

            //=============Sampler State=============

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

            //=============Rasterizer State=============

            D3D11_RASTERIZER_DESC rasDesc;
            rasDesc.AntialiasedLineEnable = FALSE;
            rasDesc.CullMode = D3D11_CULL_BACK;
            rasDesc.DepthBias = 0;
            rasDesc.DepthBiasClamp = 0.0f;
            rasDesc.FillMode = D3D11_FILL_SOLID;
            rasDesc.FrontCounterClockwise = TRUE;
            rasDesc.MultisampleEnable = FALSE;
            rasDesc.ScissorEnable = FALSE;
            rasDesc.SlopeScaledDepthBias = 0.0f;

            hr = D3D_->CreateRasterizerState(&rasDesc, &raster_);
            if(FAILED(hr))
                return false;

            return true;
        }

        void DestroyScene(void)
        {
            using namespace OWEShaderAux;
            ReleaseCOMObjects(vtxBuf_, inputLayout_);
            ReleaseCOMObjects(tex_, texView_, sampler_);
            ReleaseCOMObjects(raster_);
            SafeDeleteObjects(VSCBs_, PSSRs_, PSSSs_);
            shader_.Destroy();
        }

    public:
        void Run(void)
        {
            if(!InitD3DContext() || !InitScene())
            {
                DestroyScene();
                DestroyD3DContext();
                throw OWE::Error("Failed to initialize render context");
            }

            float horRad = 0.0f;
            while(!(GetKeyState(VK_ESCAPE) & 0x8000))
            {
                using DirectX::SimpleMath::Matrix;

                float backgroundColor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
                DC_->ClearRenderTargetView(renderTargetView_, backgroundColor);
                DC_->ClearDepthStencilView(depthStencilView_, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

                Matrix matRot = Matrix::CreateRotationY(horRad += 0.01f);
                Matrix view = Matrix::CreateLookAt(
                { 0.0f, 4.0f, 6.0f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 1.0f, 0.0f });
                Matrix proj = Matrix::CreatePerspectiveFieldOfView(
                    3.14159f * 45 / 180, 1.0f, 0.1f, 100.0f);
                VSCB VSCBData = { (matRot * view * proj).Transpose() };

                VSCBs_->GetConstantBuffer<VSCB>(D3D_, "Trans")->SetBufferData(DC_, VSCBData);
                PSSRs_->GetShaderResourceObject("tex")->SetShaderResource(texView_);
                PSSSs_->GetShaderSamplerObject("sam")->SetSampler(sampler_);

                UINT vtxStride = sizeof(Vertex), offset = 0;
                DC_->IASetInputLayout(inputLayout_);
                DC_->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
                DC_->IASetVertexBuffers(0, 1, &vtxBuf_, &vtxStride, &offset);

                shader_.Bind(DC_);
                VSCBs_->Bind(DC_); PSSRs_->Bind(DC_); PSSSs_->Bind(DC_);

                DC_->RSSetState(raster_);
                DC_->Draw(36, 0);

                VSCBs_->Unbind(DC_); PSSRs_->Unbind(DC_); PSSSs_->Unbind(DC_);
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

#endif //SAMPLE_TEXTURE_CUBE_H
