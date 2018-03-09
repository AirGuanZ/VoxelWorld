/*================================================================
Filename: VoxelModelEditor.cpp
Date: 2018.3.5
Created by AirGuanZ
================================================================*/
#include <cassert>
#include <map>

#include <Utility/HelperFunctions.h>

#include <Resource/ResourceNameManager.h>
#include "VoxelModelEditor.h"

namespace
{
    class VMECmd_ExitClicked : public VoxelModelEditorCommand
    {
    public:
        void Execute(VoxelModelEditorCore &core)
        {
            core.mainLoopDone_ = true;
        }
    };

    class VMECmd_ReloadBindingNames : public VoxelModelEditorCommand
    {
    public:
        void Execute(VoxelModelEditorCore &core)
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
    };
}

bool VoxelModelEditorCore::Initialize(void)
{
    mainLoopDone_ = false;
    needRefreshDisplay_ = false;

    return true;
}

AppState VoxelModelEditor::Run(void)
{
    if(!VoxelModelEditorCore::Initialize())
        return AppState::MainMenu;

    cmdQueue_.push(new VMECmd_ReloadBindingNames());
    while(!mainLoopDone_)
    {
        view_.Render();

        while(cmdQueue_.size())
        {
            auto *cmd = cmdQueue_.front();
            cmdQueue_.pop();
            cmd->Execute(*this);
            Helper::SafeDeleteObjects(cmd);
        }

        if(needRefreshDisplay_)
            RefreshDisplay();
    }

    return AppState::MainMenu;
}
