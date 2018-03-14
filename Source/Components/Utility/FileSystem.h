/*================================================================
Filename: FileSystem.h
Date: 2018.3.14
Created by AirGuanZ
================================================================*/
#pragma once

#include <map>
#include <string>
#include <vector>

namespace FileSystem
{
    bool ReadFile(const std::wstring &filename, std::string &output);

    bool ReadFileBinary(const std::wstring &filename, std::vector<char> &buf);

    void GetFilenames(const std::string &dirPath, const std::string &ext,
                     std::map<std::string, std::wstring> &output);

    std::string CurrentRunningPath(void);
}
