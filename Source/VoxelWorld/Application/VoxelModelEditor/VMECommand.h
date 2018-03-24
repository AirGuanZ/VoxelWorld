/*================================================================
Filename: VMECommand.h
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

class VMECmd_SaveBinding : public VMECmd
{
public:
    void Execute(VMECore &core, VMEViewRefreshConfig &refresh, VMEConsole &console);
};

class VMECmd_SaveBindingAs : public VMECmd
{
public:
    void Execute(VMECore &core, VMEViewRefreshConfig &refresh, VMEConsole &console);
};

class VMECmd_NewEmptyBinding : public VMECmd
{
public:
    void Execute(VMECore &core, VMEViewRefreshConfig &refresh, VMEConsole &console);
};

class VMECmd_BindingSkeletonTimeFactor : public VMECmd
{
public:
    VMECmd_BindingSkeletonTimeFactor(float timeFactor);

    void Execute(VMECore &core, VMEViewRefreshConfig &refresh, VMEConsole &console);

private:
    float timeFactor_;
};

class VMECmd_BindingSkeletonSizeFactor : public VMECmd
{
public:
    VMECmd_BindingSkeletonSizeFactor(float sizeFactor);

    void Execute(VMECore &core, VMEViewRefreshConfig &refresh, VMEConsole &console);

private:
    float sizeFactor_;
};
