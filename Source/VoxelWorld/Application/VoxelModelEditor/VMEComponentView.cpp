/*================================================================
Filename: VMEComponentView.cpp
Date: 2018.3.19
Created by AirGuanZ
================================================================*/
#include <cassert>

#include <Utility\HelperFunctions.h>
#include <Utility\FileSystem.h>

#include <Resource\ResourceNameManager.h>
#include <Screen\GUISystem.h>
#include <Window\Window.h>
#include "VMEComponentMeshBuilder.h"
#include "VMEComponentView.h"
#include "VMEView.h"

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

VMEComponentView::VMEComponentView(void)
{
    skeleton_ = nullptr;

    aniLoop_ = false;
    aniTime_ = 0.0f;
    aniDisplaying_ = false;
}

bool VMEComponentView::Initialize(std::string &errMsg)
{
    auto &rM = RscNameMgr::GetInstance();
    ID3D11Device *dev = Window::GetInstance().GetD3DDevice();

    meshes_.clear();
    skeleton_ = nullptr;

    curAniName_ = "";
    aniLoop_ = false;
    aniTime_ = 0.0f;
    aniDisplaying_ = false;

    if(!aniFrameBuf_.Initialize(200, 300))
    {
        errMsg = "Failed to initialize frame buffer object for"
                 " voxel model editor previewer";
        return false;
    }
    
    std::string VSSrc, PSSrc;
    if(!FileSystem::ReadFile(rM("VoxelModelEditor", "PreviewVertexShader"), VSSrc) ||
       !FileSystem::ReadFile(rM("VoxelModelEditor", "PreviewPixelShader"), PSSrc))
    {
        errMsg = "Failed to load shader source for voxel model editor previewer";
        return false;
    }

    if(!shader_.InitStage<SS_VS>(dev, VSSrc, &errMsg) ||
       !shader_.InitStage<SS_VS>(dev, VSSrc, &errMsg))
    {
        errMsg = "Failed to initialize shader stage for voxel model editor previewer: "
                 + errMsg;
        return false;
    }

    inputLayout_ = VMEComponentVertex::CreateInputLayout(
        shader_.GetShaderByteCodeWithInputSignature(),
        shader_.GetShaderByteCodeSizeWithInputSignature());
    if(!inputLayout_)
    {
        errMsg = "Failed to create input layout for voxel model editor previewer";
        return false;
    }

    uniforms_.reset(shader_.CreateUniformManager());
    if(!uniforms_)
    {
        errMsg = "Failed to create shader uniform manager for "
                 "voxel model editor previewer";
        return false;
    }

    return true;
}

void VMEComponentView::SetAnimation(const std::string &ani, bool loop)
{
    if(!skeleton_ || !skeleton_->GetAniClip(ani))
    {
        curAniName_ = "";
        return;
    }

    curAniName_ = ani;
    aniLoop_ = loop;
    aniTime_ = 0.0f;
    aniDisplaying_ = false;
}

void VMEComponentView::Start(void)
{
    aniDisplaying_ = true;
}

void VMEComponentView::Pause(void)
{
    aniDisplaying_ = false;
}

void VMEComponentView::Stop(void)
{
    aniDisplaying_ = false;
    aniTime_ = 0.0f;
}

void VMEComponentView::RebuildMeshFrom(const VMEBindingContent::Component &cpt)
{
    //如果本来就有模型，且cpt待更新标志不为真，就说明模型无需更新
    auto it = meshes_.find(cpt.componentName);
    if(it != meshes_.end() && !cpt.meshNeedUpdating)
        return;

    it->second.reset(BuildMeshFromComponent(cpt));
}

void VMEComponentView::Display(std::queue<VMECmd*> &cmds, float dT)
{
    //骨骼动画时间更新
    if(curAniName_.size() && aniDisplaying_)
    {
        const Skeleton::AniClip *clip = skeleton_->GetAniClip(curAniName_);
        if(curAniName_.size() && aniDisplaying_)
        {
            aniTime_ += dT;
            if(aniTime_ >= clip->End() && aniLoop_)
            {
                while(aniTime_ >= clip->End())
                    aniTime_ -= (clip->End() - clip->Start());
            }
        }
    }

    assert(aniFrameBuf_.IsAvailable());

    if(!ImGui::Begin("Preview##VoxelModelEditor", nullptr,
                     ImGuiWindowFlags_AlwaysAutoResize))
    {
        ImGui::End();
        return;
    }

    RenderComponentPreview();

    ImGui::Image((ImTextureID)aniFrameBuf_.GetSRV(), { 200, 300 });

    ImGui::Text("Time: %f", aniTime_);

    ImGui::End();
}

void VMEComponentView::Refresh(const VMEViewRefreshConfig &refresh, const VMECore &core)
{
    if(!refresh.componentModel)
        return;

    std::map<std::string, std::unique_ptr<ComponentMeshRec>> newMeshes_;
    for(auto &cpt : core.bindingContent.components)
    {
        auto it = meshes_.find(cpt.componentName);
        if(it != meshes_.end())
        {
            if(cpt.meshNeedUpdating || it->second.get() == nullptr)
            {
                newMeshes_[it->first].reset(BuildMeshFromComponent(cpt));
                cpt.meshNeedUpdating = false;
            }
            else
            {
                newMeshes_[it->first] = std::move(it->second);
            }
        }
        else
        {
            newMeshes_[cpt.componentName].reset(BuildMeshFromComponent(cpt));
            cpt.meshNeedUpdating = false;
        }
    }
    meshes_ = std::move(newMeshes_);
}

VMEComponentView::ComponentMeshRec *VMEComponentView::BuildMeshFromComponent(const VMEBindingContent::Component &cpt)
{
    ComponentMeshRec *rec = new ComponentMeshRec;
    if(!VMEComponentMeshBuilder().BuildMeshFromComponent(cpt, rec->mesh))
    {
        delete rec;
        return nullptr;
    }
    rec->boneIndex = cpt.boneIndex;
    return rec;
}

void VMEComponentView::RenderComponentPreview(void)
{
    ID3D11Device *dev = Window::GetInstance().GetD3DDevice();
    ID3D11DeviceContext *DC = Window::GetInstance().GetD3DDeviceContext();

    aniFrameBuf_.ClearRenderTargetView(0.35f, 0.35f, 0.35f, 0.4f);

    if(curAniName_.empty())
        return;

    std::vector<Matrix> boneTrans;
    if(!skeleton_->GetTransMatrix(curAniName_, aniTime_, boneTrans))
        return;

    aniFrameBuf_.Begin();

    shader_.Bind(DC);
    DC->IASetInputLayout(inputLayout_);

    auto vscbTrans = uniforms_->GetConstantBuffer<SS_VS, VSCBTrans, true>(dev, "Trans");
    Matrix VP = Matrix::CreateLookAt({ 0.0f, 0.0f, 4.0f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 1.0f, 0.0f }) *
                Matrix::CreateOrthographic(200.0f, 300.0f, 0.1f, 100.0f);

    for(auto &mesh : meshes_)
    {
        Matrix WVP = boneTrans[mesh.second->boneIndex] * VP;
        vscbTrans->SetBufferData(DC, { WVP.Transpose() });
        uniforms_->Bind(DC);

        mesh.second->mesh.Bind(DC);
        mesh.second->mesh.Draw(DC);
        mesh.second->mesh.Unbind(DC);
    }

    uniforms_->Unbind(DC);
    DC->IASetInputLayout(nullptr);
    shader_.Unbind(DC);

    aniFrameBuf_.End();
}
