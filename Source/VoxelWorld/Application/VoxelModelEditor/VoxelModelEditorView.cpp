/*================================================================
Filename: VoxelModelEditorView.cpp
Date: 2018.3.14
Created by AirGuanZ
================================================================*/
#include "VoxelModelEditorCore.h"
#include "VoxelModelEditorView.h"

void VMEView::Clear(void)
{
    loadedBindingName_ = "";
}

void VMEView::Refresh(const VMEViewRefreshConfig &config, const VMECore &core)
{
    if(config.all)
    {
        loadedBindingName_ = core.bindingContent.bindingName;
    }
}
