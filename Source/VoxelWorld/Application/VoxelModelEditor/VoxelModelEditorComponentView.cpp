/*================================================================
Filename: VoxelModelEditorComponentView.cpp
Date: 2018.3.19
Created by AirGuanZ
================================================================*/
#include <cassert>

#include <Utility\HelperFunctions.h>

#include "VoxelModelEditorComponentView.h"

InputLayout VMEComponentVertex::CreateInputLayout(const void *shaderByteCode, int length)
{
    assert(shaderByteCode != nullptr && length > 0);

    const D3D11_INPUT_ELEMENT_DESC desc[] =
    {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,
            Helper::MemOffset(&VMEComponentVertex::pos), D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,
            Helper::MemOffset(&VMEComponentVertex::nor), D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "COLOR", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,
            Helper::MemOffset(&VMEComponentVertex::color), D3D11_INPUT_PER_VERTEX_DATA, 0 }
    };
    InputLayout rt;
    if(!rt.Initialize(desc, shaderByteCode, static_cast<UINT>(length)))
        return InputLayout();
    return rt;
}
