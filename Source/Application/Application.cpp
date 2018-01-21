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

#include "../Input/InputManager.h"
#include "../Renderer/BasicRenderer.h"
#include "../Resource/ResourceName.h"
#include "../Texture/Sampler.h"
#include "../Texture/Texture2D.h"
#include "../Utility/Clock.h"
#include "../Utility/HelperFunctions.h"
#include "../Window/Window.h"
#include "../World/World.h"
#include "Application.h"

void Application::Run(void)
{
    std::string initErrMsg;
    Window &window      = Window::GetInstance();
    InputManager &input = InputManager::GetInstance();
    if(!window.InitWindow(800, 600, L"Voxel World", initErrMsg) ||
       !window.InitD3D(4, 0, initErrMsg))
    {
        throw std::runtime_error(initErrMsg.c_str());
    }

    window.SetBackgroundColor(0.0f, 1.0f, 1.0f, 0.0f);

    ID3D11Device *dev = window.GetD3DDevice();
    ID3D11DeviceContext *DC = window.GetD3DDeviceContext();

    //��Ⱦ��������

    BasicRenderer basicRenderer;
    ChunkSectionRenderQueue renderQueue;
    Texture2D basicRendererTexture0;

    if(!basicRenderer.Initialize(initErrMsg))
    {
        std::cerr << initErrMsg << std::endl;
        return;
    }

    if(!basicRendererTexture0.LoadFromFile(BASIC_RENDERER_TEXTURE_0))
    {
        std::cerr << "Failed to load texture for basic renderer 0" << std::endl;
        return;
    }

    //Sampler
    Sampler sampler;
    if(!sampler)
    {
        std::cerr << "Failed to create sampler object" << std::endl;
        return;
    }

    //basic renderer����

    struct VSCBTrans
    {
        Matrix WVP;
    };

    struct PSCBSunlight
    {
        alignas(16) Vector3 sunlight;
    };

    std::unique_ptr<BasicRenderer::Uniforms> basicRendererUniforms0(basicRenderer.GetShader().CreateUniformManager());
    basicRendererUniforms0->GetConstantBuffer<SS_PS, PSCBSunlight>(dev, "Sunlight")
        ->SetBufferData(DC, { { 1.0f, 1.0f, 1.0f } });
    basicRendererUniforms0->GetShaderSampler<SS_PS>("sam")
        ->SetSampler(sampler);
    basicRendererUniforms0->GetShaderResource<SS_PS>("tex")
        ->SetShaderResource(basicRendererTexture0);

    //��������

    World world;
    world.Initialize();

    Clock clock;
    clock.Restart();

    input.LockCursor(true, window.ClientCentreX(), window.ClientCentreY());
    input.ShowCursor(false);

    while(!input.IsKeyDown(VK_ESCAPE))
    {
        window.ClearRenderTarget();
        window.ClearDepthStencil();

        world.Update(clock.ElapsedTime());

        world.Render(&renderQueue);

        basicRendererUniforms0->GetConstantBuffer<SS_VS, VSCBTrans>(dev, "Trans")
            ->SetBufferData(DC, { world.GetActor().GetCamera().GetViewProjMatrix().Transpose() });

        basicRenderer.Begin();
        basicRendererUniforms0->Bind(DC);

        renderQueue.basic[0].Render();

        basicRendererUniforms0->Unbind(DC);
        basicRenderer.End();

        window.Present();
        window.DoEvents();
        clock.Tick();
    }

    window.Destroy();
}
