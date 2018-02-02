/*================================================================
Filename: ActorModel.h
Date: 2018.2.2
Created by AirGuanZ
================================================================*/
#ifndef VW_ACTOR_MODEL_H
#define VW_ACTOR_MODEL_H

#include <map>
#include <string>
#include <vector>

#include "../D3DObject/BasicBuffer.h"
#include "../Renderer/ActorRenderer.h"
#include "../Utility/Math.h"
#include "../Utility/Uncopiable.h"

class ActorSubmodel : public Uncopiable
{
public:
    using Vertex = ActorRenderer::Vertex;

    ActorSubmodel(void) = default;

    void AddVertex(const Vertex &vtx);
    void AddIndex(UINT idx);

    bool MakeBuffer(void);
    void DestroyBuffer(void);
    void Clear(void);

    bool IsAvailable(void) const;

    void Bind(void);
    void Draw(void);
    void Unbind(void);

private:
    BasicBuffer<D3D11_BIND_VERTEX_BUFFER, false> vtxBuf_;
    BasicBuffer<D3D11_BIND_INDEX_BUFFER, false>  idxBuf_;
    int idxCount_ = 0;

    std::vector<Vertex> vtxData_;
    std::vector<UINT> idxData_;
};

class ActorComponent : public Uncopiable
{
public:
    friend class ActorModel;

    void SetTransform(const Matrix &mat);
    void AddChild(ActorComponent *child);

    const Matrix &GetFinalTransform(void) const;

    ActorSubmodel &GetModel(void);

private:
    void Render(const Matrix &viewProj, ActorRenderer::Uniforms *uniforms);

private:
    ActorSubmodel model_;

    Matrix world_;
    Matrix finalWorld_;
    const ActorComponent *parent_ = nullptr;

    std::vector<ActorComponent*> children_;
};

class ActorModel : public Uncopiable
{
public:
    void Render(const Matrix &viewProj, ActorRenderer::Uniforms *uniforms);

    void AddComponent(ActorComponent *component);

    void SetTransform(const Matrix &world);

private:
    ActorComponent root_;
};

#endif //VW_ACTOR_MODEL_H
