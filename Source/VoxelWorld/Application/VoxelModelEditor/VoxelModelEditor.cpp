/*================================================================
Filename: VoxelModelEditor.cpp
Date: 2018.3.5
Created by AirGuanZ
================================================================*/
#include <cassert>
#include <filesystem>
#include <map>

#include <Utility/HelperFunctions.h>

#include "VoxelModelEditor.h"
#include "VoxelModelEditorCommand.h"

VoxelModelEditor::VoxelModelEditor(void)
    : VoxelModelEditorCore(), view_(cmdQueue_)
{

}

AppState VoxelModelEditor::Run(void)
{
    cmdQueue_.push_back(new VMECmd_ReloadBindingNames());
    while(!mainLoopDone_)
    {
        view_.Display();

        while(cmdQueue_.size())
        {
            auto *cmd = cmdQueue_.front();
            cmdQueue_.pop_front();
            cmd->Execute(*this, cmdQueue_);
            Helper::SafeDeleteObjects(cmd);
        }

        if(needRefreshDisplay_)
            RefreshDisplay();
    }

    return AppState::MainMenu;
}

void VoxelModelEditor::RefreshDisplay(void)
{
    view_.bindingNames_.clear();
    for(auto &it : bindingNames_)
        view_.bindingNames_.push_back(it.data());
    view_.selectedBindingNameIndex_ = selectedBindingNameIndex_;
}
