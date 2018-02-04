/*================================================================
Filename: Main.cpp
Date: 2018.1.12
Created by AirGuanZ
================================================================*/
#include <atomic>
#include <iostream>
#include <stdexcept>

#ifdef _DEBUG
    #include <crtdbg.h>
#endif

#include "Application\Application.h"

#include "Land\V1\Biome.h"
#include <Windows.h>

int main(void)
{
#ifdef _DEBUG
    _CrtSetDbgFlag(_CrtSetDbgFlag(_CRTDBG_REPORT_FLAG) | _CRTDBG_LEAK_CHECK_DF);
#endif

    try
    {
        Application app;
        app.Run();
    }
    catch(const std::exception &err)
    {
        std::cerr << err.what();
    }
}
