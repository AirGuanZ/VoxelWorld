/*================================================================
Filename: BasicModel.h
Date: 2018.1.15
Created by AirGuanZ
================================================================*/
#ifndef VW_BASIC_MODEL_H
#define VW_BASIC_MODEL_H

#include <vector>

#include "../Renderer/BasicRenderer.h"
#include "../Utility/Uncopiable.h"
#include "Model.h"

class BasicModel : public Model, public Uncopiable
{
public:
    using Renderer = BasicRenderer;
    using Vertex = typename Renderer::Vertex;
    using Uniforms = typename Renderer::Uniforms;

    BasicModel(void);
    ~BasicModel(void);

    void AddTriangle(const Vertex *triangle);
    bool MakeVertexBuffer(void);
    void Destroy(void);

private:
    std::vector<Vertex> triangles_;
};

#endif //VW_BASIC_MODEL_H
