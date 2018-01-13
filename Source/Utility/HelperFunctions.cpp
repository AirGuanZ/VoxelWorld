/*================================================================
Filename: HelperFunctions.cpp
Date: 2018.1.13
Created by AirGuanZ
================================================================*/
#include <fstream>
#include <iterator>
#include <string>

#include "../Window/Window.h"
#include "D3D11Header.h"
#include "HelperFunctions.h"

ID3D11InputLayout *Helper::CreateInputLayout(const D3D11_INPUT_ELEMENT_DESC *desc,
                                             int num, const void *byteCode, int length)
{
    assert(desc != nullptr && num > 0);
    assert(byteCode != nullptr && length > 0);
    assert(Window::GetInstance().IsD3DAvailable());

    ID3D11InputLayout *rt = nullptr;
    HRESULT hr = Window::GetInstance().GetD3DDevice()->CreateInputLayout(
        desc, static_cast<UINT>(num), byteCode, static_cast<SIZE_T>(length), &rt);
    return FAILED(hr) ? nullptr : rt;
}

bool Helper::ReadFile(const std::string &filename, std::string &output)
{
    output = "";
    std::ifstream fin(filename, std::ifstream::in);
    if(!fin)
        return false;
    output = std::string(std::istreambuf_iterator<char>(fin),
                         std::istreambuf_iterator<char>());
}
