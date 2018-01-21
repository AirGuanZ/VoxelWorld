/*================================================================
Filename: Main.cpp
Date: 2017.12.10
Created by AirGuanZ
================================================================*/
#include "Sample_BasicShader.h"
#include "Sample_GeometryShader.h"
#include "Sample_HeightMap.h"
#include "Sample_MixTex.h"
#include "Sample_TextureCube.h"
#include "Sample_NormalMap.h"

int main(void)
{
    try
    {
        Sample_NormalMap::App app;
        app.Run();
    }
    catch(const OWE::Error &err)
    {
        std::cout << err.what() << std::endl;
    }
}
