/*================================================================
Filename: Model.cpp
Date: 2018.1.13
Created by AirGuanZ
================================================================*/
#include "../Window/Window.h"
#include "Model.h"

const std::vector<ID3D11Buffer*> Model::emptyVtxBufs_(
    D3D11_IA_VERTEX_INPUT_RESOURCE_SLOT_COUNT, nullptr);

bool Model::IsAvailable(void) const
{
    assert(vtxBufBinding_.startSlot < 0 ||
        vtxBufBinding_.startSlot + vtxBufBinding_.bufs.size()
        <= D3D11_IA_VERTEX_INPUT_RESOURCE_SLOT_COUNT);
    return vtxBufBinding_.bufs.size() > 0;
}

void Model::Draw(void) const
{
    if(!IsAvailable())
        return;
    ID3D11DeviceContext *DC = Window::GetInstance().GetD3DDeviceContext();
    DC->IASetVertexBuffers(
        vtxBufBinding_.startSlot,
        vtxBufBinding_.bufs.size(),
        vtxBufBinding_.bufs.data(),
        vtxBufBinding_.strides.data(),
        vtxBufBinding_.offsets.data());
    DC->IASetIndexBuffer(
        vtxBufBinding_.indices, vtxBufBinding_.indicesFormat, 0);

    DC->DrawIndexed(
        vtxBufBinding_.idxCount, 0, 0);

    DC->IASetVertexBuffers(
        vtxBufBinding_.startSlot,
        vtxBufBinding_.bufs.size(),
        emptyVtxBufs_.data(),
        vtxBufBinding_.strides.data(),
        vtxBufBinding_.offsets.data());
    DC->IASetIndexBuffer(
        nullptr, vtxBufBinding_.indicesFormat, 0);
}
