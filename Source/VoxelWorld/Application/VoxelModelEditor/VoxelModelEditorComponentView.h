/*================================================================
Filename: VoxelModelEditorComponentView.h
Date: 2018.3.19
Created by AirGuanZ
================================================================*/
#pragma once

#include <vector>

#include <Utility\D3D11Header.h>
#include <Utility\Math.h>

#include <D3DObject\BasicBuffer.h>
#include <D3DObject\InputLayout.h>
#include <D3DObject\FrameBuffer.h>
#include "VoxelModelEditorCore.h"

class VMEComponentMesh
{
public:
    struct Vertex
    {
        Vector3 pos;    // POSITION
        Vector3 nor;    // NORMAL
        Vector3 color;  // COLOR
    };

    static InputLayout CreateInputLayout(const void *shaderByteCode, int length);

    VMEComponentMesh(void);

    bool Initialize(const Vertex *vtxData, int vtxCount, const UINT16 *idxData, int idxCount);
    bool IsAvailable(void) const;
    void Destroy(void);

    void Bind(ID3D11DeviceContext *DC);
    void Unbind(ID3D11DeviceContext *DC);
    void Draw(void);

private:
    VertexBuffer<false> vtxBuf_;
    IndexBuffer<false> idxBuf_;
    int idxCount_;
};
