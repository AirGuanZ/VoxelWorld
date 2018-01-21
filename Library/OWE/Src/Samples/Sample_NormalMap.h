/*================================================================
Filename: Sample_NormalMap.h
Date: 2017.12.22
Created by AirGuanZ
================================================================*/
#ifndef SAMPLE_NORMAL_MAP_H
#define SAMPLE_NORMAL_MAP_H

#include <DDSTextureLoader.h>
#include <SimpleMath.h>

#include <OWEShader.hpp>
#include "SampleApp.h"

namespace Sample_NormalMap
{
    using namespace OWE;
    using namespace DirectX::SimpleMath;

    class App : public SampleApp
    {
        struct Vertex
        {
            Vector3 pos;   //POSITION
            Vector2 uv;    //TEXCOORD
            Vector3 axisU; //AXIS_U
            Vector3 axisV; //AXIS_V
        };

        struct VSCB_Trans
        {
            Matrix world;
            Matrix WVP;
        };

        struct PSCB_DirectionalLight
        {
            alignas(16) Vector3 pos;
            alignas(16) Vector3 color;
        };

        Shader<SS_VS, SS_PS> shader_;
        ShaderUniforms<SS_VS, SS_PS> *uniforms_;

        ID3D11Resource *normalMap_ = nullptr;
        ID3D11ShaderResourceView *normalMapView_ = nullptr;

        ID3D11Resource *tex_ = nullptr;
        ID3D11ShaderResourceView *texView_ = nullptr;

        ID3D11Buffer *vtxBuf_ = nullptr;
        ID3D11InputLayout *inputLayout_ = nullptr;

        ID3D11SamplerState *sampler_;

        void InitScene(void)
        {
            HRESULT hr;

            //============ Shader ============

            shader_.InitStage<SS_VS>(D3D_, ReadFile("Data\\Sample_NormalMap\\test_vs.hlsl"));
            shader_.InitStage<SS_PS>(D3D_, ReadFile("Data\\Sample_NormalMap\\test_ps.hlsl"));

            uniforms_ = shader_.CreateUniformManager();

            //============ Input Layout ============

            D3D11_INPUT_ELEMENT_DESC inputLayoutDesc[] =
            {
                { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,
                    MemOffset(&Vertex::pos), D3D11_INPUT_PER_VERTEX_DATA, 0 },
                { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0,
                    MemOffset(&Vertex::uv), D3D11_INPUT_PER_VERTEX_DATA, 0 },
                { "AXIS_U", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,
                    MemOffset(&Vertex::axisU), D3D11_INPUT_PER_VERTEX_DATA, 0 },
                { "AXIS_V", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,
                    MemOffset(&Vertex::axisV), D3D11_INPUT_PER_VERTEX_DATA, 0 }
            };

            hr = D3D_->CreateInputLayout(
                inputLayoutDesc, 4,
                shader_.GetShaderByteCodeWithInputSignature(),
                shader_.GetShaderByteCodeSizeWithInputSignature(),
                &inputLayout_);
            if(FAILED(hr))
                throw Error("Failed to create input layout");

            //============ Vertex Buffer ============

            Vertex vtxBufData[] =
            {
                { { -0.5f, -0.5f, 0.0f }, { 0.0f, 1.0f }, { 1.0f, 0.0f, 0.0f }, { 0.0f, -1.0f, 0.0f } },
                { { -0.5f,  0.5f, 0.0f }, { 0.0f, 0.0f }, { 1.0f, 0.0f, 0.0f }, { 0.0f, -1.0f, 0.0f } },
                { {  0.5f,  0.5f, 0.0f }, { 1.0f, 0.0f }, { 1.0f, 0.0f, 0.0f }, { 0.0f, -1.0f, 0.0f } },
                { { -0.5f, -0.5f, 0.0f }, { 0.0f, 1.0f }, { 1.0f, 0.0f, 0.0f }, { 0.0f, -1.0f, 0.0f } },
                { {  0.5f,  0.5f, 0.0f }, { 1.0f, 0.0f }, { 1.0f, 0.0f, 0.0f }, { 0.0f, -1.0f, 0.0f } },
                { {  0.5f, -0.5f, 0.0f }, { 1.0f, 1.0f }, { 1.0f, 0.0f, 0.0f }, { 0.0f, -1.0f, 0.0f } }
            };

            D3D11_BUFFER_DESC vtxBufDesc;
            vtxBufDesc.ByteWidth = sizeof(vtxBufData);
            vtxBufDesc.Usage = D3D11_USAGE_IMMUTABLE;
            vtxBufDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
            vtxBufDesc.CPUAccessFlags = 0;
            vtxBufDesc.MiscFlags = 0;
            vtxBufDesc.StructureByteStride = 0;

            D3D11_SUBRESOURCE_DATA vtxBufInitData = { vtxBufData, 0, 0 };
            hr = D3D_->CreateBuffer(
                &vtxBufDesc, &vtxBufInitData, &vtxBuf_);
            if(FAILED(hr))
                throw Error("Failed to create vertex buffer");

            //============= Textures =============

            if(FAILED(DirectX::CreateDDSTextureFromFile(
                    D3D_, L"Data\\Sample_NormalMap\\tex.dds", &tex_, &texView_)) ||
               FAILED(DirectX::CreateDDSTextureFromFile(
                    D3D_, L"Data\\Sample_NormalMap\\normalMap.dds", &normalMap_, &normalMapView_)))
                throw Error("Failed to load textures from file");

            //============= Sampler State =============

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
                throw Error("Failed to create sampler state");
        }

