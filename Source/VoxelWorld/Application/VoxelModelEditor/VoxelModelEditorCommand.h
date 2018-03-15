/*================================================================
Filename: VoxelModelEditorCommand.h
Date: 2018.3.14
Created by AirGuanZ
================================================================*/
#pragma once

class VMEConsole;
class VMECore;
struct VMEViewRefreshConfig;

class VMECmd
{
public:
    virtual void Execute(VMECore &core, VMEViewRefreshConfig &refresh, VMEConsole &console) = 0;
};

class VMECmd_LoadBinding : public VMECmd
{
public:
    void Execute(VMECore &core, VMEViewRefreshConfig &refresh, VMEConsole &console);
};

class VMECmd_UnloadBinding : public VMECmd
{
public:
    void Execute(VMECore &core, VMEViewRefreshConfig &refresh, VMEConsole &console);
};
