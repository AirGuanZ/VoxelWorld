/*================================================================
Filename: VoxelModelEditorCommand.cpp
Date: 2018.3.10
Created by AirGuanZ
================================================================*/
#include <filesystem>

#include <SkeletonAnimation/SkeletonDataLoader.h>
#include <Utility/HelperFunctions.h>

#include <Resource/ResourceNameManager.h>
#include "VoxelModelBinding.h"
#include "VoxelModelEditorCommand.h"
#include "VoxelModelEditorCommandWindow.h"
#include "VoxelModelEditorCore.h"

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

void VMECmd_ExitClicked::Execute(VoxelModelEditorCore &core, VMECmdMsgQueue &cmdMsgs)
{
    core.mainLoopDone_ = true;
}

VMECmd_ReloadBindingNames::VMECmd_ReloadBindingNames(bool showMsg)
{
    showMsg_ = showMsg;
}

void VMECmd_ReloadBindingNames::Execute(VoxelModelEditorCore &core, VMECmdMsgQueue &cmdMsgs)
{
    VMWCmd_UnloadBinding(false).Execute(core, cmdMsgs);

    core.bindingNames_.clear();

    auto &rM = RscNameMgr::GetInstance();
    std::map<std::string, std::wstring> filenames;
    Helper::GetFilenames(
        rM.AsString("VoxelModelEditor", "Binding"),
        rM.AsString("VoxelModelEditor", "BindingExtension"),
        filenames);
    for(auto &it : filenames)
        core.bindingNames_.push_back(it.first);

    if(showMsg_)
        cmdMsgs.push({ VMECmdMsg::NORMAL_COLOR, "Binding list reloaded" });
    core.needRefreshDisplay_ = true;

    VMWCmd_SelectBindingName(0).Execute(core, cmdMsgs);
}

VMWCmd_SelectBindingName::VMWCmd_SelectBindingName(int selected)
    : selected_(selected)
{

}

void VMWCmd_SelectBindingName::Execute(VoxelModelEditorCore &core, VMECmdMsgQueue &cmdMsgs)
{
    core.needRefreshDisplay_ = true;
    core.selectedBindingNameIndex_ = selected_;
}

void VMWCmd_DeleteSelectedBinding::Execute(VoxelModelEditorCore &core, VMECmdMsgQueue &cmdMsgs)
{
    if(core.bindingNames_.empty())
        return;

    core.needRefreshDisplay_ = true;

    if(core.model_ && core.bindingNames_[core.selectedBindingNameIndex_] ==
                      core.model_->GetName())
    {
        VMWCmd_UnloadBinding(false).Execute(core, cmdMsgs);
    }

    std::string filename = ConstructBindingPath(
        core.bindingNames_[core.selectedBindingNameIndex_]);
    filesystem::remove(filename);
    cmdMsgs.push({ VMECmdMsg::NORMAL_COLOR, "Binding file deleted: " + filename });

    VMECmd_ReloadBindingNames(false).Execute(core, cmdMsgs);
}

void VMWCmd_LoadSelectedBinding::Execute(VoxelModelEditorCore &core, VMECmdMsgQueue &cmdMsgs)
{
    if(core.model_)
        VMWCmd_UnloadBinding(false).Execute(core, cmdMsgs);
    if(core.bindingNames_.empty())
        return;

    core.model_ = std::make_unique<VoxelModelBinding>();
    std::string filename = ConstructBindingPath(core.bindingNames_[core.selectedBindingNameIndex_]);
    if(!core.model_->LoadFromFile(Helper::ToWStr(filename)))
    {
        core.model_.reset();
        cmdMsgs.push({ VMECmdMsg::ERROR_COLOR, "Failed to load selected binding file: " + filename });
    }
    else
        cmdMsgs.push({ VMECmdMsg::NORMAL_COLOR, "Binding file loaded: " + filename });

    core.needRefreshDisplay_ = true;
}

VMWCmd_UnloadBinding::VMWCmd_UnloadBinding(bool showMsg)
{
    showMsg_ = showMsg;
}

void VMWCmd_UnloadBinding::Execute(VoxelModelEditorCore &core, VMECmdMsgQueue &cmdMsgs)
{
    if(core.model_)
    {
        if(showMsg_)
        {
            cmdMsgs.push({ VMECmdMsg::NORMAL_COLOR,
                "Binding file unloaded: " + ConstructBindingPath(core.model_->GetName()) });
        }
        core.model_.reset();
        core.needRefreshDisplay_ = true;
    }
}

VMWCmd_CreateBinding::VMWCmd_CreateBinding(const std::string &name)
    : name_(name)
{

}

void VMWCmd_CreateBinding::Execute(VoxelModelEditorCore &core, VMECmdMsgQueue &cmdMsgs)
{
    std::string filename = ConstructBindingPath(name_);
    if(!VoxelModelBinding::CreateEmptyBindingFile(Helper::ToWStr(filename)))
    {
        cmdMsgs.push({ VMECmdMsg::ERROR_COLOR, "Failed to create empty binding file: " + filename });
    }
    else
    {
        cmdMsgs.push({ VMECmdMsg::NORMAL_COLOR, "Empty binding file created: " + filename });
    }

    VMECmd_ReloadBindingNames(false).Execute(core, cmdMsgs);
}
