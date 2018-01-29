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

    void AddVertex(const Vertex &vertex)
    {
        vertices_.push_back(vertex);
    }

    void AddIndex(UINT16 index)
    {
        indices_.push_back(index);
    }

    bool MakeVertexBuffer(void);
    
    void Destroy(void);

    size_t GetVerticesCount(void) const
    {
        return vertices_.size();
    }

    size_t GetIndicesCount(void) const
    {
        return indices_.size();
    }

private:
    std::vector<Vertex> vertices_;
    std::vector<UINT16> indices_;
};

using CarveModel = BasicModel;
using LiquidModel = BasicModel;

#endif //VW_BASIC_MODEL_H
