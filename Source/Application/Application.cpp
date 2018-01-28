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

#include "../D3DObject/Sampler.h"
#include "../Input/InputManager.h"
#include "../Renderer/BasicRenderer.h"
#include "../Resource/ResourceName.h"
#include "../Screen/Crosshair.h"
#include "../Texture/Texture2D.h"
#include "../Utility/Clock.h"
#include "../Utility/ConfigFile.h"
#include "../Utility/FPSCounter.h"
#include "../Utility/HelperFunctions.h"
#include "../Window/Window.h"
#include "../World/World.h"
#include "Application.h"

void Application::Run(void)
{
    ConfigFile conf(L"config.txt");
    if(!conf)
        throw std::runtime_error("Failed to load configure file");

    std::string initErrMsg;
    Window &window      = Window::GetInstance();
    InputManager &input = InputManager::GetInstance();
    if(!window.InitWindow(std::stoi(conf("Initialize", "WindowWidth")),
                          std::stoi(conf("Initialize", "WindowHeight")),
                          L"Voxel World", initErrMsg) ||
       !window.InitD3D(std::stoi(conf("Initialize", "MSAA")), 0, initErrMsg))
    {
        throw std::runtime_error(initErrMsg.c_str());
    }

    ID3D11Device *dev = window.GetD3DDevice();
    ID3D11DeviceContext *DC = window.GetD3DDeviceContext();

    //渲染器和纹理

    ChunkSectionRenderQueue renderQueue;

    BasicRenderer basicRenderer;
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

    CarveRenderer carveRenderer;
    Texture2D carveRendererTexture0;

    if(!carveRenderer.Initialize(initErrMsg))
    {
        std::cerr << initErrMsg << std::endl;
        return;
    }

    if(!carveRendererTexture0.LoadFromFile(CARVE_RENDERER_TEXTURE_0))
    {
        std::cerr << "Failed to load texture for carve renderer 0" << std::endl;
        return;
    }

    LiquidRenderer liquidRenderer;
    Texture2D liquidRendererTexture0;

    if(!liquidRenderer.Initialize(initErrMsg))
    {
        std::cerr << initErrMsg << std::endl;
        return;
    }

    if(!liquidRendererTexture0.LoadFromFile(LIQUID_RENDERER_TEXTURE_0))
    {
        std::cerr << "Failed to load texture for liquid renderer 0" << std::endl;
        return;
    }

    //Sampler
    Sampler sampler;
    if(!sampler)
    {
        std::cerr << "Failed to create sampler object" << std::endl;
        return;
    }

    //basic renderer参数

    struct BasicVSCBTrans
    {
        Matrix WVP;
    };

    struct BasicPSCBSunlight
    {
        alignas(16) Vector3 sunlight;
    };

    std::unique_ptr<BasicRenderer::Uniforms> basicRendererUniforms0(basicRenderer.GetShader().CreateUniformManager());
    basicRendererUniforms0->GetShaderSampler<SS_PS>("sam")
        ->SetSampler(sampler);
    basicRendererUniforms0->GetShaderResource<SS_PS>("tex")
        ->SetShaderResource(basicRendererTexture0);

    //carve render参数

    struct CarveVSCBTrans
    {
        Matrix WVP;
    };

    struct CarvePSCBSunlight
    {
        alignas(16) Vector3 sunlight;
    };

    std::unique_ptr<CarveRenderer::Uniforms> carveRendererUniforms0(carveRenderer.GetShader().CreateUniformManager());
    carveRendererUniforms0->GetShaderSampler<SS_PS>("sam")
        ->SetSampler(sampler);
    carveRendererUniforms0->GetShaderResource<SS_PS>("tex")
        ->SetShaderResource(carveRendererTexture0);

    //liquid renderer参数

    struct LiquidVSCBTrans
    {
        Matrix WVP;
    };

    struct LiquidPSCBSunlight
    {
        alignas(16) Vector3 sunlight;
    };

    std::unique_ptr<LiquidRenderer::Uniforms> liquidRendererUniforms0(liquidRenderer.GetShader().CreateUniformManager());
    liquidRendererUniforms0->GetShaderSampler<SS_PS>("sam")
        ->SetSampler(sampler);
    liquidRendererUniforms0->GetShaderResource<SS_PS>("tex")
        ->SetShaderResource(liquidRendererTexture0);

    //区块世界

    World world(std::stoi(conf("World", "PreloadDistance")), std::stoi(conf("World", "RenderDistance")));
    world.Initialize();

    //准星

    ImmediateScreen2D imScr2D;
    if(!imScr2D.Initialize(initErrMsg))
    {
        std::cerr << initErrMsg << std::endl;
        return;
    }

    Crosshair crosshair;

    input.LockCursor(true, window.ClientCentreX(), window.ClientCentreY());
    input.ShowCursor(false);

    Clock clock;
    clock.Restart();

    FPSCounter fps;
    fps.Restart();

    float daynightT = 0.0f;

    while(!input.IsKeyDown(VK_ESCAPE))
    {
        window.ClearRenderTarget();
        window.ClearDepthStencil();

        daynightT += input.IsKeyDown('T') ? 0.01f : 0.001f;
        float absdnt = 0.5f * (std::max)((std::min)(2.0f * std::cos(daynightT), 1.0f), -1.0f) + 0.5f;

        Vector3 sunlight = { absdnt, absdnt, absdnt };

        window.SetBackgroundColor(0.0f, absdnt, absdnt, 0.0f);

        basicRendererUniforms0->GetConstantBuffer<SS_PS, BasicPSCBSunlight>(dev, "Sunlight")
            ->SetBufferData(DC, { sunlight });
        carveRendererUniforms0->GetConstantBuffer<SS_PS, CarvePSCBSunlight>(dev, "Sunlight")
            ->SetBufferData(DC, { sunlight });
        liquidRendererUniforms0->GetConstantBuffer<SS_PS, LiquidPSCBSunlight>(dev, "Sunlight")
            ->SetBufferData(DC, { sunlight });

        world.Update(clock.ElapsedTime());

        world.Render(&renderQueue);

        //basic rendering

        basicRendererUniforms0->GetConstantBuffer<SS_VS, BasicVSCBTrans>(dev, "Trans")
            ->SetBufferData(DC, { world.GetActor().GetCamera().GetViewProjMatrix().Transpose() });

        basicRenderer.Begin();
        basicRendererUniforms0->Bind(DC);
        renderQueue.basic[0].Render();
        basicRendererUniforms0->Unbind(DC);
        basicRenderer.End();

        //carve rendering

        carveRendererUniforms0->GetConstantBuffer<SS_VS, CarveVSCBTrans>(dev, "Trans")
            ->SetBufferData(DC, { world.GetActor().GetCamera().GetViewProjMatrix().Transpose() });

        carveRenderer.Begin();
        carveRendererUniforms0->Bind(DC);
        renderQueue.carve[0].Render();
        carveRendererUniforms0->Unbind(DC);
        carveRenderer.End();

        //liquid rendering

        liquidRendererUniforms0->GetConstantBuffer<SS_VS, LiquidVSCBTrans>(dev, "Trans")
            ->SetBufferData(DC, { world.GetActor().GetCamera().GetViewProjMatrix().Transpose() });

        liquidRenderer.Begin();
        liquidRendererUniforms0->Bind(DC);
        renderQueue.liquid[0].Render();
        liquidRendererUniforms0->Unbind(DC);
        liquidRenderer.End();

        crosshair.Draw(&imScr2D);

        window.Present();
        window.DoEvents();
        clock.Tick();
    }

    window.Destroy();
}
