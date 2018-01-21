/*================================================================
Filename: Sample_GeometryShader.h
Date: 2017.12.25
Created by AirGuanZ
================================================================*/
#ifndef SAMPLE_GEOMETRYShader_H
#define SAMPLE_GEOMETRYShader_H

#include <DDSTextureLoader.h>
#include <SimpleMath.h>

#include <OWEShader.hpp>
#include "SampleApp.h"

namespace Sample_GeometryShader
{
    using namespace OWE;
    using namespace DirectX::SimpleMath;

    class App : public SampleApp
    {
        struct Vertex
        {
            Vector3 pos;   //POSITION
            float size;    //SIZE
        };

        struct GSCB_Trans
        {
            Matrix VP;
        };

        ID3D11Buffer *vtxBuf_ = nullptr;
        ID3D11InputLayout *inputLayout_ = nullptr;

        Shader<SS_VS, SS_GS, SS_PS> shader_;
        ShaderUniforms<SS_VS, SS_GS, SS_PS> *uniforms_ = nullptr;

        int vtxCount_ = 0;
        
        void InitScene(void)
        {
            HRESULT hr;

            //============ Shader ============

            shader_.InitStage<SS_VS>(D3D_, ReadFile("Data\\Sample_GeometryShader\\vs.hlsl"));
            shader_.InitStage<SS_GS>(D3D_, ReadFile("Data\\Sample_GeometryShader\\gs.hlsl"));
            shader_.InitStage<SS_PS>(D3D_, ReadFile("Data\\Sample_GeometryShader\\ps.hlsl"));

            uniforms_ = shader_.CreateUniformManager();

            //============ Input Layout ============

            D3D11_INPUT_ELEMENT_DESC inputLayoutDesc[] =
            {
                { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, MemOffset(&Vertex::pos), D3D11_INPUT_PER_VERTEX_DATA, 0 },
                { "SIZE", 0, DXGI_FORMAT_R32_FLOAT, 0, MemOffset(&Vertex::size), D3D11_INPUT_PER_VERTEX_DATA, 0 }
            };

            hr = D3D_->CreateInputLayout(
                inputLayoutDesc, 2,
                shader_.GetShaderByteCodeWithInputSignature(),
                shader_.GetShaderByteCodeSizeWithInputSignature(),
                &inputLayout_);
            if(FAILED(hr))
                throw Error("Failed to create input layout");

            //============ Vertex Buffer ============

            constexpr int GRID_COUNT = 20;
            constexpr float GRID_SIZE = 1.0f;

            vtxCount_ = GRID_COUNT * GRID_COUNT;
            std::vector<Vertex> vtxData(vtxCount_);
            int zBase = 0; float zPos = 0.0f;
            for(int z = 0; z != GRID_COUNT; ++z)
            {
                float xPos = 0.0f;
                for(int x = 0; x != GRID_COUNT; ++x)
                {
                    Vertex &v = vtxData[zBase + x];
                    v.pos = Vector3(xPos, 0.0f, zPos);
                    v.size = 1.0f / ((GRID_COUNT - 1) * (GRID_COUNT - 1)) *
                            ((GRID_COUNT - 1 - x) * (GRID_COUNT - 1 - z)) + 0.2f;
                    xPos += GRID_SIZE;
                }
                zBase += GRID_COUNT;
                zPos += GRID_SIZE;
            }

            D3D11_BUFFER_DESC vtxBufDesc;
            vtxBufDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
            vtxBufDesc.ByteWidth = sizeof(Vertex) * vtxData.size();
            vtxBufDesc.CPUAccessFlags = 0;
            vtxBufDesc.MiscFlags = 0;
            vtxBufDesc.StructureByteStride = 0;
            vtxBufDesc.Usage = D3D11_USAGE_IMMUTABLE;

            D3D11_SUBRESOURCE_DATA vtxInitData = { vtxData.data(), 0, 0 };
            hr = D3D_->CreateBuffer(&vtxBufDesc, &vtxInitData, &vtxBuf_);
            if(FAILED(hr))
                throw Error("Failed to create vertex buffer");
        }

        void DestroyScene(void)
        {
            using namespace OWEShaderAux;

            ReleaseCOMObjects(vtxBuf_, inputLayout_);
            SafeDeleteObjects(uniforms_);
            shader_.Destroy();
        }

    public:
        void Run(void)
        {
            try
            {
                if(!InitD3DContext())
                    throw Error("Failed to create D3D context");
                InitScene();
            }
            catch(const Error &err)
            {
                DestroyScene();
                DestroyD3DContext();
                throw err;
            }

            Matrix VP = Matrix::CreateLookAt({ 30.0f, 10.0f, 30.0f },
                                             { 5.0f, 0.0f, 5.0f },
                                             { 0.0f, 1.0f, 0.0f }) *
                        Matrix::CreatePerspectiveFieldOfView(
                                    3.14159f * 45 / 180, 1.0f, 0.1f, 1000.0f);
            GSCB_Trans trans = { VP.Transpose() };
            uniforms_->GetConstantBuffer<SS_GS, GSCB_Trans>(D3D_, "Trans")->SetBufferData(DC_, trans);

            while(!(GetKeyState(VK_ESCAPE) & 0x8000))
            {
                float backgroundColor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
                DC_->ClearRenderTargetView(renderTargetView_, backgroundColor);
                DC_->ClearDepthStencilView(depthStencilView_, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

                UINT vtxStride = sizeof(Vertex), vtxOffset = 0;
                DC_->IASetInputLayout(inputLayout_);
                DC_->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
                DC_->IASetVertexBuffers(0, 1, &vtxBuf_, &vtxStride, &vtxOffset);
                
                shader_.Bind(DC_);
                uniforms_->Bind(DC_);

                DC_->Draw(vtxCount_, 0);

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

#endif //SAMPLE_GEOMETRYShader_H
