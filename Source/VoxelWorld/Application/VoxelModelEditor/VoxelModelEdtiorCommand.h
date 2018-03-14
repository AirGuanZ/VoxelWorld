/*================================================================
Filename: VoxelModelEditorCommand.h
Date: 2018.3.14
Created by AirGuanZ
================================================================*/
#pragma once

class VMECore;
struct VMEViewRefreshConfig;

class VMECmd
{
public:
    virtual void Execute(VMECore &core, VMEViewRefreshConfig &refresh) = 0;
};
