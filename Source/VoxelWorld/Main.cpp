/*================================================================
Filename: Main.cpp
Date: 2018.1.12
Created by AirGuanZ
================================================================*/
#include <iostream>
#include <stdexcept>

#ifdef _DEBUG
    #include <crtdbg.h>
#endif

#include <Application/Application.h>

int main(void)
{
#ifdef _DEBUG
    _CrtSetDbgFlag(_CrtSetDbgFlag(_CRTDBG_REPORT_FLAG) | _CRTDBG_LEAK_CHECK_DF);
#endif

    try
    {
        Application app;
        
        std::string errMsg;
        if(app.Initialize(errMsg))
        {
            app.Run();
        }
        else
        {
            std::cerr << errMsg << std::endl;
        }
    }
    catch(const std::exception &err)
    {
        std::cerr << err.what();
    }
}
