/*================================================================
Filename: ResourceNameManager.h
Date: 2018.2.28
Created by AirGuanZ
================================================================*/
#pragma once

#include <codecvt>
#include <string>

#include <Utility/ConfigFile.h>
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
        static std::wstring_convert<std::codecvt_utf8_utf16<std::wstring::value_type>> wcvt;
        output = wcvt.from_bytes(conf_(section, key));
        return output.size() != 0;
    }

    std::wstring operator()(const std::string &section, const std::string &key)
    {
        static std::wstring_convert<std::codecvt_utf8_utf16<std::wstring::value_type>> wcvt;
        return wcvt.from_bytes(conf_(section, key));
    }

    void Clear(void)
    {
        conf_.Clear();
    }

private:
    ConfigFile conf_;
};

using RscNameMgr = ResourceNameManager;