        void DestroyScene(void)
        {
            using namespace OWEShaderAux;

            ReleaseCOMObjects(vtxBuf_, inputLayout_);
            ReleaseCOMObjects(normalMap_, normalMapView_);
            ReleaseCOMObjects(tex_, texView_);
            ReleaseCOMObjects(sampler_);
            shader_.Destroy();
            SafeDeleteObjects(uniforms_);
        }

    public:
        void Run(void)
        {
            try
            {
                if(!InitD3DContext())
                    throw Error("Failed to initialize D3D context");
                InitScene();
            }
            catch(const Error &err)
            {
                DestroyScene();
                DestroyD3DContext();
                throw err;
            }

            Matrix world = Matrix::Identity;
            Matrix view = Matrix::CreateLookAt(
                { 0.0f, 0.0f, 2.0f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 1.0f, 0.0f });
            Matrix proj = Matrix::CreatePerspectiveFieldOfView(
                3.14159f * 45 / 180, 1.0f, 0.1f, 1000.0f);
            Matrix WVP = world * view * proj;
            VSCB_Trans trans =
            {
                world.Transpose(),
                WVP.Transpose()
            };

            uniforms_->GetConstantBuffer<SS_VS, VSCB_Trans>(D3D_, "Trans")->SetBufferData(DC_, trans);

            uniforms_->GetShaderResource<SS_PS>("normalMap")->SetShaderResource(normalMapView_);
            uniforms_->GetShaderResource<SS_PS>("tex")->SetShaderResource(texView_);

            uniforms_->GetShaderSampler<SS_PS>("sam")->SetSampler(sampler_);

            float lightT = 0.0f;

            while(!(GetKeyState(VK_ESCAPE) & 0x8000))
            {
                float backgroundColor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
                DC_->ClearRenderTargetView(renderTargetView_, backgroundColor);
                DC_->ClearDepthStencilView(depthStencilView_, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

                lightT += 0.012f;
                Vector3 lightPos(0.5f * std::sin(lightT), 0.5f * std::cos(1.347f * lightT), 0.4f);
                PSCB_DirectionalLight light =
                {
                    lightPos, Vector3(1.0f, 1.0f, 1.0f)
                };
                uniforms_->GetConstantBuffer<SS_PS, PSCB_DirectionalLight>(D3D_, "DirectionalLight")->SetBufferData(DC_, light);

                UINT vtxStride = sizeof(Vertex), vtxOffset = 0;
                DC_->IASetInputLayout(inputLayout_);
                DC_->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
                DC_->IASetVertexBuffers(0, 1, &vtxBuf_, &vtxStride, &vtxOffset);

                shader_.Bind(DC_);
                uniforms_->Bind(DC_);

                DC_->Draw(6, 0);

                uniforms_->Unbind(DC_);
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

#endif //SAMPLE_NORMAL_MAP_H
