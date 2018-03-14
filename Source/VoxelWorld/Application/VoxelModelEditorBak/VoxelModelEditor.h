/*================================================================
Filename: VoxelModelEditor.h
Date: 2018.3.4
Created by AirGuanZ
================================================================*/
#pragma once

#include <deque>
#include <map>
#include <string>
#include <vector>

#include <Application/Common.h>
#include "VoxelModelEditorCore.h"
#include "VoxelModelEditorDisplay.h"

class VoxelModelEditor : private VoxelModelEditorCore
{
public:
    VoxelModelEditor(void);

    AppState Run(void);

private:
    void RefreshDisplay(void);

private:
    VoxelModelEditorDisplay display_;

    VMECmdQueue cmdQueue_;
};
