/*================================================================
Filename: BasicBuffer.h
Date: 2018.1.23
Created by AirGuanZ
================================================================*/
#ifndef VW_BASIC_BUFFER_H
#define VW_BASIC_BUFFER_H

#include <cassert>

#include "../Utility/D3D11Header.h"
#include "../Utility/HelperFunctions.h"
#include "../Window/Window.h"

class BasicBufferView
{
public:
    BasicBufferView(void)
        : buf_(nullptr)
    {

    }

    BasicBufferView(const BasicBufferView &other)
    {
        buf_ = other.buf_;
        Helper::AddRefForCOMObjects(buf_);
    }

    BasicBufferView &operator=(const BasicBufferView &other)
    {
        buf_ = other.buf_;
        Helper::AddRefForCOMObjects(buf_);
        return *this;
    }

    ~BasicBufferView(void)
    {
        Helper::ReleaseCOMObjects(buf_);
    }

    operator ID3D11Buffer* (void)
    {
        return buf_;
    }

    ID3D11Buffer *GetBuffer(void)
    {
        return buf_;
    }

    bool IsAvailable(void) const
    {
        return buf_ != nullptr;
    }

protected:
    ID3D11Buffer *buf_;
};

template<bool Dynamic>
class BasicBufferSetter;

template<>
class BasicBufferSetter<true> : public BasicBufferView
{
public:
    bool SetData(const void *data, int byteSize)
    {
        assert(data && byteSize > 0);

        D3D11_MAPPED_SUBRESOURCE rsc;
        HRESULT hr = Window::GetInstance().GetD3DDeviceContext()
            ->Map(buf_, 0, D3D11_MAP_WRITE_DISCARD, 0, &rsc);
        if(FAILED(hr))
            return false;

        std::memcpy(rsc.pData, data, byteSize);
        Window::GetInstance().GetD3DDeviceContext()
            ->Unmap(buf_, 0);
        return true;
    }
};

template<>
class BasicBufferSetter<false> : public BasicBufferView
{

};

template<D3D11_BIND_FLAG BindFlag, bool Dynamic>
class BasicBuffer : public BasicBufferSetter<Dynamic>
{
public:
    bool Initialize(const void *data, int byteSize)
    {
        assert(byteSize > 0);
        Helper::ReleaseCOMObjects(buf_);

        D3D11_USAGE usage;
        UINT cpuAccessFlag;

        if constexpr(Dynamic)
        {
            usage = D3D11_USAGE_DYNAMIC;
            cpuAccessFlag = D3D11_CPU_ACCESS_WRITE;
        }
        else
        {
            assert(data != nullptr);
            usage = D3D11_USAGE_IMMUTABLE;
            cpuAccessFlag = 0;
        }

        D3D11_BUFFER_DESC dc;
        dc.BindFlags = BindFlag;
        dc.ByteWidth = byteSize;
        dc.CPUAccessFlags = cpuAccessFlag;
        dc.MiscFlags = 0;
        dc.StructureByteStride = 0;
        dc.Usage = usage;

        const D3D11_SUBRESOURCE_DATA dataDesc = { data, 0, 0 };

        HRESULT hr = Window::GetInstance().GetD3DDevice()
            ->CreateBuffer(&dc, data ? &dataDesc : nullptr, &buf_);
        return SUCCEEDED(hr);
    }

    void Destroy(void)
    {
        Helper::ReleaseCOMObjects(buf_);
    }
};

template<bool Dynamic>
using VertexBuffer = BasicBuffer<D3D11_BIND_VERTEX_BUFFER, Dynamic>;

template<bool Dynamic>
using IndexBuffer = BasicBuffer<D3D11_BIND_INDEX_BUFFER, Dynamic>;

#endif //VW_BASIC_BUFFER_H
