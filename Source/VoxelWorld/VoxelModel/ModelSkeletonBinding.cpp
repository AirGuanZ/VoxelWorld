/*================================================================
Filename: ModelSkeletonBinding.cpp
Date: 2018.3.6
Created by AirGuanZ
================================================================*/
#include <fstream>

#include <Utility/ConfigFile.h>

#include "ModelSkeletonBinding.h"

bool ModelSkeletonBinding::LoadFromFile(const std::wstring &filename)
{
    Clear();
    
    ConfigFile conf;
    if(!conf.LoadFromFile(filename))
        return false;

    auto &section = conf.GetSection("Global");
    for(auto &it : section.GetData())
        componentToBone_[it.first] = it.second;

    return true;
}

bool ModelSkeletonBinding::SaveToFile(const std::wstring &filename) const
{
    std::ofstream fout(filename, std::ios_base::out | std::ios_base::trunc);
    if(!fout)
        return false;

    for(auto &it : componentToBone_)
        fout << it.first << " = " << it.second << std::endl;

    return true;
}

void ModelSkeletonBinding::Clear(void)
{
    componentToBone_.clear();
}

bool ModelSkeletonBinding::GetBone(const std::string &component, std::string &bone) const
{
    auto it = componentToBone_.find(component);
    if(it != componentToBone_.end())
    {
        bone = it->second;
        return true;
    }
    return false;
}

bool ModelSkeletonBinding::AddComponent(const std::string &component, const std::string &bone)
{
    if(componentToBone_.find(component) != componentToBone_.end())
        return false;
    componentToBone_[component] = bone;
    return true;
}
