/*================================================================
Filename: Sample_HeightMap.h
Date: 2017.12.19
Created by AirGuanZ
================================================================*/
#ifndef SAMPLE_HEIGHT_MAP_H
#define SAMPLE_HEIGHT_MAP_H

#include <fstream>
#include <string>

#include <d3d11.h>
#include <DDSTextureLoader.h>
#include <SimpleMath.h>
#include <OWEShader.hpp>

#include "SampleApp.h"

namespace Sample_HeightMap
{
    using namespace DirectX::SimpleMath;
    using namespace OWE;

    class App : public SampleApp
    {
        struct Vertex
        {
            Vector2 pos;
            Vector2 uv;
        };

        struct VSCB
        {
            Matrix WVP;
        };

        ID3D11Buffer *vtxBuf_ = nullptr;
        ID3D11Buffer *idxBuf_ = nullptr;
        ID3D11InputLayout *inputLayout_ = nullptr;

        Shader<SS_VS, SS_PS> shader_;
        ShaderUniforms<SS_VS, SS_PS> *uniforms_ = nullptr;

        ID3D11Resource *heightMap_ = nullptr;
        ID3D11ShaderResourceView *heightMapView_ = nullptr;

        ID3D11Resource *tex_ = nullptr;
        ID3D11ShaderResourceView *texView_ = nullptr;

        ID3D11SamplerState *sampler_ = nullptr;
        ID3D11RasterizerState *raster_ = nullptr;
        
        int vtxCount_ = 0;

    public:

        void InitScene(void)
        {
            HRESULT hr;

            //============= Shader =============
            
            shader_.InitStage<SS_VS>(D3D_, ReadFile("Data\\Sample_HeightMap\\test.vs"));
            shader_.InitStage<SS_PS>(D3D_, ReadFile("Data\\Sample_HeightMap\\test.ps"));

            uniforms_ = shader_.CreateUniformManager();

            //============= Input Layout =============

            D3D11_INPUT_ELEMENT_DESC inputDesc[] =
            {
                { "POSITION", 0, DXGI_FORMAT_R32G32_FLOAT, 0, MemOffset(&Vertex::pos), D3D11_INPUT_PER_VERTEX_DATA, 0 },
                { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, MemOffset(&Vertex::uv), D3D11_INPUT_PER_VERTEX_DATA, 0 }
            };

            hr = D3D_->CreateInputLayout(inputDesc, 2,
                shader_.GetShaderByteCodeWithInputSignature(),
                shader_.GetShaderByteCodeSizeWithInputSignature(),
                &inputLayout_);
            if(FAILED(hr))
                throw OWE::Error("Failed to create input layout");

            //============= Vertex & Index Buffer =============

            constexpr float VERTEX_GRID_SIZE = 1.0f;
            constexpr int VERTEX_GRID_COUNT = 40;

            std::vector<Vertex> vtxBufData(VERTEX_GRID_COUNT * VERTEX_GRID_COUNT);
            for(int yIdx = 0; yIdx != VERTEX_GRID_COUNT; ++yIdx)
            {
                for(int xIdx = 0; xIdx != VERTEX_GRID_COUNT; ++xIdx)
                {
                    vtxBufData[yIdx * VERTEX_GRID_COUNT + xIdx].pos =
                        Vector2(xIdx * VERTEX_GRID_SIZE, yIdx * VERTEX_GRID_SIZE);
                    vtxBufData[yIdx * VERTEX_GRID_COUNT + xIdx].uv =
                        Vector2(xIdx / (VERTEX_GRID_COUNT - 1.0f), yIdx / (VERTEX_GRID_COUNT - 1.0f));
                }
            }

            std::vector<uint16_t> idxBufData(6 * (VERTEX_GRID_COUNT - 1) * (VERTEX_GRID_COUNT - 1));
            int idxBufIdx = 0;
            for(int yIdx = 0; yIdx != VERTEX_GRID_COUNT - 1; ++yIdx)
            {
                for(int xIdx = 0; xIdx != VERTEX_GRID_COUNT - 1; ++xIdx)
                {
                    idxBufData[idxBufIdx++] = yIdx * VERTEX_GRID_COUNT + xIdx;
                    idxBufData[idxBufIdx++] = (yIdx + 1) * VERTEX_GRID_COUNT + xIdx;
                    idxBufData[idxBufIdx++] = (yIdx + 1) * VERTEX_GRID_COUNT + xIdx + 1;
                    idxBufData[idxBufIdx++] = yIdx * VERTEX_GRID_COUNT + xIdx;
                    idxBufData[idxBufIdx++] = (yIdx + 1) * VERTEX_GRID_COUNT + xIdx + 1;
                    idxBufData[idxBufIdx++] = yIdx * VERTEX_GRID_COUNT + xIdx + 1;
                }
            }
            vtxCount_ = idxBufData.size();

            D3D11_BUFFER_DESC vtxBufDesc;
            vtxBufDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
            vtxBufDesc.ByteWidth = vtxBufData.size() * sizeof(Vertex);
            vtxBufDesc.CPUAccessFlags = 0;
            vtxBufDesc.MiscFlags = 0;
            vtxBufDesc.StructureByteStride = 0;
            vtxBufDesc.Usage = D3D11_USAGE_IMMUTABLE;
            D3D11_SUBRESOURCE_DATA vtxBufInitData = { vtxBufData.data(), 0, 0 };
            hr = D3D_->CreateBuffer(&vtxBufDesc, &vtxBufInitData, &vtxBuf_);
            if(FAILED(hr))
                throw Error("Failed to create vertex buffer");

            D3D11_BUFFER_DESC idxBufDesc;
            idxBufDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
            idxBufDesc.ByteWidth = idxBufData.size() * sizeof(decltype(idxBufData)::value_type);
            idxBufDesc.CPUAccessFlags = 0;
            idxBufDesc.MiscFlags = 0;
            idxBufDesc.StructureByteStride = 0;
            idxBufDesc.Usage = D3D11_USAGE_IMMUTABLE;
            D3D11_SUBRESOURCE_DATA idxBufInitData = { idxBufData.data(), 0, 0 };
            hr = D3D_->CreateBuffer(&idxBufDesc, &idxBufInitData, &idxBuf_);
            if(FAILED(hr))
                throw Error("Failed to create index buffer");

            //============= Textures =============

            if(FAILED(DirectX::CreateDDSTextureFromFile(
                    D3D_, L"Data\\Sample_HeightMap\\tex.dds", &tex_, &texView_)) ||
               FAILED(DirectX::CreateDDSTextureFromFile(
                    D3D_, L"Data\\Sample_HeightMap\\heightMap.dds", &heightMap_, &heightMapView_)))
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

            //============= Rasterizer State =============

            D3D11_RASTERIZER_DESC rasDesc;
            rasDesc.AntialiasedLineEnable = FALSE;
            rasDesc.CullMode = D3D11_CULL_BACK;
            rasDesc.DepthBias = 0;
            rasDesc.DepthBiasClamp = 0.0f;
            rasDesc.FillMode = D3D11_FILL_WIREFRAME;
            rasDesc.FrontCounterClockwise = TRUE;
            rasDesc.MultisampleEnable = FALSE;
            rasDesc.ScissorEnable = FALSE;
            rasDesc.SlopeScaledDepthBias = 0.0f;

            hr = D3D_->CreateRasterizerState(&rasDesc, &raster_);
            if(FAILED(hr))
                throw Error("Failed to create rasterizer state");
        }

