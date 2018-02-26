/*================================================================
Filename: ObjFile.h
Date: 2018.2.6
Created by AirGuanZ
================================================================*/
#pragma once

#include <string>
#include <vector>

#include "Math.h"

class ObjFile_PNT
{
public:
    struct Vertex
    {
        Vector3 pos;
        Vector3 nor;
        Vector2 uv;
    };

    /*  注意这里假设被加载的模型的坐标系和magica voxel 0.99a相同
        会自动转换到典型的图形学右手系，而保持实际形状不变
        画个图的话，文件坐标系假设是这样的：
                z
                |
                |
                *------- y
              /
           x /
        加载的结果是这样的：
                y
                |
                |
                *------- x
              /
           z /
        而实际形状完全没变
        所以对magica voxel导出的obj，放心用就行了；
        对其他来源的obj，注意对照一下坐标系
    */
    bool LoadFromFile(const std::wstring &filename, float scale = 1.0f);

    void Clear(void);

    std::vector<Vertex> vertices;
    std::vector<UINT16> indices;
};
