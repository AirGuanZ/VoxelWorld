/*================================================================
Filename: VoxelModelEditor.h
Date: 2018.3.4
Created by AirGuanZ
================================================================*/
#pragma once

#include <map>
#include <stack>
#include <vector>

#include <Application/Common.h>

class VoxelModelEditor
{
public:
    AppState Run(void);

private:
    bool Initialize(std::string &errMsg);

    bool HasBinding(void) const;

    void ReloadBindingPaths(void);
    void SetSelectedBinding(int idx);

    void ShowBindingWindow(void);
    void ShowErrMsgWindow(void);

    void PushWindowState(void);
    void PopWindowState(void);

    void EnableErrorWindow(const std::string &errMsg);

private:
    //显示控制

    struct WindowStateSetting
    {
        bool showBindingWindow;
        bool useBindingWindow;

        bool showErrMsgWindow;
        bool useErrMsgWindow;

        std::string errMsg;
    };

    WindowStateSetting winState_;
    std::stack<WindowStateSetting> winStateStack_;

private:
    //界面数据绑定

    std::vector<char> newBindingName_;

    std::vector<const char*> bindingNames_;
    int selectedBinding_;

    std::string skeletonName_;
    std::wstring skeletonPath_;

    bool exit_;

private:
    std::map<std::string, std::wstring> bindingPaths_;
};
