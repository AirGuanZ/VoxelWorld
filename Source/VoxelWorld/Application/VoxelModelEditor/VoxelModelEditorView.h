/*================================================================
Filename: VoxelModelEditorView.h
Date: 2018.3.14
Created by AirGuanZ
================================================================*/
#pragma once

#include <string>

class VMECore;

struct VMEViewRefreshConfig
{
    bool all = false;
};

class VMEView
{
public:
    void Clear(void);
    void Refresh(const VMEViewRefreshConfig &config, const VMECore &core);

private:
    std::string loadedBindingName_;
};
