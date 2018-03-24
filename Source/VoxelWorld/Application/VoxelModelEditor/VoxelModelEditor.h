/*================================================================
Filename: VoxelModelEditor.h
Date: 2018.3.14
Created by AirGuanZ
================================================================*/
#pragma once

#include <Application\Common.h>
#include "VMECommand.h"
#include "VMEConsole.h"
#include "VMECore.h"
#include "VMEView.h"

class VoxelModelEditor
{
public:
    AppState Run(void);

private:
    VMECore core_;
    VMEView view_;
};
