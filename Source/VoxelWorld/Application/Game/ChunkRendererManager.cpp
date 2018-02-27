/*================================================================
Filename: ChunkRendererManager.cpp
Date: 2018.2.25
Created by AirGuanZ
================================================================*/
#include <Resource/ResourceName.h>
#include <Window/Window.h>

#include "ChunkRendererManager.h"

ChunkRendererManager::ChunkRendererManager(void)
    : sampler_(D3DObj_Noinit())
{
    DC_ = nullptr;

    basicUniform_Trans_    = nullptr;
    basicUniform_Sunlight_ = nullptr;
    basicUniform_Fog_      = nullptr;
    basicUniform_Tex_      = nullptr;

    carveUniform_Trans_    = nullptr;
    carveUniform_Sunlight_ = nullptr;
    carveUniform_Fog_      = nullptr;
    carveUniform_Tex_      = nullptr;

    liquidUniform_Trans_    = nullptr;
    liquidUniform_Sunlight_ = nullptr;
    liquidUniform_Fog_      = nullptr;
    liquidUniform_Tex_      = nullptr;
}

bool ChunkRendererManager::Initialize(std::string &errMsg)
{
    DC_               = Window::GetInstance().GetD3DDeviceContext();
    ID3D11Device *dev = Window::GetInstance().GetD3DDevice();

    if(!basicRenderer_.Initialize(errMsg))
    {
        Destroy();
        return false;
    }

    if(!basicRendererTextures_[0].LoadFromFile(BASIC_RENDERER_TEXTURE_0))
    {
        errMsg = "Failed to load texture for basic renderer 0";
        Destroy();
        return false;
    }

    if(!carveRenderer_.Initialize(errMsg))
    {
        Destroy();
        return false;
    }

    if(!carveRendererTextures_[0].LoadFromFile(CARVE_RENDERER_TEXTURE_0))
    {
        errMsg = "Failed to load texture for carve renderer 0";
        Destroy();
        return false;
    }

    if(!liquidRenderer_.Initialize(errMsg))
    {
        Destroy();
        return false;
    }

    if(!liquidRendererTextures_[0].LoadFromFile(LIQUID_RENDERER_TEXTURE_0))
    {
        errMsg = "Failed to load texture for liquid renderer 0";
        Destroy();
        return false;
    }

    sampler_ = Sampler();
    if(!sampler_)
    {
        errMsg = "Failed to create sampler object for chunk rendering";
        Destroy();
        return false;
    }

    basicUniforms_.reset(basicRenderer_.GetShader().CreateUniformManager());
    basicUniforms_->GetShaderSampler<SS_PS>("sam")->SetSampler(sampler_);

    carveUniforms_.reset(carveRenderer_.GetShader().CreateUniformManager());
    carveUniforms_->GetShaderSampler<SS_PS>("sam")->SetSampler(sampler_);

    liquidUniforms_.reset(liquidRenderer_.GetShader().CreateUniformManager());
    liquidUniforms_->GetShaderSampler<SS_PS>("sam")->SetSampler(sampler_);

    basicUniform_Trans_ = basicUniforms_->GetConstantBuffer<SS_VS, BasicVSCBTrans>(dev, "Trans");
    basicUniform_Fog_ = basicUniforms_->GetConstantBuffer<SS_PS, BasicPSCBFog>(dev, "Fog");
    basicUniform_Sunlight_ = basicUniforms_->GetConstantBuffer<SS_PS, BasicPSCBSunlight>(dev, "Sunlight");
    basicUniform_Tex_ = basicUniforms_->GetShaderResource<SS_PS>("tex");

    carveUniform_Trans_ = carveUniforms_->GetConstantBuffer<SS_VS, CarveVSCBTrans>(dev, "Trans");
    carveUniform_Fog_ = carveUniforms_->GetConstantBuffer<SS_PS, CarvePSCBFog>(dev, "Fog");
    carveUniform_Sunlight_ = carveUniforms_->GetConstantBuffer<SS_PS, CarvePSCBSunlight>(dev, "Sunlight");
    carveUniform_Tex_ = carveUniforms_->GetShaderResource<SS_PS>("tex");

    liquidUniform_Trans_ = liquidUniforms_->GetConstantBuffer<SS_VS, LiquidVSCBTrans>(dev, "Trans");
    liquidUniform_Fog_ = liquidUniforms_->GetConstantBuffer<SS_PS, LiquidPSCBFog>(dev, "Fog");
    liquidUniform_Sunlight_ = liquidUniforms_->GetConstantBuffer<SS_PS, LiquidPSCBSunlight>(dev, "Sunlight");
    liquidUniform_Tex_ = liquidUniforms_->GetShaderResource<SS_PS>("tex");


    return true;
}

