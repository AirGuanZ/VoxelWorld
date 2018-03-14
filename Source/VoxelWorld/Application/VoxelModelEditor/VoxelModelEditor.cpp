/*================================================================
Filename: VoxelModelEditor.cpp
Date: 2018.3.14
Created by AirGuanZ
================================================================*/
#include "VoxelModelEditor.h"

#include <Window\Window.h>
#include <iostream>

AppState VoxelModelEditor::Run(void)
{
    std::string filename;
    if(Window::GetInstance().OpenFileName("Bin\\Model\\Voxel\\Binding\\", filename))
        std::cout << filename << std::endl;

    return AppState::MainMenu;
}
