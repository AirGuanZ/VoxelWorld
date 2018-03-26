/*================================================================
Filename: Application/Common.cpp
Date: 2018.2.27
Created by AirGuanZ
================================================================*/
#include <Window\Window.h>

#include "Common.h"

void ShowErrMsg(const std::string &errMsg)
{
    Window::GetInstance().ErrMsgBox(errMsg);
}
