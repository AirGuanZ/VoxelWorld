/*================================================================
Filename: VoxelModelEditorView.h
Date: 2018.3.14
Created by AirGuanZ
================================================================*/
#pragma once

#include <string>

struct VMEViewRefreshConfig
{
    bool all = false;
};

class VMEView
{
public:
    void Clear(void);
    void Refresh(void);

private:
    std::string loadedBindingName_;
};
