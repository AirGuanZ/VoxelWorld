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
#include <VoxelModel/ModelSkeletonBinding.h>
#include <VoxelModel/VoxelModel.h>
#include <VoxelModel/VoxelModelAnimationDisplayer.h>

class VoxelModelEditor
{
public:
    enum class State
    {
        Normal,
        AddNewBinding,
    };

    AppState Run(void);

private:
    bool Initialize(std::string &errMsg);

    bool InitSkeletonPaths (std::string &errMsg);
    bool InitComponentPaths(std::string &errMsg);
    bool InitBindingPaths  (std::string &errMsg);

    using SpecFilePaths = std::map<std::string, std::wstring>;
    bool InitSpecFileList(SpecFilePaths &output, std::string &errMsg,
                          const std::wstring &dirPath, const std::wstring &ext);

    bool InitGUI(std::string &errMsg);

    void SelectionWindow(void);
    void AddNewBindingWindow(void);

    bool NoBindingSelected(void);

    void CreateNewBinding(const std::string &name);

    void ReloadBinding(void);

private:
    //动画、模型相关数据

    //从骨骼名字到相对路径的映射
    SpecFilePaths skeletonPaths_;

    //从组件名字到相对路径的映射
    SpecFilePaths componentPaths_;

    //绑定方案：将一些组件绑定到特定的骨骼上以构成一个完整的动画模型
    //从绑定方案名字到相对路径的映射
    SpecFilePaths bindingPaths_;

    Skeleton::Skeleton skeleton_;

private:
    //用户交互

    std::vector<const char*> bindingNames_;
    int currentBindingIdx_;

    std::vector<const char*> componentsOfSelectedBinding_;
    int currentComponentIdx_;

    ModelSkeletonBinding currentBinding_;

    bool exitClicked_;

    State state_;
};
