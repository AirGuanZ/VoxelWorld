/*================================================================
Filename: VoxelModelEditor.h
Date: 2018.3.4
Created by AirGuanZ
================================================================*/
#pragma once

#include <map>
#include <string>

#include <SkeletonAnimation/SkeletonData.h>

#include <Application/Common.h>
#include <VoxelModel/VoxelModel.h>
#include <VoxelModel/VoxelModelAnimationDisplayer.h>

class VoxelModelEditor
{
public:
    AppState Run(void);

private:
    bool Initialize(std::string &errMsg);

    bool InitSkeletonPaths (std::string &errMsg);
    bool InitComponentPaths(std::string &errMsg);
    bool InitBindingPaths  (std::string &errMsg);

    using SpecFilePaths = std::map<std::wstring, std::wstring>;
    bool InitSpecFileList(SpecFilePaths &output, std::string &errMsg,
                          const std::wstring &dirPath, const std::wstring &ext);

private:
    //������ģ���������

    //�ӹ������ֵ����·����ӳ��
    SpecFilePaths skeletonPaths_;

    //��������ֵ����·����ӳ��
    SpecFilePaths componentPaths_;

    //�󶨷�������һЩ����󶨵��ض��Ĺ������Թ���һ�������Ķ���ģ��
    //�Ӱ󶨷������ֵ����·����ӳ��
    SpecFilePaths bindingPaths_;

    Skeleton::Skeleton skeleton_;
};
