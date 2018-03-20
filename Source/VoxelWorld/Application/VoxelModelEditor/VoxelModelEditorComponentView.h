/*================================================================
Filename: VoxelModelEditorComponentView.h
Date: 2018.3.19
Created by AirGuanZ
================================================================*/
#pragma once

#include <vector>

#include <Utility\Math.h>

#include <D3DObject\InputLayout.h>
#include <D3DObject\SingleBufferMesh.h>
#include "VoxelModelEditorCore.h"

struct VMEComponentVertex
{
    static InputLayout CreateInputLayout(const void *shaderByteCode, int length);

    Vector3 pos;    // POSITION
    Vector3 nor;    // NORMAL
    Vector3 color;  // COLOR
};

using VMEComponentMesh = SingleBufferMesh<VMEComponentVertex>;
