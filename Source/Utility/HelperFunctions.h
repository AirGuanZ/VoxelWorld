/*================================================================
Filename: HelperFunctions.h
Date: 2018.1.13
Created by AirGuanZ
================================================================*/
#ifndef VW_HELPER_FUNCTIONS_H
#define VW_HELPER_FUNCTIONS_H

namespace Helper
{
    template<typename T, size_t N>
    inline constexpr size_t ArraySize(const T(&arr)[N])
    {
        return N;
    }

    template<typename T>
    void SafeDeleteObjects(T &ptr)
    {
        if(ptr)
        {
            delete ptr;
            ptr = nullptr;
        }
    }

    template<typename T, typename... Others>
    void SafeDeleteObjects(T &ptr, Others&... others)
    {
        SafeDeleteObjects(ptr);
        SafeDeleteObjects(others...);
    }

    template<typename T>
    void ReleaseCOMObjects(T &ptr)
    {
        if(ptr)
        {
            ptr->Release();
            ptr = nullptr;
        }
    }

    template<typename T, typename... Others>
    void ReleaseCOMObjects(T &ptr, Others&... others)
    {
        ReleaseCOMObjects(ptr);
        ReleaseCOMObjects(others...);
    }
}

#endif //VW_HELPER_FUNCTIONS_H
