/*================================================================
Filename: ActorModel.h
Date: 2018.2.5
Created by AirGuanZ
================================================================*/
#ifndef VW_ACTOR_MODEL_H
#define VW_ACTOR_MODEL_H

#include <memory>
#include <vector>

#include <OWEShader.hpp>

#include "../Camera/Camera.h"
#include "../D3DObject/BasicBuffer.h"
#include "../D3DObject/Sampler.h"
#include "../Texture/Texture2D.h"
#include "../Utility/Math.h"

enum class ActorModelState
{
    Standing,
    Running
};

struct ActorModelVertex
{
    Vector3 pos;    // POSITION
    Vector3 nor;    // NORMAL
    Vector2 uv;     // TEXCOORD
};

class ActorRenderer
{
public:
    using Shader = OWE::Shader<SS_VS, SS_PS>;
    using Uniforms = OWE::ShaderUniforms<SS_VS, SS_PS>;

    ActorRenderer(void);

    bool Initialize(std::string &errMsg);
    void Destroy(void);

    void Begin(void);
    void End(void);

    Shader &GetShader(void);

private:
    Shader shader_;
    ID3D11InputLayout *inputLayout_;
};

class ActorModelComponent
{
public:
    using Vertex   = ActorModelVertex;

    ActorModelComponent(void);
    
    bool Initialize(const std::vector<Vertex> vtxData,
                    const std::vector<UINT> idxData);
    void Destroy(void);
    bool IsAvailable(void) const;
    
    void Draw(void);
    
private:
    BasicBuffer<D3D11_BIND_VERTEX_BUFFER, false> vtxBuf_;
    BasicBuffer<D3D11_BIND_INDEX_BUFFER, false>  idxBuf_;
    size_t idxCount_;
};

class ActorModel
{
public:
    ActorModel(void);
    
    bool Initialize(std::string &errMsg);

    void SetState(ActorModelState state);
    void Update(float deltaT);
    
    void SetBrightness(float brightness);

    void Render(const Camera &cam, const Matrix &worldTrans);
    
private:
    void Destroy(void);

    void InitState_Standing(void);
    void InitState_Running(void);
    
    void UpdateState_Standing(float deltaT);
    void UpdateState_Running(float deltaT);

private:
    ActorModelState state_;
    float time_;
    
    float brightness_;

    ActorModelComponent headModel_;
    ActorModelComponent bodyModel_;
    ActorModelComponent leftHandModel_;
    ActorModelComponent rightHandModel_;
    ActorModelComponent leftFootModel_;
    ActorModelComponent rightFootModel_;
    
    Matrix headLocalTrans_;
    Matrix bodyLocalTrans_;
    Matrix leftHandLocalTrans_;
    Matrix rightHandLocalTrans_;
    Matrix leftFootLocalTrans_;
    Matrix rightFootLocalTrans_;
    
    Texture2D headTex_;
    Texture2D bodyTex_;
    Texture2D leftHandTex_;
    Texture2D rightHandTex_;
    Texture2D leftFootTex_;
    Texture2D rightFootTex_;

    Sampler sampler_;
    ActorRenderer renderer_;
    std::unique_ptr<ActorRenderer::Uniforms> uniforms_;
};

#endif //VW_ACTOR_MODEL_H
