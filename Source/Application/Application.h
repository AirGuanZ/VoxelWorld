/*================================================================
Filename: Application.h
Date: 2018.1.12
Created by AirGuanZ
================================================================*/
#ifndef VW_APPLICATION_H
#define VW_APPLICATION_H

#include <stdexcept>

class Application
{
public:
    void Run(void)
    {
        throw std::exception("Hello, world\n");
    }
};

#endif //VW_APPLICATION_H