void ChunkRendererManager::Destroy(void)
{
    basicRenderer_.Destroy();
    for(Texture2D &tex : basicRendererTextures_)
        tex.Destroy();
    basicUniforms_.reset();

    carveRenderer_.Destroy();
    for(Texture2D &tex : carveRendererTextures_)
        tex.Destroy();
    carveUniforms_.reset();

    liquidRenderer_.Destroy();
    for(Texture2D &tex : liquidRendererTextures_)
        tex.Destroy();
    liquidUniforms_.reset();

    sampler_ = Sampler(D3DObj_Noinit());

    basicUniform_Trans_    = nullptr;
    basicUniform_Sunlight_ = nullptr;
    basicUniform_Fog_      = nullptr;
    basicUniform_Tex_      = nullptr;

    carveUniform_Trans_    = nullptr;
    carveUniform_Sunlight_ = nullptr;
    carveUniform_Fog_      = nullptr;
    carveUniform_Tex_      = nullptr;

    liquidUniform_Trans_    = nullptr;
    liquidUniform_Sunlight_ = nullptr;
    liquidUniform_Fog_      = nullptr;
    liquidUniform_Tex_      = nullptr;
}

void ChunkRendererManager::SetSunlight(const Vector3 &sunlight)
{
    basicUniform_Sunlight_->SetBufferData(DC_, { sunlight });
    carveUniform_Sunlight_->SetBufferData(DC_, { sunlight });
    liquidUniform_Sunlight_->SetBufferData(DC_, { sunlight });
}

void ChunkRendererManager::SetFog(float fogStart, float fogRange,
                                  const Vector3 &fogColor, const Vector3 &camPosW)
{
    basicUniform_Fog_->SetBufferData(DC_, { fogStart, fogColor, fogRange, camPosW });
    carveUniform_Fog_->SetBufferData(DC_, { fogStart, fogColor, fogRange, camPosW });
    liquidUniform_Fog_->SetBufferData(DC_, { fogStart, fogColor, fogRange, camPosW });
}

void ChunkRendererManager::SetTrans(const Matrix &trans)
{
    basicUniform_Trans_->SetBufferData(DC_, { trans });
    carveUniform_Trans_->SetBufferData(DC_, { trans });
    liquidUniform_Trans_->SetBufferData(DC_, { trans });
}

void ChunkRendererManager::Render(ChunkSectionRenderQueue &renderQueue)
{
    for(size_t i = 0; i < basicRendererTextures_.size(); ++i)
    {
        basicUniform_Tex_->SetShaderResource(basicRendererTextures_[i]);

        basicRenderer_.Begin();
        basicUniforms_->Bind(DC_);
        renderQueue.basic[i].Render();
        basicUniforms_->Unbind(DC_);
        basicRenderer_.End();
    }

    for(size_t i = 0; i < carveRendererTextures_.size(); ++i)
    {
        carveUniform_Tex_->SetShaderResource(carveRendererTextures_[i]);

        carveRenderer_.Begin();
        carveUniforms_->Bind(DC_);
        renderQueue.carve[i].Render();
        carveUniforms_->Unbind(DC_);
        carveRenderer_.End();
    }

    for(size_t i = 0; i < liquidRendererTextures_.size(); ++i)
    {
        liquidUniform_Tex_->SetShaderResource(liquidRendererTextures_[i]);

        liquidRenderer_.Begin();
        liquidUniforms_->Bind(DC_);
        renderQueue.liquid[i].Render();
        liquidUniforms_->Unbind(DC_);
        liquidRenderer_.End();
    }
}
