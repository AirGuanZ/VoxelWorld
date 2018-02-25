/*================================================================
Filename: ChunkRendererManager.h
Date: 2018.2.25
Created by AirGuanZ
================================================================*/
#ifndef VW_CHUNK_RENDERER_MANAGER_H
#define VW_CHUNK_RENDERER_MANAGER_H

#include <array>
#include <memory>

#include <Utility/Math.h>

#include <Chunk/BasicRenderer.h>
#include <Chunk/CarveRenderer.h>
#include <Chunk/LiquidRenderer.h>
#include <Chunk/ChunkManager.h>
#include <D3DObject/Sampler.h>
#include <Texture/Texture2D.h>

class ChunkRendererManager
{
public:
    ChunkRendererManager(void) = default;

    bool Initialize(std::string &errMsg);

    void Destroy(void);

    void SetSunlight(ID3D11DeviceContext *DC, const Vector3 &sunlight);

    void SetFog(ID3D11DeviceContext *DC, float fogStart, float fogRange,
                const Vector3 &fogColor, const Vector3 &camPosW);

    void SetTrans(ID3D11DeviceContext *DC, const Matrix &trans);

    void Render(ID3D11DeviceContext *DC, ChunkSectionRenderQueue &renderQueue);

private:
    struct BasicVSCBTrans
    {
        Matrix VP;
    };
    struct BasicPSCBSunlight
    {
        alignas(16) Vector3 sunlight;
    };
    struct BasicPSCBFog
    {
        float fogStart;
        Vector3 fogColor;
        float fogRange;
        Vector3 camPosW;
    };
    struct CarveVSCBTrans
    {
        Matrix VP;

    };
    struct CarvePSCBSunlight
    {
        alignas(16) Vector3 sunlight;
    };
    struct CarvePSCBFog
    {
        float fogStart;
        Vector3 fogColor;
        float fogRange;
        Vector3 camPosW;
    };
    struct LiquidVSCBTrans
    {
        Matrix VP;
    };
    struct LiquidPSCBSunlight
    {
        alignas(16) Vector3 sunlight;
    };
    struct LiquidPSCBFog
    {
        float fogStart;
        Vector3 fogColor;
        float fogRange;
        Vector3 camPosW;
    };

private:
    BasicRenderer basicRenderer_;
    std::array<Texture2D, BASIC_RENDERER_TEXTURE_NUM> basicRendererTextures_;
    std::unique_ptr<BasicRenderer::Uniforms> basicUniforms_;
    OWE::ConstantBufferObject<SS_VS, BasicVSCBTrans>    *basicUniform_Trans_ = nullptr;
    OWE::ConstantBufferObject<SS_PS, BasicPSCBSunlight> *basicUniform_Sunlight_ = nullptr;
    OWE::ConstantBufferObject<SS_PS, BasicPSCBFog>      *basicUniform_Fog_ = nullptr;
    OWE::ShaderResourceObject<SS_PS>                    *basicUniform_Tex_ = nullptr;

    CarveRenderer carveRenderer_;
    std::array<Texture2D, CARVE_RENDERER_TEXTURE_NUM> carveRendererTextures_;
    std::unique_ptr<CarveRenderer::Uniforms> carveUniforms_;
    OWE::ConstantBufferObject<SS_VS, CarveVSCBTrans>    *carveUniform_Trans_ = nullptr;
    OWE::ConstantBufferObject<SS_PS, CarvePSCBSunlight> *carveUniform_Sunlight_ = nullptr;
    OWE::ConstantBufferObject<SS_PS, CarvePSCBFog>      *carveUniform_Fog_ = nullptr;
    OWE::ShaderResourceObject<SS_PS>                    *carveUniform_Tex_ = nullptr;

    LiquidRenderer liquidRenderer_;
    std::array<Texture2D, LIQUID_RENDERER_TEXTURE_NUM> liquidRendererTextures_;
    std::unique_ptr<LiquidRenderer::Uniforms> liquidUniforms_;
    OWE::ConstantBufferObject<SS_VS, LiquidVSCBTrans>    *liquidUniform_Trans_ = nullptr;
    OWE::ConstantBufferObject<SS_PS, LiquidPSCBSunlight> *liquidUniform_Sunlight_ = nullptr;
    OWE::ConstantBufferObject<SS_PS, LiquidPSCBFog>      *liquidUniform_Fog_ = nullptr;
    OWE::ShaderResourceObject<SS_PS>                     *liquidUniform_Tex_ = nullptr;

    std::unique_ptr<Sampler> sampler_;
};

#endif //VW_CHUNK_RENDERER_MANAGER_H
