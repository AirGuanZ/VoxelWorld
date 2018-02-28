/*================================================================
Filename: ChunkRendererManager.cpp
Date: 2018.2.25
Created by AirGuanZ
================================================================*/
#include <Resource/ResourceNameManager.h>
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
    RscNameMgr &rscMgr = RscNameMgr::GetInstance();
    DC_               = Window::GetInstance().GetD3DDeviceContext();
    ID3D11Device *dev = Window::GetInstance().GetD3DDevice();

    if(!basicRenderer_.Initialize(errMsg))
    {
        Destroy();
        return false;
    }

    for(size_t i = 0; i < basicRendererTextures_.size(); ++i)
    {
        if(!basicRendererTextures_[i].LoadFromFile(rscMgr(u8"BasicRenderer", u8"Texture[" + std::to_string(i) + u8"]")))
        {
            errMsg = u8"Failed to load textures for basic renderer";
            Destroy();
            return false;
        }
    }

    if(!carveRenderer_.Initialize(errMsg))
    {
        Destroy();
        return false;
    }

    for(size_t i = 0; i < carveRendererTextures_.size(); ++i)
    {
        if(!carveRendererTextures_[i].LoadFromFile(rscMgr(u8"CarveRenderer", u8"Texture[" + std::to_string(i) + u8"]")))
        {
            errMsg = u8"Failed to load textures for carve renderer";
            Destroy();
            return false;
        }
    }

    if(!liquidRenderer_.Initialize(errMsg))
    {
        Destroy();
        return false;
    }

    for(size_t i = 0; i < liquidRendererTextures_.size(); ++i)
    {
        if(!liquidRendererTextures_[i].LoadFromFile(rscMgr(u8"LiquidRenderer", u8"Texture[" + std::to_string(i) + u8"]")))
        {
            errMsg = u8"Failed to load textures for liquid renderer";
            Destroy();
            return false;
        }
    }

    sampler_ = Sampler();
    if(!sampler_)
    {
        errMsg = u8"Failed to create sampler object for chunk rendering";
        Destroy();
        return false;
    }

    basicUniforms_.reset(basicRenderer_.GetShader().CreateUniformManager());
    basicUniforms_->GetShaderSampler<SS_PS>(u8"sam")->SetSampler(sampler_);

    carveUniforms_.reset(carveRenderer_.GetShader().CreateUniformManager());
    carveUniforms_->GetShaderSampler<SS_PS>(u8"sam")->SetSampler(sampler_);

    liquidUniforms_.reset(liquidRenderer_.GetShader().CreateUniformManager());
    liquidUniforms_->GetShaderSampler<SS_PS>(u8"sam")->SetSampler(sampler_);

    basicUniform_Trans_ = basicUniforms_->GetConstantBuffer<SS_VS, BasicVSCBTrans>(dev, u8"Trans");
    basicUniform_Fog_ = basicUniforms_->GetConstantBuffer<SS_PS, BasicPSCBFog>(dev, u8"Fog");
    basicUniform_Sunlight_ = basicUniforms_->GetConstantBuffer<SS_PS, BasicPSCBSunlight>(dev, u8"Sunlight");
    basicUniform_Tex_ = basicUniforms_->GetShaderResource<SS_PS>(u8"tex");

    carveUniform_Trans_ = carveUniforms_->GetConstantBuffer<SS_VS, CarveVSCBTrans>(dev, u8"Trans");
    carveUniform_Fog_ = carveUniforms_->GetConstantBuffer<SS_PS, CarvePSCBFog>(dev, u8"Fog");
    carveUniform_Sunlight_ = carveUniforms_->GetConstantBuffer<SS_PS, CarvePSCBSunlight>(dev, u8"Sunlight");
    carveUniform_Tex_ = carveUniforms_->GetShaderResource<SS_PS>(u8"tex");

    liquidUniform_Trans_ = liquidUniforms_->GetConstantBuffer<SS_VS, LiquidVSCBTrans>(dev, u8"Trans");
    liquidUniform_Fog_ = liquidUniforms_->GetConstantBuffer<SS_PS, LiquidPSCBFog>(dev, u8"Fog");
    liquidUniform_Sunlight_ = liquidUniforms_->GetConstantBuffer<SS_PS, LiquidPSCBSunlight>(dev, u8"Sunlight");
    liquidUniform_Tex_ = liquidUniforms_->GetShaderResource<SS_PS>(u8"tex");


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
