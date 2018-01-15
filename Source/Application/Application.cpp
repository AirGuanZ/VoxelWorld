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

#include "../Camera/Camera.h"
#include "../Model/BasicCube.h"
#include "../Renderer/BasicRenderer.h"
#include "../Texture/Texture2D.h"
#include "../Texture/TextureFile.h"
#include "../Utility/HelperFunctions.h"
#include "../Window/Window.h"
#include "Application.h"

void Application::Run(void)
{
    std::string initErrMsg;
    Window &window = Window::GetInstance();
    if(!window.InitWindow(640, 480, L"Voxel World", initErrMsg) ||
       !window.InitD3D(1, 0, initErrMsg))
    {
        throw std::runtime_error(initErrMsg.c_str());
    }

    window.SetBackgroundColor(0.0f, 1.0f, 1.0f, 0.0f);

    BasicCube cubeModel;
    cubeModel.Initialize();

    BasicRenderer basicRenderer;
    if(!basicRenderer.Initialize(initErrMsg))
        throw std::runtime_error(initErrMsg.c_str());

    std::unique_ptr<BasicCube::Uniforms> uniforms(basicRenderer.GetShader().CreateUniformManager());

    Camera camera;
    camera.SetPosition({ 0.0f, 0.0f, 5.0f });

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

    while(!(GetKeyState(VK_ESCAPE) & 0x8000))
    {
        window.ClearRenderTarget();
        window.ClearDepthStencil();

        if(GetKeyState('A') & 0x8000)
        {
            camera.SetYaw(camera.GetYaw() + 0.03f);
            std::cout << camera.GetYaw() << std::endl;
        }

        if(GetKeyState('D') & 0x8000)
        {
            camera.SetYaw(camera.GetYaw() - 0.03f);
            std::cout << camera.GetYaw() << std::endl;
        }

        if(GetKeyState('W') & 0x8000)
            camera.SetPitch(camera.GetPitch() + 0.01f);

        if(GetKeyState('S') & 0x8000)
            camera.SetPitch(camera.GetPitch() - 0.01f);

        if(GetKeyState(VK_SPACE) & 0x8000)
            camera.SetPosition(camera.GetPosition() + 0.03f * camera.GetDirection());

        basicRenderer.Begin();
        cubeModel.Bind();

        camera.UpdateViewProjMatrix();
        struct VSTrans
        {
            Matrix WVP;
        } VSTransData = { camera.GetViewProjMatrix().Transpose() };
        uniforms->GetConstantBuffer<SS_VS, VSTrans>(
            Window::GetInstance().GetD3DDevice(),"Trans")->SetBufferData(
                Window::GetInstance().GetD3DDeviceContext(), VSTransData);

        uniforms->Bind(Window::GetInstance().GetD3DDeviceContext());

        Window::GetInstance().GetD3DDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
        Window::GetInstance().GetD3DDeviceContext()->Draw(36, 0);

        uniforms->Unbind(Window::GetInstance().GetD3DDeviceContext());

        cubeModel.Unbind();
        basicRenderer.End();

        window.Present();
        window.DoEvents();
    }

    window.Destroy();
}
