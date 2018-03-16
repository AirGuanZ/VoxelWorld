/*================================================================
Filename: FileSystem.cpp
Date: 2018.3.14
Created by AirGuanZ
================================================================*/
#include <cassert>
#include <filesystem>
#include <fstream>
#include <iterator>

#include <Windows.h>

#include "FileSystem.h"

namespace fs = std::experimental::filesystem::v1;

bool FileSystem::ReadFile(const std::wstring &filename, std::string &output)
{
    output = "";
    std::ifstream fin(filename, std::ifstream::in);
    if(!fin)
        return false;
    output = std::string(std::istreambuf_iterator<char>(fin),
                         std::istreambuf_iterator<char>());
    return true;
}

bool FileSystem::ReadFileBinary(const std::wstring &filename, std::vector<char> &buf)
{
    buf.clear();

    std::ifstream fin(filename, std::ios::binary);
    if(!fin)
        return false;

    std::filebuf *fb = fin.rdbuf();
    std::streampos fileSize = fb->pubseekoff(0, std::ios::end, std::ios::in);
    fb->pubseekpos(0, std::ios::in);

    buf.resize(static_cast<std::vector<char>::size_type>(fileSize));
    fb->sgetn(buf.data(), fileSize);

    return true;
}

void FileSystem::GetFilenames(const std::string &dirPath,
                              const std::string &ext,
                              std::map<std::string, std::wstring> &output)
{
    output.clear();

    if(ext.empty())
    {
        for(auto &entry : fs::directory_iterator(dirPath))
        {
            auto &path = entry.path();
            if(fs::is_regular_file(path) && !path.has_extension())
                output[path.stem().string()] = path.wstring();
        }
    }
    else
    {
        for(auto &entry : fs::directory_iterator(dirPath))
        {
            auto &path = entry.path();
            if(fs::is_regular_file(path) &&
                path.has_extension() &&
                path.extension().string() == ext)
            {
                output[path.stem().string()] = path.wstring();
            }
        }
    }
}

std::string FileSystem::CurrentRunningPath(void)
{
    return fs::current_path().string();
}

std::string FileSystem::AbsolutePath(const std::string &relative)
{
    assert(fs::path(relative).is_relative());
    return fs::absolute(fs::path(relative)).string();
}

std::string FileSystem::RelativePath(const std::string &absolute)
{
    assert(fs::path(absolute).is_absolute());
    return fs::path(absolute).relative_path().string();
}

std::string FileSystem::PathToFilename(const std::string &path)
{
    return fs::path(path).filename().string();
}
