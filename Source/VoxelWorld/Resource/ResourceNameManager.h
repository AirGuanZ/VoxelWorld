/*================================================================
Filename: ResourceNameManager.h
Date: 2018.2.28
Created by AirGuanZ
================================================================*/
#pragma once

#include <codecvt>
#include <string>

#include <Utility/ConfigFile.h>
#include <Utility/HelperFunctions.h>
#include <Utility/Singleton.h>

class ResourceNameManager : public Singleton<ResourceNameManager>
{
public:
    bool LoadFromFile(const std::wstring &filename)
    {
        conf_.Clear();
        return conf_.LoadFromFile(filename);
    }

    bool Find(const std::string &section, const std::string &key, std::wstring &output) const
    {
        output = Helper::ToWStr(conf_(section, key));
        return output.size() != 0;
    }

    std::wstring operator()(const std::string &section, const std::string &key)
    {
        return Helper::ToWStr(conf_(section, key));
    }

    void Clear(void)
    {
        conf_.Clear();
    }

    std::string AsString(const std::string &section, const std::string &key)
    {
        return conf_(section, key);
    }

private:
    ConfigFile conf_;
};

using RscNameMgr = ResourceNameManager;
