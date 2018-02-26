/*================================================================
Filename: InputLayout.h
Date: 2018.2.10
Created by AirGuanZ
================================================================*/
#pragma once

#include <Utility/D3D11Header.h>
#include <Utility/HelperFunctions.h>

#include <Window/Window.h>

class InputLayout
{
public:
    InputLayout(void)
        : inputLayout_(nullptr)
    {

    }

    InputLayout(InputLayout &other)
        : inputLayout_(other.inputLayout_)
    {
        Helper::AddRefForCOMObjects(inputLayout_);
    }

    InputLayout &operator=(InputLayout &other)
    {
        Helper::ReleaseCOMObjects(inputLayout_);
        inputLayout_ = other.inputLayout_;
        Helper::AddRefForCOMObjects(inputLayout_);
        return *this;
    }

    ~InputLayout(void)
    {
        Helper::ReleaseCOMObjects(inputLayout_);
    }

    bool IsAvailable(void) const
    {
        return inputLayout_ != nullptr;
    }

    void Destroy(void)
    {
        Helper::ReleaseCOMObjects(inputLayout_);
    }

    template<int N>
    bool Initialize(D3D11_INPUT_ELEMENT_DESC (&desc)[N], const void *byteCode, UINT length)
    {
        return Initialize(desc, N, byteCode, length);
    }

    bool Initialize(D3D11_INPUT_ELEMENT_DESC *desc, int N, const void *byteCode, UINT length)
    {
        Helper::ReleaseCOMObjects(inputLayout_);

        HRESULT hr = Window::GetInstance().GetD3DDevice()->CreateInputLayout(
            desc, N, byteCode, length, &inputLayout_);
        return SUCCEEDED(hr);
    }

    operator ID3D11InputLayout* (void)
    {
        return inputLayout_;
    }

    ID3D11InputLayout *GetInputLayout(void)
    {
        return inputLayout_;
    }

private:
    ID3D11InputLayout *inputLayout_;
};

