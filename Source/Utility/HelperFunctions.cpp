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

ID3D11Buffer *Helper::CreateVertexBuffer(void *initData, size_t byteSize, bool dynamic)
{
    assert(dynamic || initData);

    D3D11_BUFFER_DESC dc;
    dc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    dc.ByteWidth = byteSize;
    dc.CPUAccessFlags = dynamic ? D3D11_CPU_ACCESS_WRITE : 0;
    dc.MiscFlags = 0;
    dc.StructureByteStride = 0;
    dc.Usage = dynamic ? D3D11_USAGE_DYNAMIC : D3D11_USAGE_IMMUTABLE;

    D3D11_SUBRESOURCE_DATA data = { initData, 0, 0 };

    ID3D11Buffer *rt = nullptr;
    HRESULT hr = Window::GetInstance().GetD3DDevice()->CreateBuffer(
        &dc, initData ? &data : nullptr, &rt);

    return SUCCEEDED(hr) ? rt : nullptr;
}
