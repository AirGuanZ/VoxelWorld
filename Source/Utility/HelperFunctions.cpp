/*================================================================
Filename: HelperFunctions.cpp
Date: 2018.1.13
Created by AirGuanZ
================================================================*/
#include <fstream>
#include <iterator>
#include <string>
#include <vector>

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

bool Helper::ReadFileBinary(const std::wstring &filename, std::vector<char> &buf)
{
    buf.clear();

    std::ifstream fin(filename, std::ios::binary);
    if(!fin)
        return false;

    std::filebuf *fb = fin.rdbuf();
    std::streampos fileSize = fb->pubseekoff(0, std::ios::end, std::ios::in);
    fb->pubseekpos(0, std::ios::in);

    buf.resize(fileSize);
    fb->sgetn(buf.data(), fileSize);

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

ID3D11Buffer *Helper::CreateIndexBuffer(void *initData, size_t byteSize, bool dynamic)
{
    assert(dynamic || initData);

    D3D11_BUFFER_DESC dc;
    dc.BindFlags = D3D11_BIND_INDEX_BUFFER;
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

ID3D11SamplerState *Helper::CreateSamplerState(D3D11_FILTER filter, D3D11_TEXTURE_ADDRESS_MODE addrMode)
{
    D3D11_SAMPLER_DESC dc;
    dc.Filter = filter;
    dc.AddressU = addrMode;
    dc.AddressV = addrMode;
    dc.AddressW = addrMode;
    dc.MinLOD = -FLT_MAX;
    dc.MaxLOD = +FLT_MAX;
    dc.MipLODBias = 0.0f;
    dc.MaxAnisotropy = 1;
    dc.ComparisonFunc = D3D11_COMPARISON_NEVER;
    dc.BorderColor[0] = dc.BorderColor[1] = dc.BorderColor[2] = dc.BorderColor[3] = 1.0f;
    
    ID3D11SamplerState *rt = nullptr;
    HRESULT hr = Window::GetInstance().GetD3DDevice()->CreateSamplerState(&dc, &rt);

    return SUCCEEDED(hr) ? rt : nullptr;
}
