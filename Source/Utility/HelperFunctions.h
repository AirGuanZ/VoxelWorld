/*================================================================
Filename: HelperFunctions.h
Date: 2018.1.13
Created by AirGuanZ
================================================================*/
#ifndef VW_HELPER_FUNCTIONS_H
#define VW_HELPER_FUNCTIONS_H

#include <string>

#include "D3D11Header.h"

namespace Helper
{
    template<typename T, size_t N>
    inline constexpr size_t ArraySize(const T(&arr)[N])
    {
        return N;
    }

    template<typename T>
    inline void SafeDeleteObjects(T &ptr)
    {
        if(ptr)
        {
            delete ptr;
            ptr = nullptr;
        }
    }

    template<typename T, typename... Others>
    inline void SafeDeleteObjects(T &ptr, Others&... others)
    {
        SafeDeleteObjects(ptr);
        SafeDeleteObjects(others...);
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
    inline void AddRefForCOMObject(T ptr)
    {
        ptr->AddRef();
    }

    template<typename T, typename... Others>
    inline void AddRefForCOMObject(T ptr, Others... others)
    {
        AddRefForCOMObject(ptr);
        AddRefForCOMObject(others...);
    }

    template<typename ClassType, typename MemType>
    inline size_t MemOffset(MemType ClassType::* pMem)
    {
        return reinterpret_cast<size_t>(&(reinterpret_cast<ClassType*>(0)->*pMem));
    }

    ID3D11InputLayout *CreateInputLayout(const D3D11_INPUT_ELEMENT_DESC *desc,
                                         int num, const void *byteCode, int length);

    bool ReadFile(const std::wstring &filename, std::string &output);
}

#endif //VW_HELPER_FUNCTIONS_H