        void DestroyScene(void)
        {
            using namespace OWEShaderAux;

            ReleaseCOMObjects(vtxBuf_, idxBuf_, inputLayout_);
            ReleaseCOMObjects(heightMap_, heightMapView_, tex_, texView_);
            ReleaseCOMObjects(sampler_, raster_);
            SafeDeleteObjects(uniforms_);
            shader_.Destroy();
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

            uniforms_->GetShaderResource<SS_VS>("heightMap")->SetShaderResource(heightMapView_);
            uniforms_->GetShaderSampler<SS_VS>("sam")->SetSampler(sampler_);

            uniforms_->GetShaderResource<SS_PS>("tex")->SetShaderResource(texView_);
            uniforms_->GetShaderSampler<SS_PS>("sam")->SetSampler(sampler_);

            Matrix world = Matrix::Identity;
            Matrix view = Matrix::CreateLookAt({ -10.0f, 20.0f, -10.0f },
                                               { 20.0f, 0.0f, 20.0f },
                                               { 0.0f, 1.0f, 0.0f });
            Matrix proj = Matrix::CreatePerspectiveFieldOfView(
                                    3.14159f * 45 / 180, 1.0f, 0.1f, 1000.0f);
            VSCB VSCBData = { (world * view * proj).Transpose() };
            uniforms_->GetConstantBuffer<SS_VS, VSCB>(D3D_, "Trans")->SetBufferData(DC_, VSCBData);
            
            while(!(GetKeyState(VK_ESCAPE) & 0x8000))
            {
                float backgroundColor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
                DC_->ClearRenderTargetView(renderTargetView_, backgroundColor);
                DC_->ClearDepthStencilView(depthStencilView_, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

                UINT vtxStride = sizeof(Vertex), vtxOffset = 0;
                DC_->IASetInputLayout(inputLayout_);
                DC_->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
                DC_->IASetVertexBuffers(0, 1, &vtxBuf_, &vtxStride, &vtxOffset);
                DC_->IASetIndexBuffer(idxBuf_, DXGI_FORMAT_R16_UINT, 0);

                shader_.Bind(DC_);
                uniforms_->Bind(DC_);
                
                DC_->RSSetState(raster_);
                DC_->DrawIndexed(vtxCount_, 0, 0);
                
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

#endif //SAMPLE_HEIGHT_MAP_H
