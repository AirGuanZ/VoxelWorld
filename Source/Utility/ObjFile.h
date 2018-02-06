/*================================================================
Filename: ObjFile.h
Date: 2018.2.6
Created by AirGuanZ
================================================================*/
#ifndef VW_OBJ_FILE_H
#define VW_OBJ_FILE_H

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

    bool LoadFromFile(const std::wstring &filename, float scale = 1.0f);
    void Clear(void);

    std::vector<Vertex> vertices;
    std::vector<UINT> indices;
};

#endif //VW_OBJ_FILE_H
