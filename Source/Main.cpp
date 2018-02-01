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

    /*using namespace LandGenerator_V1;

    HWND hWnd = GetConsoleWindow();
    HDC hDC = GetDC(hWnd);

    BiomeGenerator bm(5892383);
    for(int ckX = 0; ckX != 64; ++ckX)
    {
        int xBase = CHUNK_SECTION_SIZE * ckX;
        for(int ckZ = 0; ckZ != 32; ++ckZ)
        {
            int zBase = CHUNK_SECTION_SIZE * ckZ;
            bm.Generate(ckX, ckZ);

            for(int x = 0; x != CHUNK_SECTION_SIZE; ++x)
            {
                for(int z = 0; z != CHUNK_SECTION_SIZE; ++z)
                {
                    if(x + xBase == 29 && z + zBase == 30)
                    {
                        x = x;
                    }

                    SetPixel(hDC, 50 + x + xBase, 50 + z + zBase, RGB((2.0f * bm.GetResult(x, z).factor - 1.0f) * 255, 0, 0));
                }
            }
        }
    }*/
}
