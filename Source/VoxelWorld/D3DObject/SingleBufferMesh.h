/*================================================================
Filename: SingleBufferMesh.h
Date: 2018.3.19
Created by AirGuanZ
================================================================*/
#pragma once

#include <cassert>
#include <vector>

#include <Utility\D3D11Header.h>
#include <Utility\Uncopiable.h>

#include "BasicBuffer.h"

template<typename _vertexType>
class SingleBufferMesh : public Uncopiable
{
public:
    using Vertex = _vertexType;
    using Index = UINT16;

    SingleBufferModel(void)
    {
        idxCnt_ = 0;
    }

    bool Initialize(const std::vector<Vertex> &vtxData, const std::vector<Index> &idxData)
    {
        return Initialize(vtxData.data(), vtxData.size(), idxData.data(), idxData.size());
    }

    bool Initialize(const Vertex *vtxData, size_t vtxCnt, const Index *idxData, size_t idxCnt)
    {
        assert(vtxData != nullptr && vtxCnt > 0 && idxData != nullptr && idxCnt > 0);
        Destroy();

        if(!vtxBuf_.Initialize(vtxData, vtxCnt) || idxBuf_.Initialize(idxData, idxCnt))
        {
            Destroy();
            return false;
        }
        idxCnt_ = idxCnt;

        return true;
    }

    bool IsAvailable(void) const
    {
        assert(vtxBuf_.IsAvailable() == idxBuf_.IsAvailable());
        return vtxBuf_.IsAvailable();
    }

    void Destroy(void)
    {
        vtxBuf_.Destroy();
        idxBuf_.Destroy();
        idxCnt_ = 0;
    }

    void Bind(ID3D11DeviceContext *DC)
    {
        assert(IsAvailable() && DC != nullptr);
        static const UINT stride = sizeof(Vertex), offset = 0;
        ID3D11Buffer *vtxBuf = vtxBuf_.GetBuffer();
        DC->IASetVertexBuffers(0, 1, &vtxBuf, &stride, &offset);
        DC->IASetIndexBuffer(idxBuf_.GetBuffer(), DXGI_FORMAT_R16_UINT, 0);
    }

    void Unbind(ID3D11DeviceContext *DC)
    {
        assert(IsAvailable() && DC != nullptr);
        static const UINT stride = 0, offset = 0;
        static const ID3D11Buffer *vtxBuf = nullptr;
        DC->IASetVertexBuffers(0, 1, &vtxBuf, &stride, &offset);
        DC->IASetIndexBuffer(nullptr, DXGI_FORMAT_UNKNOWN, 0);
    }

    void Draw(ID3D11DeviceContext *DC)
    {
        assert(IsAvailable() && DC != nullptr);
        DC->DrawIndexed(idxCnt_, 0, 0);
    }

private:
    VertexBuffer<false> vtxBuf_;
    IndexBuffer<false> idxBuf_;
    size_t idxCnt_;
};
