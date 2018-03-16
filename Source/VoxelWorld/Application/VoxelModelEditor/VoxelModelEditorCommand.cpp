/*================================================================
Filename: VoxelModelEditorCommand.cpp
Date: 2018.3.14
Created by AirGuanZ
================================================================*/
#include <Windows.h>

#include <Utility\FileSystem.h>
#include <Utility\HelperFunctions.h>

#include <Resource\ResourceNameManager.h>
#include <Window\Window.h>
#include "VoxelModelEditorCommand.h"
#include "VoxelModelEditorConsole.h"
#include "VoxelModelEditorCore.h"
#include "VoxelModelEditorView.h"

void VMECmd_LoadBinding::Execute(VMECore &core, VMEViewRefreshConfig &refresh, VMEConsole &console)
{
    auto &rM = RscNameMgr::GetInstance();

    std::string filename;
    if(!Window::GetInstance().OpenFileName(rM.AsString("VoxelModelEditor", "Binding"), filename))
        return;
    filename = FileSystem::RelativePath(filename);

    refresh.all = true;
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
        console.AddText(VMEConsoleText::Normal, "Binding cleared");
        refresh.all = true;
    }
    else
        console.AddText(VMEConsoleText::Error, "No available binding");
}
