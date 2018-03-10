/*================================================================
Filename: VoxelModelEditorCommand.cpp
Date: 2018.3.10
Created by AirGuanZ
================================================================*/
#include <filesystem>

#include <Resource/ResourceNameManager.h>
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

void VMECmd_ExitClicked::Execute(VoxelModelEditorCore &core, VMECmdQueue &cmdQueue)
{
    core.mainLoopDone_ = true;
}

void VMECmd_ReloadBindingNames::Execute(VoxelModelEditorCore &core, VMECmdQueue &cmdQueue)
{
    core.bindingNames_.clear();
    core.selectedBindingNameIndex_ = 0;

    auto &rM = RscNameMgr::GetInstance();
    std::map<std::string, std::wstring> filenames;
    Helper::GetFilenames(
        rM.AsString("VoxelModelEditor", "Binding"),
        rM.AsString("VoxelModelEditor", "BindingExtension"),
        filenames);
    for(auto &it : filenames)
        core.bindingNames_.push_back(it.first);

    core.needRefreshDisplay_ = true;
}

VMWCmd_SelectBindingName::VMWCmd_SelectBindingName(int selected)
    : selected_(selected)
{

}

void VMWCmd_SelectBindingName::Execute(VoxelModelEditorCore &core, VMECmdQueue &cmdQueue)
{
    core.selectedBindingNameIndex_ = selected_;
    core.needRefreshDisplay_ = true;
}

void VMWCmd_DeleteSelectedBinding::Execute(VoxelModelEditorCore &core, VMECmdQueue &cmdQueue)
{
    auto &rM = RscNameMgr::GetInstance();

    std::string filename = ConstructBindingPath(
        core.bindingNames_[core.selectedBindingNameIndex_]);
    filesystem::remove(std::move(filename));

    VMECmd_ReloadBindingNames().Execute(core, cmdQueue);
}
