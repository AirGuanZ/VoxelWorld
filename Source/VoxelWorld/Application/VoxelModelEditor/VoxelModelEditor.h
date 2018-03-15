/*================================================================
Filename: VoxelModelEditor.h
Date: 2018.3.14
Created by AirGuanZ
================================================================*/
#pragma once

#include <Application\Common.h>
#include "VoxelModelEditorCommand.h"
#include "VoxelModelEditorConsole.h"
#include "VoxelModelEditorCore.h"
#include "VoxelModelEditorView.h"

class VoxelModelEditor
{
public:
    AppState Run(void);

private:
    VMECore core_;
    VMEView view_;
};
