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
    //��������Ķ������ڱ��ؿռ�
    bool BuildMeshFromComponent(const VMEBindingContent::Component &cpt,
                                VMEComponentMesh &mesh) const;
};
