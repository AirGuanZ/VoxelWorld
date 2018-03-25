/*================================================================
Filename: VMEComponentMeshBuilder.h
Date: 2018.3.24
Created by AirGuanZ
================================================================*/
#pragma once

#include "VMEComponentView.h"
#include "VMECore.h"

class VMEComponentMeshBuilder
{
public:
    //构造出来的顶点是在本地空间
    bool BuildMeshFromComponent(const VMEBindingContent::Component &cpt,
                                VMEComponentMesh &mesh) const;
};
