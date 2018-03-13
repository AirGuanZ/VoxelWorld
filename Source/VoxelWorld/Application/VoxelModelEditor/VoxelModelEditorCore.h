/*================================================================
Filename: VoxelModelEditorCore.h
Date: 2018.3.10
Created by AirGuanZ
================================================================*/
#pragma once

#include <deque>
#include <map>
#include <memory>
#include <queue>
#include <string>
#include <vector>

#include <SkeletonAnimation/SkeletonData.h>

#include "VoxelModelBinding.h"

class VMEFatalError
{
public:
    VMEFatalError(const std::string &msg)
        : msg_(msg)
    {

    }

    const char *What(void) const
    {
        return msg_.c_str();
    }

private:
    std::string msg_;
};

class VoxelModelEditorCore
{
public:
    VoxelModelEditorCore(void);
    ~VoxelModelEditorCore(void);

    int selectedBindingNameIndex_;
    std::vector<std::string> bindingNames_;

    bool mainLoopDone_;
    bool needRefreshDisplay_;

    bool modelChanged_;
    std::unique_ptr<VoxelModelBinding> model_;
};
