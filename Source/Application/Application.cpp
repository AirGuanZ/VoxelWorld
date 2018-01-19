/*================================================================
Filename: Application.cpp
Date: 2018.1.13
Created by AirGuanZ
================================================================*/
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>

#include <Windows.h>

#include "../Actor/Actor.h"
#include "../Input/InputManager.h"
#include "../Model/BasicCube.h"
#include "../Renderer/BasicRenderer.h"
#include "../Texture/Texture2D.h"
#include "../Utility/HelperFunctions.h"
#include "../Window/Window.h"
#include "Application.h"

void Application::Run(void)
{
    std::string initErrMsg;
    Window &window = Window::GetInstance();
    if(!window.InitWindow(1200, 900, L"Voxel World", initErrMsg) ||
       !window.InitD3D(4, 0, initErrMsg))
    {
        throw std::runtime_error(initErrMsg.c_str());
    }

    window.SetBackgroundColor(0.0f, 1.0f, 1.0f, 0.0f);

    Actor actor;

    InputManager::GetInstance().LockCursor(true, window.ClientCentreX(), window.ClientCentreY());

    BasicCube cubeModel;
    cubeModel.Initialize(1.0f, actor.GetCamera().GetDirection() * 5.0f);

    BasicRenderer basicRenderer;
    if(!basicRenderer.Initialize(initErrMsg))
        throw std::runtime_error(initErrMsg.c_str());

    std::unique_ptr<BasicCube::Uniforms> uniforms(basicRenderer.GetShader().CreateUniformManager());

    Texture2D tex;
    if(!tex.LoadFromFile(L"tex.dds"))
        throw std::runtime_error("Failed to load texture from file");

    uniforms->GetShaderResource<SS_PS>("tex")->SetShaderResource(tex.GetSRV());

    ID3D11SamplerState *sampler = Helper::CreateSamplerState(
        D3D11_FILTER_MIN_MAG_MIP_LINEAR,
        D3D11_TEXTURE_ADDRESS_CLAMP);
    if(!sampler)
        throw std::runtime_error("Failed to create sampler");
    uniforms->GetShaderSampler<SS_PS>("sam")->SetSampler(sampler);
    Helper::ReleaseCOMObjects(sampler);

    while(!InputManager::GetInstance().IsKeyDown(VK_ESCAPE))
    {
        window.ClearRenderTarget();
        window.ClearDepthStencil();

        actor.UpdateCamera(1.0f);

        basicRenderer.Begin();
        cubeModel.Bind();

        struct VSTrans
        {
            Matrix WVP;
        } VSTransData = { actor.GetViewProjMatrix().Transpose() };
        uniforms->GetConstantBuffer<SS_VS, VSTrans>(
            Window::GetInstance().GetD3DDevice(), "Trans")->SetBufferData(
                Window::GetInstance().GetD3DDeviceContext(), VSTransData);

        uniforms->Bind(Window::GetInstance().GetD3DDeviceContext());

        Window::GetInstance().GetD3DDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
        cubeModel.Draw();

        uniforms->Unbind(Window::GetInstance().GetD3DDeviceContext());

        cubeModel.Unbind();
        basicRenderer.End();

        window.Present();
        window.DoEvents();
    }

    window.Destroy();
}
