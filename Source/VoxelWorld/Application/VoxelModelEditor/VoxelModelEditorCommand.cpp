/*================================================================
Filename: VoxelModelEditorCommand.cpp
Date: 2018.3.10
Created by AirGuanZ
================================================================*/
#include <filesystem>

#include <SkeletonAnimation/SkeletonDataLoader.h>
#include <Utility/HelperFunctions.h>

#include <Resource/ResourceNameManager.h>
#include <VoxelModel/ModelSkeletonBinding.h>
#include "VoxelModelEditorCommand.h"

namespace filesystem = std::experimental::filesystem::v1;

namespace
{
    inline std::string ConstructBindingPath(const std::string &name)
    {
        return RscNameMgr::GetInstance().AsString("VoxelModelEditor", "Binding") + name +
               RscNameMgr::GetInstance().AsString("VoxelModelEditor", "BindingExtension");
    }

    inline std::string ConstructComponentPath(const std::string &name)
    {
        return RscNameMgr::GetInstance().AsString("VoxelModelEditor", "Component") + name +
               RscNameMgr::GetInstance().AsString("VoxelModelEditor", "ComponentExtension");
    }

    inline std::string ConstructSkeletonPath(const std::string &name)
    {
        return RscNameMgr::GetInstance().AsString("VoxelModelEditor", "Skeleton") + name +
               RscNameMgr::GetInstance().AsString("VoxelModelEditor", "SkeletonExtension");
    }
}

void VMECmd_ExitClicked::Execute(VoxelModelEditorCore &core)
{
    core.mainLoopDone_ = true;
}

void VMECmd_ReloadBindingNames::Execute(VoxelModelEditorCore &core)
{
    VMWCmd_UnloadBinding().Execute(core);

    core.bindingNames_.clear();

    auto &rM = RscNameMgr::GetInstance();
    std::map<std::string, std::wstring> filenames;
    Helper::GetFilenames(
        rM.AsString("VoxelModelEditor", "Binding"),
        rM.AsString("VoxelModelEditor", "BindingExtension"),
        filenames);
    for(auto &it : filenames)
        core.bindingNames_.push_back(it.first);

    core.needRefreshDisplay_ = true;

    VMWCmd_SelectBindingName(0).Execute(core);
}

VMWCmd_SelectBindingName::VMWCmd_SelectBindingName(int selected)
    : selected_(selected)
{

}

void VMWCmd_SelectBindingName::Execute(VoxelModelEditorCore &core)
{
    core.needRefreshDisplay_ = true;
    core.selectedBindingNameIndex_ = selected_;
}

void VMWCmd_DeleteSelectedBinding::Execute(VoxelModelEditorCore &core)
{
    if(core.bindingNames_.empty())
        return;

    core.needRefreshDisplay_ = true;

    if(core.model_ && core.bindingNames_[core.selectedBindingNameIndex_] ==
                      core.model_->GetName())
    {
        VMWCmd_UnloadBinding().Execute(core);
    }

    std::string filename = ConstructBindingPath(
        core.bindingNames_[core.selectedBindingNameIndex_]);
    filesystem::remove(std::move(filename));

    VMECmd_ReloadBindingNames().Execute(core);
}

void VMWCmd_LoadSelectedBinding::Execute(VoxelModelEditorCore &core)
{
    if(core.model_)
        VMWCmd_UnloadBinding().Execute(core);
    if(core.bindingNames_.empty())
        return;

    core.model_ = std::make_unique<VoxelModelBinding>();
    core.model_->LoadFromFile(Helper::ToWStr(ConstructBindingPath(
        core.bindingNames_[core.selectedBindingNameIndex_])));

    core.needRefreshDisplay_ = true;
}

void VMWCmd_UnloadBinding::Execute(VoxelModelEditorCore &core)
{
    if(core.model_)
    {
        core.model_.reset();
        core.needRefreshDisplay_ = true;
    }
}

VMWCmd_CreateBinding::VMWCmd_CreateBinding(const std::string &name)
    : name_(name)
{

}

void VMWCmd_CreateBinding::Execute(VoxelModelEditorCore &core)
{
    std::string filename = ConstructBindingPath(name_);
    VoxelModelBinding::CreateEmptyBindingFile(Helper::ToWStr(filename));

    VMECmd_ReloadBindingNames().Execute(core);
}
