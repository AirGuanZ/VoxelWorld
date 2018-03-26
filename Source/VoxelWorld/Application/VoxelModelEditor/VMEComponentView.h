/*================================================================
Filename: VoxelModelEditorComponentView.h
Date: 2018.3.19
Created by AirGuanZ
================================================================*/
#pragma once

#include <map>
#include <memory>
#include <queue>

#include <OWEShader.hpp>
#include <Utility\Math.h>

#include <D3DObject\FrameBuffer.h>
#include <D3DObject\InputLayout.h>
#include <D3DObject\SingleBufferMesh.h>
#include "VMECommand.h"
#include "VMECore.h"

struct VMEViewRefreshConfig;

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

    bool Initialize(std::string &errMsg);

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

    struct VSCBTrans
    {
        Matrix WVP;
    };

    static ComponentMeshRec *BuildMeshFromComponent(const VMEBindingContent::Component &cpt);

    void RenderComponentPreview(void);

private:
    //component name -> component mesh
    std::map<std::string, std::unique_ptr<ComponentMeshRec>> meshes_;
    Skeleton::Skeleton *skeleton_;

    //动画相关
    std::string curAniName_;
    bool aniLoop_;
    float aniTime_;
    bool aniDisplaying_;

    //用于渲染绑定后的骨骼动画的frame buffer
    BasicFrameBuffer aniFrameBuf_;

    //着色器相关
    OWE::Shader<SS_VS, SS_PS> shader_;
    std::unique_ptr<OWE::ShaderUniforms<SS_VS, SS_PS>> uniforms_;
    InputLayout inputLayout_;
};
