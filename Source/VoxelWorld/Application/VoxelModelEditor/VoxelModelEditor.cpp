/*================================================================
Filename: VoxelModelEditor.cpp
Date: 2018.3.14
Created by AirGuanZ
================================================================*/
#include <Utility\HelperFunctions.h>

#include "VoxelModelEditor.h"

AppState VoxelModelEditor::Run(void)
{
    bool done = false;
    std::queue<VMECmd*> cmds;

    while(!done)
    {
        VMEViewControl viewCtrl;
        VMEViewRefreshConfig refreshConfig;

        view_.Display(viewCtrl, cmds);
        while(cmds.size())
        {
            VMECmd *cmd = cmds.front();
            cmds.pop();
            cmd->Execute(core_, refreshConfig, view_.GetConsole());
            Helper::SafeDeleteObjects(cmd);
        }

        if(viewCtrl.exit)
            done = true;
    }

    return AppState::MainMenu;
}
