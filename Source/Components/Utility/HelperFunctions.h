/*================================================================
Filename: HelperFunctions.h
Date: 2018.1.13
Created by AirGuanZ
================================================================*/
#pragma once

#include <codecvt>
#include <fstream>
#include <locale>
#include <map>
#include <string>
#include <vector>

#include "D3D11Header.h"

namespace Helper
{
    template<typename T, int N>
    inline constexpr int ArraySize(const T(&arr)[N])
    {
        return N;
    }

    template<typename T>
    inline void SafeDeleteObjects(T &&ptr)
    {
        if(ptr)
        {
            delete ptr;
            ptr = nullptr;
        }
    }

    template<typename T, typename... Others>
    inline void SafeDeleteObjects(T &&ptr, Others&&... others)
    {
        SafeDeleteObjects(ptr);
        SafeDeleteObjects(forward(others)...);
    }

    template<typename T>
    inline void ReleaseCOMObjects(T &ptr)
    {
        if(ptr)
        {
            ptr->Release();
            ptr = nullptr;
        }
    }

    template<typename T, typename... Others>
    inline void ReleaseCOMObjects(T &ptr, Others&... others)
    {
        ReleaseCOMObjects(ptr);
        ReleaseCOMObjects(others...);
    }

    template<typename T>
    inline void AddRefForCOMObjects(T ptr)
    {
        if(ptr)
            ptr->AddRef();
    }

    template<typename T, typename... Others>
    inline void AddRefForCOMObjects(T ptr, Others... others)
    {
        AddRefForCOMObjects(ptr);
        AddRefForCOMObjects(others...);
    }

    template<typename ClassType, typename MemType>
    inline UINT MemOffset(MemType ClassType::* pMem)
    {
        return static_cast<UINT>(reinterpret_cast<size_t>(&(reinterpret_cast<ClassType*>(0)->*pMem)));
    }

    std::string ToStr(const std::wstring &wstr);

    std::wstring ToWStr(const std::string &str);

    template<typename T>
    bool ReadBinary(std::ifstream &fin, T &data)
    {
        fin.read(reinterpret_cast<char*>(&data), sizeof(data));
        return static_cast<bool>(fin);
    }

    template<typename T, typename...Others>
    bool ReadBinary(std::ifstream &fin, T &data, Others&...others)
    {
        return ReadBinary(fin, data) &&
               ReadBinary(fin, others...);
    }

    template<typename T>
    bool WriteBinary(std::ofstream &fout, T &&data)
    {
        fout.write(reinterpret_cast<const char*>(&data), sizeof(data));
        return static_cast<bool>(fout);
    }

    template<typename T, typename...Others>
    bool WriteBinary(std::ofstream &fout, T &&data, Others&&...others)
    {
        return WriteBinary(fout, std::forward<T>(data)) &&
               WriteBinary(fout, std::forward<Others>(others)...);
    }

    bool ReadString(std::ifstream &fin, std::string &str);
    bool WriteString(std::ofstream &fout, const std::string &str);
}
