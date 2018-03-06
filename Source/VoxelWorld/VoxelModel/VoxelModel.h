/*================================================================
Filename: VoxelModel.h
Date: 2018.3.3
Created by AirGuanZ
================================================================*/
#pragma once

#include <vector>

#include <Utility/D3D11Header.h>
#include <Utility/Math.h>
#include <Utility/Singleton.h>

#include <D3DObject/BasicBuffer.h>
#include <D3DObject/InputLayout.h>

struct VoxelModelVertex
{
    Vector3 pos;    //POSITION
    Vector3 color;  //COLOR
    Vector3 nor;    //NORMAL
};

class VoxelModel
{
public:
    using Vertex = VoxelModelVertex;

    static InputLayout CreateInputLayout(const void *shaderByteCode, UINT length);

    static bool LoadFromFile(const std::wstring &filename, VoxelModel &model);
    static bool SaveToFile(const std::wstring &filename,
                           const std::vector<VoxelModelVertex> &vtx,
                           const std::vector<UINT16> &idx);

    bool Initialize(const std::vector<VoxelModelVertex> &vtx, const std::vector<UINT16> &idx);
    void Destroy(void);
    bool IsAvailable(void) const;

    UINT GetIndexCount(void) const;

    void Bind(ID3D11DeviceContext *DC);
    void Unbind(ID3D11DeviceContext *DC);

private:
    VertexBuffer<false> vtxBuf_;
    IndexBuffer<false> idxBuf_;
    size_t idxCount_ = 0;
};
