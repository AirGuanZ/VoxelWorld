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

    void AddVertex(const Vertex &vertex);
    void AddIndex(UINT16 index);
    bool MakeVertexBuffer(void);
    void Destroy(void);

    size_t GetVerticesCount(void) const;
    size_t GetIndicesCount(void) const;

private:
    std::vector<Vertex> vertices_;
    std::vector<UINT16> indices_;
};

#endif //VW_BASIC_MODEL_H
