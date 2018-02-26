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

    /*  ע��������豻���ص�ģ�͵�����ϵ��magica voxel 0.99a��ͬ
        ���Զ�ת�������͵�ͼ��ѧ����ϵ��������ʵ����״����
        ����ͼ�Ļ����ļ�����ϵ�����������ģ�
                z
                |
                |
                *------- y
              /
           x /
        ���صĽ���������ģ�
                y
                |
                |
                *------- x
              /
           z /
        ��ʵ����״��ȫû��
        ���Զ�magica voxel������obj�������þ����ˣ�
        ��������Դ��obj��ע�����һ������ϵ
    */
    bool LoadFromFile(const std::wstring &filename, float scale = 1.0f);

    void Clear(void);

    std::vector<Vertex> vertices;
    std::vector<UINT16> indices;
};
