/*================================================================
Filename: VoxelModelEditorComponentView.cpp
Date: 2018.3.19
Created by AirGuanZ
================================================================*/
#include <cassert>

#include <Utility\HelperFunctions.h>

#include <Screen\GUISystem.h>
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
    //�����������ģ�ͣ���cpt�����±�־��Ϊ�棬��˵��ģ���������
    auto it = meshes_.find(cpt.componentName);
    if(it != meshes_.end() && !cpt.meshNeedUpdating)
        return;

    it->second.reset(BuildMeshFromComponent(cpt));
}

void VMEComponentView::Display(std::queue<VMECmd*> &cmds, float dT)
{
    //��������ʱ�����
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

    //���frame buffer�Ƿ���Ҫ��ʼ��
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

    aniFrameBuf_.ClearRenderTargetView(1.0f, 0.0f, 0.0f, 1.0f);
    ImGui::Image((ImTextureID)aniFrameBuf_.GetSRV(), { 200, 300 });

    ImGui::Text("time: %f", aniTime_);

    ImGui::End();
}

void VMEComponentView::Refresh(const VMEViewRefreshConfig &refresh, const VMECore &core)
{
    //TODO
}

VMEComponentView::ComponentMeshRec *VMEComponentView::BuildMeshFromComponent(const VMEBindingContent::Component &cpt)
{
    //TODO
    return nullptr;
}
