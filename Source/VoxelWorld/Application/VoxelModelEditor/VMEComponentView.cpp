/*================================================================
Filename: VMEComponentView.cpp
Date: 2018.3.19
Created by AirGuanZ
================================================================*/
#include <cassert>

#include <Utility\HelperFunctions.h>

#include <Screen\GUISystem.h>
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

    //检查frame buffer是否需要初始化
    if(!aniFrameBuf_.IsAvailable())
    {
        if(!aniFrameBuf_.Initialize(200, 300))
            return;
    }

    if(!ImGui::Begin("Preview##VoxelModelEditor", nullptr,
                     ImGuiWindowFlags_AlwaysAutoResize))
    {
        ImGui::End();
        return;
    }

    aniFrameBuf_.ClearRenderTargetView(0.35f, 0.35f, 0.35f, 0.4f);
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
            if(cpt.meshNeedUpdating)
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
    //TODO
    return nullptr;
}
