/*================================================================
Filename: ResourceName.h
Date: 2018.1.13
Created by AirGuanZ
================================================================*/
#ifndef VW_RESOURCE_NAME_H
#define VW_RESOURCE_NAME_H

#define VW_FILENAME(X) VW_FILENAME_AUX(X)
#define VW_FILENAME_AUX(X) L#X

//BasicRenderer相关

#define BASIC_RENDERER_VERTEX_SHADER VW_FILENAME(Bin/Shader/Basic/vertex.hlsl)
#define BASIC_RENDERER_PIXEL_SHADER  VW_FILENAME(Bin/Shader/Basic/pixel.hlsl)

#define BASIC_RENDERER_TEXTURE_0 VW_FILENAME(Bin/Texture/Basic/0.png)

//CurveRenderer相关

#define CARVE_RENDERER_VERTEX_SHADER VW_FILENAME(Bin/Shader/Carve/vertex.hlsl)
#define CARVE_RENDERER_PIXEL_SHADER  VW_FILENAME(Bin/Shader/Carve/pixel.hlsl)

#define CARVE_RENDERER_TEXTURE_0 VW_FILENAME(Bin/Texture/Carve/0.png)

//LiquidRenderer相关

#define LIQUID_RENDERER_TEXTURE_0 VW_FILENAME(Bin/Texture/Liquid/0.png)

//ImmediateScreen2D相关

#define IMMEDIATE_SCREEN_2D_NOR_VERTEX_SHADER VW_FILENAME(Bin/Shader/ImmediateScreen2D/normalVertex.hlsl)
#define IMMEDIATE_SCREEN_2D_NOR_PIXEL_SHADER  VW_FILENAME(Bin/Shader/ImmediateScreen2D/normalPixel.hlsl)

//Crosshair

#define CROSSHAIR_BASIC_TEXTURE VW_FILENAME(Bin/Texture/Crosshair/cross.png)

//ActorModel相关

#define ACTOR_RENDERER_VERTEX_SHADER VW_FILENAME(Bin/Shader/Actor/vertex.hlsl)
#define ACTOR_RENDERER_PIXEL_SHADER  VW_FILENAME(Bin/Shader/Actor/pixel.hlsl)

#define ACTOR_RENDERER_DEFAULT_TEXTURE VW_FILENAME(Bin/Texture/Actor/tex.png)

#define ACTOR_MODEL_DEFAULT_HEAD       VW_FILENAME(Bin/Model/Actor/head.obj)
#define ACTOR_MODEL_DEFAULT_BODY       VW_FILENAME(Bin/Model/Actor/body.obj)
#define ACTOR_MODEL_DEFAULT_LEFT_HAND  VW_FILENAME(Bin/Model/Actor/leftHand.obj)
#define ACTOR_MODEL_DEFAULT_RIGHT_HAND VW_FILENAME(Bin/Model/Actor/rightHand.obj)
#define ACTOR_MODEL_DEFAULT_LEFT_FOOT  VW_FILENAME(Bin/Model/Actor/leftFoot.obj)
#define ACTOR_MODEL_DEFAULT_RIGHT_FOOT VW_FILENAME(Bin/Model/Actor/rightFoot.obj)

constexpr float ACTOR_MODEL_SCALE_FACTOR = 0.02f;

#endif //VW_RESOURCE_NAME_H
