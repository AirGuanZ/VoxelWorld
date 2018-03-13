/*================================================================
Filename: VoxelModelEditor.cpp
Date: 2018.3.5
Created by AirGuanZ
================================================================*/
#include <cassert>
#include <filesystem>
#include <map>

#include <Utility/HelperFunctions.h>

#include "VoxelModelBinding.h"
#include "VoxelModelEditor.h"
#include "VoxelModelEditorCommand.h"
#include "VoxelModelEditorCommandWindow.h"

VoxelModelEditor::VoxelModelEditor(void)
    : VoxelModelEditorCore(), display_(cmdQueue_)
{

}

AppState VoxelModelEditor::Run(void)
{
    try
    {
        cmdQueue_.push_back(new VMECmd_ReloadBindingNames());
        while(!mainLoopDone_)
        {
            display_.Display();

            VMECmdMsgQueue cmdMsgQueue;
            while(cmdQueue_.size())
            {
                auto *cmd = cmdQueue_.front();
                cmdQueue_.pop_front();
                cmd->Execute(*this, cmdMsgQueue);
                Helper::SafeDeleteObjects(cmd);
            }

            while(cmdMsgQueue.size())
            {
                VMECmdMsg msg = cmdMsgQueue.front();
                cmdMsgQueue.pop();
                display_.cmdWinDisplay_.AddText(msg.msg, msg.color);
            }

            if(needRefreshDisplay_)
                RefreshDisplay();
        }
    }
    catch(const VMEFatalError &err)
    {
        ShowErrMsgBox(err.What());
    }

    return AppState::MainMenu;
}

void VoxelModelEditor::RefreshDisplay(void)
{
    display_.bindingNames_.clear();
    for(auto &it : bindingNames_)
        display_.bindingNames_.push_back(it.data());
    display_.selectedBindingNameIndex_ = selectedBindingNameIndex_;

    if(model_)
        model_->RefreshDisplay(display_.bindingDisplay_);
    else
        display_.bindingDisplay_.Clear();
}
