/*================================================================
Filename: VoxelModelEditorComponentView.h
Date: 2018.3.19
Created by AirGuanZ
================================================================*/
#pragma once

#include <map>
#include <queue>

#include <Utility\Math.h>

#include <D3DObject\InputLayout.h>
#include <D3DObject\SingleBufferMesh.h>
#include "VoxelModelEditorCommand.h"
#include "VoxelModelEditorCore.h"

class VMEViewRefreshConfig;

struct VMEComponentVertex
{
    static InputLayout CreateInputLayout(const void *shaderByteCode, int length);

    Vector3 pos;    // POSITION
    Vector3 nor;    // NORMAL
    Vector3 color;  // COLOR
};

using VMEComponentMesh = SingleBufferMesh<VMEComponentVertex>;

class VMEComponentView
{
public:
    VMEComponentView(void);

    void SetAnimation(const std::string &ani, bool loop);

    void Start(void);
    void Pause(void);
    void Stop(void);

    void RebuildMeshFrom(const VMEBindingContent::Component &cpt);

    void Display(std::queue<VMECmd*> &cmds, float dT);

    void Refresh(const VMEViewRefreshConfig &refresh, const VMECore &core);

private:
    struct ComponentMeshRec
    {
        VMEComponentMesh mesh;
        int boneIndex;
    };

    //component name -> component mesh
    std::map<std::string, ComponentMeshRec> meshes_;
    Skeleton::Skeleton *skeleton_;
};
