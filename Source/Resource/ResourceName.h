/*================================================================
Filename: ResourceName.h
Date: 2018.1.13
Created by AirGuanZ
================================================================*/
#ifndef VW_RESOURCE_NAME_H
#define VW_RESOURCE_NAME_H

#define VW_FILENAME(X) VW_FILENAME_AUX(X)
#define VW_FILENAME_AUX(X) #X

#define BASIC_RENDERER_VERTEX_SHADER VW_FILENAME(Bin/Shader/Basic/vertex.hlsl)
#define BASIC_RENDERER_PIXEL_SHADER  VW_FILENAME(Bin/Shader/Basic/pixel.hlsl)

#endif //VW_RESOURCE_NAME_H
