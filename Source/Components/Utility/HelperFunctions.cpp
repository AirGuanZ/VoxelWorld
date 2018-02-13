/*================================================================
Filename: HelperFunctions.cpp
Date: 2018.1.13
Created by AirGuanZ
================================================================*/
#include <fstream>
#include <iterator>
#include <string>
#include <vector>

#include "D3D11Header.h"
#include "HelperFunctions.h"

bool Helper::ReadFile(const std::wstring &filename, std::string &output)
{
    output = "";
    std::ifstream fin(filename, std::ifstream::in);
    if(!fin)
        return false;
    output = std::string(std::istreambuf_iterator<char>(fin),
        std::istreambuf_iterator<char>());
    return true;
}

bool Helper::ReadFileBinary(const std::wstring &filename, std::vector<char> &buf)
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
