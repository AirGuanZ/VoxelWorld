/*================================================================
Filename: VMECommand.cpp
Date: 2018.3.14
Created by AirGuanZ
================================================================*/
#include <cassert>

#include <Utility\FileSystem.h>
#include <Utility\HelperFunctions.h>

#include <Resource\ResourceNameManager.h>
#include <Window\Window.h>
#include "VMECommand.h"
#include "VMEConsole.h"
#include "VMECore.h"
#include "VMEView.h"

void VMECmd_LoadBinding::Execute(VMECore &core, VMEViewRefreshConfig &refresh, VMEConsole &console)
{
    auto &rM = RscNameMgr::GetInstance();

    std::string filename;
    if(!Window::GetInstance().OpenFileName(rM.AsString("VoxelModelEditor", "Binding"), filename))
        return;

    refresh.bindingFile = true;
    if(core.bindingContent.LoadFromFile(filename))
        console.AddText(VMEConsoleText::Normal, "Binding file loaded: " + filename);
    else
        console.AddText(VMEConsoleText::Error, "Failed to load binding file: " + filename);
}

void VMECmd_UnloadBinding::Execute(VMECore &core, VMEViewRefreshConfig &refresh, VMEConsole &console)
{
    if(core.bindingContent.IsAvailable())
    {
        core.bindingContent.Clear();
        console.AddText(VMEConsoleText::Normal, "Binding unloaded");
        refresh.bindingFile = true;
    }
    else
        console.AddText(VMEConsoleText::Error, "No available binding");
}

void VMECmd_SaveBinding::Execute(VMECore &core, VMEViewRefreshConfig &refresh, VMEConsole &console)
{
    if(!core.bindingContent.IsAvailable())
    {
        console.AddText(VMEConsoleText::Error, "No available binding");
        return;
    }

    if(core.bindingContent.SaveToFile(core.bindingContent.bindingPath))
    {
        console.AddText(VMEConsoleText::Normal,
            "Binding saved: " + core.bindingContent.bindingPath);
    }
    else
    {
        console.AddText(VMEConsoleText::Error,
            "Failed to save binding: " + core.bindingContent.bindingPath);
    }
}

void VMECmd_SaveBindingAs::Execute(VMECore &core, VMEViewRefreshConfig &refresh, VMEConsole &console)
{
    auto &rM = RscNameMgr::GetInstance();

    if(!core.bindingContent.IsAvailable())
    {
        console.AddText(VMEConsoleText::Error, "No available binding");
        return;
    }

    std::string filename;
    if(!Window::GetInstance().SaveFileName(rM.AsString("VoxelModelEditor", "Binding"), filename))
        return;

    if(core.bindingContent.SaveToFile(filename))
    {
        core.bindingContent.bindingPath = filename;
        refresh.bindingFile = true;
        console.AddText(VMEConsoleText::Normal,
            "Binding saved: " + core.bindingContent.bindingPath);
    }
    else
    {
        console.AddText(VMEConsoleText::Error,
            "Failed to save binding: " + core.bindingContent.bindingPath);
    }
}

void VMECmd_NewEmptyBinding::Execute(VMECore &core, VMEViewRefreshConfig &refresh, VMEConsole &console)
{
    auto &rM = RscNameMgr::GetInstance();

    std::string filename;
    if(!Window::GetInstance().SaveFileName(rM.AsString("VoxelModelEditor", "Binding"), filename))
        return;

    std::string skeletonName;
    if(!Window::GetInstance().OpenFileName(rM.AsString("VoxelModelEditor", "Skeleton"), skeletonName))
        return;
    skeletonName = rM.AsString("VoxelModelEditor", "Skeleton")
                 + FileSystem::PathToFilename(skeletonName);

    VMEBindingContent newBindingContent;
    newBindingContent.bindingPath = filename;
    newBindingContent.skeletonPath = skeletonName;

    if(newBindingContent.SaveToFile(filename))
    {
        console.AddText(VMEConsoleText::Normal,
            "New binding created: " + filename);
    }
    else
    {
        console.AddText(VMEConsoleText::Error,
            "Failed to create new binding: " + filename);
    }
}

VMECmd_BindingSkeletonTimeFactor::VMECmd_BindingSkeletonTimeFactor(float timeFactor)
{
    assert(timeFactor > 0.0f);
    timeFactor_ = timeFactor;
}

void VMECmd_BindingSkeletonTimeFactor::Execute(VMECore &core, VMEViewRefreshConfig &refresh, VMEConsole &console)
{
    core.bindingContent.skeletonTimeFactor = timeFactor_;
    core.bindingContent.RefreshScaledSkeleton();

    refresh.bindingFile = true;

    console.AddText(VMEConsoleText::Normal,
        "Skeleton time factor refreshed: " + std::to_string(timeFactor_));
}

VMECmd_BindingSkeletonSizeFactor::VMECmd_BindingSkeletonSizeFactor(float sizeFactor)
{
    assert(sizeFactor > 0.0f);
    sizeFactor_ = sizeFactor;
}

void VMECmd_BindingSkeletonSizeFactor::Execute(VMECore &core, VMEViewRefreshConfig &refresh, VMEConsole &console)
{
    core.bindingContent.skeletonSizeFactor = sizeFactor_;
    core.bindingContent.RefreshScaledSkeleton();

    refresh.bindingFile = true;

    console.AddText(VMEConsoleText::Normal,
        "Skeleton size factor refreshed: " + std::to_string(sizeFactor_));
}

VMECmd_NewComponent::VMECmd_NewComponent(const std::string &componentName, const std::string &boneName)
    : componentName_(componentName), boneName_(boneName)
{

}

void VMECmd_NewComponent::Execute(VMECore &core, VMEViewRefreshConfig &refresh, VMEConsole &console)
{
    if(componentName_.empty())
    {
        console.AddText(VMEConsoleText::Error, "Empty component name");
        return;
    }

    if(boneName_.empty())
    {
        console.AddText(VMEConsoleText::Error, "Invalid bone name");
        return;
    }

    //ÓÐÃû×ÖÖØ¸´Âð£¿
    for(auto &cpt : core.bindingContent.components)
    {
        if(cpt.componentName == componentName_)
        {
            console.AddText(VMEConsoleText::Error, "Component name repeated");
            return;
        }
    }

    int boneIdx = core.bindingContent.originSkeleton.GetBoneIndex(boneName_);
    if(boneIdx < 0)
    {
        console.AddText(VMEConsoleText::Error, "Invalid bone name");
        return;
    }

    auto &cpt = core.bindingContent.components.emplace_back();
    cpt.componentName = componentName_;
    cpt.boneName = boneName_;
    cpt.boneIndex = boneIdx;

    //cpt.boundPosX = 1;
    //cpt.boundPosY = 1;
    //cpt.boundPosZ = 1;
    cpt.voxelSize = 1.0f;
    cpt.voxels[{ 0, 0, 0 }] = { 255, 0, 0 };
    cpt.meshNeedUpdating = true;
    
    console.AddText(VMEConsoleText::Normal, "New component created: " + componentName_);
    refresh.componentList = true;
}
