/*================================================================
Filename: BasicCube.h
Date: 2018.1.15
Created by AirGuanZ
================================================================*/
#ifndef VW_BASIC_CUBE_H
#define VW_BASIC_CUBE_H

#include "../Renderer/BasicRenderer.h"
#include "../Utility/Uncopiable.h"
#include "Model.h"

class BasicCube : public Model, public Uncopiable
{
public:
    using Renderer = BasicRenderer;
    using Vertex = typename Renderer::Vertex;
    using Uniforms = typename Renderer::Uniforms;

    BasicCube(void) = default;
    ~BasicCube(void);

    bool Initialize(float size = 1.0f, Vector3 posOffset = Vector3(0.0f, 0.0f, 0.0f));
    void Destroy(void);
};

#endif //VW_BASIC_CUBE_H
