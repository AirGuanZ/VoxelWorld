/*================================================================
Filename: Application/Common.cpp
Date: 2018.2.27
Created by AirGuanZ
================================================================*/
#include <Windows.h>

#include "Common.h"

void ShowErrMsgBox(const std::string &errMsg)
{
    MessageBoxA(NULL, errMsg.c_str(), u8"Error", MB_ICONERROR | MB_OK);
}
