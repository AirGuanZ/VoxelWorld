/*================================================================
Filename: ConfigFile.cpp
Date: 2018.1.17
Created by AirGuanZ
================================================================*/
#include <algorithm>
#include <cassert>
#include <cctype>
#include <fstream>

#include "ConfigFile.h"

ConfigFile::ConfigFile(void)
    : state_(Uninitialized)
{

}

ConfigFile::ConfigFile(const std::wstring &filename)
    : ConfigFile()
{
    LoadFromFile(filename);
}

namespace
{
    std::string TrimInline(const std::string &s)
    {
        auto b = s.find_first_not_of(" \t");
        auto e = s.find_last_not_of(" \t");
        if(b == std::string::npos || e == std::string::npos || b > e)
            return "";
        return s.substr(b, e - b + 1);
    }
}

bool ConfigFile::LoadFromFile(const std::wstring &filename)
{
    assert(state_ == Uninitialized);

    std::ifstream fin(filename);
    std::string section = "Global", line;
    if(!fin)
        goto ErrorLabel;

    while(std::getline(fin, line))
    {
        if((line = TrimInline(line)).empty() || line[0] == '#') //空行/注释
            continue;

        if(line[0] == '[') //section
        {
            int end = line.rfind(']');
            if(end == std::string::npos || end == 1)
                goto ErrorLabel;

            section = TrimInline(line.substr(1, end - 1));
        }
        else //item
        {
            auto eq = line.find('=');
            if(eq == std::string::npos || eq == 0 || eq == line.length() - 1)
                goto ErrorLabel;

            std::string left = TrimInline(line.substr(0, eq));
            std::string right = TrimInline(line.substr(eq + 1));
            if(left.empty() || right.empty())
                goto ErrorLabel;

            map_[section][left] = right;
        }
    }

    state_ = Available;
    return true;

ErrorLabel:
    map_.clear();
    state_ = Error;
    return false;
}

bool ConfigFile::IsAvailable(void) const
{
    return state_ == Available;
}

ConfigFile::operator bool(void) const
{
    return state_ != Error;
}

void ConfigFile::Clear(void)
{
    map_.clear();
    state_ = Uninitialized;
}

bool ConfigFile::FindSection(const std::string &section)
{
    return map_.find(section) != map_.end();
}

const std::string &ConfigFile::operator()(const std::string &section, const std::string &key) const
{
    static const std::string EMPTY_STR;

    auto fIt = map_.find(section);
    if(fIt == map_.end())
        return EMPTY_STR;

    auto &sec = fIt->second;
    auto sIt = sec.find(key);
    if(sIt == sec.end())
        return EMPTY_STR;

    return sIt->second;
}
