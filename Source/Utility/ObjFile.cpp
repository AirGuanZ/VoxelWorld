/*================================================================
Filename: ObjFile.cpp
Date: 2018.2.6
Created by AirGuanZ
================================================================*/
#define _CRT_SECURE_NO_WARNINGS

#include <cstdio>
#include <fstream>

#include "ObjFile.h"

namespace
{
    bool parsePosition(const std::string &line, Vector3 &pos)
    {
        return std::sscanf(line.c_str(), "v%f%f%f", &pos.x, &pos.y, &pos.z) == 3;
    }

    bool parseNormal(const std::string &line, Vector3 &nor)
    {
        return std::sscanf(line.c_str(), "vn %f%f%f", &nor.x, &nor.y, &nor.z) == 3;
    }

    bool parseTexCoord(const std::string &line, Vector2 &texCoord)
    {
        return std::sscanf(line.c_str(), "vt %f%f", &texCoord.x, &texCoord.y) == 2;
    }

    bool parseTriangle(const std::string &line, int *pos, int *nor, int *tex)
    {
        return std::sscanf(line.c_str(), "f %d/%d/%d %d/%d/%d %d/%d/%d",
                           pos, tex, nor,
                           pos + 1, tex + 1, nor + 1,
                           pos + 2, tex + 2, nor + 2) == 9;
    }
}

bool ObjFile_PNT::LoadFromFile(const std::wstring &filename, float scale)
{
    assert(scale > 0.0f);

    Clear();

    std::vector<Vector3> pos;
    std::vector<Vector3> nor;
    std::vector<Vector2> tex;

    std::vector<Vertex> vtx;
    std::vector<UINT16> idx;

    Vector3 vpos, vnor;
    Vector2 vtex;
    int ipos[3], inor[3], itex[3];

    std::ifstream fin(filename, std::ifstream::in);
    if(!fin)
        return false;

    std::string line;
    while(std::getline(fin, line))
    {
        //Ìø¹ý¿ÕÐÐºÍ×¢ÊÍ
        if(line.empty() || line[0] == '#')
            continue;

        if(parsePosition(line, vpos))
            pos.push_back(scale * vpos);
        else if(parseNormal(line, vnor))
            nor.push_back(vnor);
        else if(parseTexCoord(line, vtex))
            tex.push_back(vtex);
        else if(parseTriangle(line, ipos, inor, itex))
        {
            vtx.push_back({ pos[ipos[0] - 1], nor[inor[0] - 1], tex[itex[0] - 1] });
            vtx.push_back({ pos[ipos[1] - 1], nor[inor[1] - 1], tex[itex[1] - 1] });
            vtx.push_back({ pos[ipos[2] - 1], nor[inor[2] - 1], tex[itex[2] - 1] });

            UINT16 sizeBase = static_cast<UINT16>(idx.size());
            idx.push_back(sizeBase);
            idx.push_back(sizeBase + 2);
            idx.push_back(sizeBase + 1);
        }
    }

    vertices = std::move(vtx);
    indices = std::move(idx);
    return true;
}

void ObjFile_PNT::Clear(void)
{
    vertices.clear();
    indices.clear();
}
