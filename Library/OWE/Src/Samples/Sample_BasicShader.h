/*================================================================
Filename: Sample_BasicShader.h
Date: 2017.12.10
Created by AirGuanZ
================================================================*/
#ifndef SAMPLE_BASICShader_H
#define SAMPLE_BASICShader_H

#include <iostream>
#include <d3d11.h>
#include <DirectXMath.h>

#include <OWEShader.hpp>

#include "SampleApp.h"

namespace Sample_BasicShader
{
    class App : public SampleApp
    {
        ID3D11InputLayout *inputLayout_ = nullptr;
        ID3D11Buffer *vtxBuf_ = nullptr;

        OWE::Shader<SS_VS, SS_PS> shader_;

        struct VSCBSize
        {
            DirectX::XMFLOAT2 size;
            float pad0;
            float pad1;
        };

        OWE::ConstantBufferManager<SS_VS> *VSCBs_ = nullptr;

        bool InitScene(void)
        {
            HRESULT hr;

            //=============着色器编译=============

            ID3D10Blob *shaderByteCode = nullptr;
            ID3D10Blob *shaderErr = nullptr;
            hr = D3DCompileFromFile(
                L"Data\\Sample_BasicShader\\test.vs",
                nullptr, nullptr, "main",
                "vs_5_0", 0, 0, &shaderByteCode, &shaderErr);
            if(FAILED(hr))
            {
                if(shaderErr)
                {
                    std::cout << shaderErr->GetBufferPointer() << std::endl;
                    shaderErr->Release();
                }
                return false;
            }
            else if(shaderErr)
                shaderErr->Release();
            shader_.InitStage<SS_VS>(D3D_, shaderByteCode);
            shaderByteCode->Release();

            shaderErr = nullptr;
            hr = D3DCompileFromFile(
                L"Data\\Sample_BasicShader\\test.ps",
                nullptr, nullptr, "main",
                "ps_5_0", 0, 0, &shaderByteCode, &shaderErr);
            if(FAILED(hr))
            {
                if(shaderErr)
                {
                    std::cout << shaderErr->GetBufferPointer() << std::endl;
                    shaderErr->Release();
                }
                return false;
            }
            else if(shaderErr)
                shaderErr->Release();
            shader_.InitStage<SS_PS>(D3D_, shaderByteCode);
            shaderByteCode->Release();

            VSCBs_ = shader_.GetStage<SS_VS>()->CreateConstantBufferManager();

            //=============顶点缓存初始化=============

            DirectX::XMFLOAT2 vtxBufData[] =
            {
                { -1.0f, -1.0f },
                { 0.0f, 1.0f },
                { 1.0f, -1.0f }
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
                { "POSITION", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 }
            };
            hr = D3D_->CreateInputLayout(
                inputDesc, 1,
                shader_.GetStage<SS_VS>()->GetShaderByteCode(),
                shader_.GetStage<SS_VS>()->GetShaderByteCodeSize(),
                &inputLayout_);
            if(FAILED(hr))
                return false;

            return true;
        }

        void DestroyScene(void)
        {
            using namespace OWEShaderAux;
            ReleaseCOMObjects(inputLayout_, vtxBuf_);
            shader_.Destroy();
            SafeDeleteObjects(VSCBs_);
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

            MSG msg;
            float t = 0.0f;
            while(!(GetKeyState(VK_ESCAPE) & 0x8000))
            {
                float backgroundColor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
                DC_->ClearRenderTargetView(renderTargetView_, backgroundColor);
                DC_->ClearDepthStencilView(depthStencilView_, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

                float sizeComponent = std::sin(t += 0.01f);
                VSCBSize CBSize = { { sizeComponent, sizeComponent }, 0.0f, 0.0f };
                VSCBs_->GetConstantBuffer<VSCBSize, true>(D3D_, "Trans")->SetBufferData(DC_, CBSize);

                DC_->IASetInputLayout(inputLayout_);
                UINT vtxStride = sizeof(DirectX::XMFLOAT2), vtxOffset = 0;
                DC_->IASetVertexBuffers(0, 1, &vtxBuf_, &vtxStride, &vtxOffset);

                shader_.Bind(DC_);
                VSCBs_->Bind(DC_);

                DC_->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
                DC_->Draw(3, 0);

                VSCBs_->Unbind(DC_);
                shader_.Unbind(DC_);

                swapChain_->Present(1, 0);

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

#endif //SAMPLE_BASICShader_H
