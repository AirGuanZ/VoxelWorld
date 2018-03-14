/*================================================================
Filename: VoxelModelBindingDisplay.h
Date: 2018.3.13
Created by AirGuanZ
================================================================*/
#pragma once

#include <string>

class VoxelModelBindingDisplay
{
public:
    void Clear(void);

    void Display(void);

private:
    friend class VoxelModelBinding;

    std::string bindingName_;
};
