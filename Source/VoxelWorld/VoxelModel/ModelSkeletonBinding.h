/*================================================================
Filename: ModelSkeletonBinding.h
Date: 2018.3.6
Created by AirGuanZ
================================================================*/
#pragma once

#include <map>
#include <string>

class ModelSkeletonBinding
{
public:
    bool LoadFromFile(const std::wstring &filename);
    bool SaveToFile(const std::wstring &filename) const;

    void Clear(void);

    bool GetBone(const std::string &component, std::string &bone) const;
    bool AddComponent(const std::string &component, const std::string &bone);

    template<typename Func>
    void ForAllComponents(Func &&func)
    {
        for(const auto &it : componentToBone_)
            func(it.first);
    }

private:
    std::map<std::string, std::string> componentToBone_;
};
