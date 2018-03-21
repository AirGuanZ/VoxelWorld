/*================================================================
Filename: TextureFile.cpp
Date: 2018.1.14
Created by AirGuanZ
================================================================*/
#include <DirectXTK\ScreenGrab.h>
#include <DirectXTK\WICTextureLoader.h>
#include <wincodec.h>

#include <Window\Window.h>
#include "TextureFile.h"

SINGLETON_CLASS_DEFINITION(TextureFile);

bool TextureFile::LoadTexture2D(const std::wstring &filename, ID3D11Resource *&tex, ID3D11ShaderResourceView *&SRV)
{
    HRESULT hr = DirectX::CreateWICTextureFromFile(
        Window::GetInstance().GetD3DDevice(),
        filename.c_str(), &tex, &SRV);
    return SUCCEEDED(hr);
}

bool TextureFile::SaveTexture2DToPNG(const std::wstring &filename, ID3D11Resource *tex)
{
    HRESULT hr = DirectX::SaveWICTextureToFile(
        Window::GetInstance().GetD3DDeviceContext(),
        tex, GUID_ContainerFormatPng, filename.c_str());
    return SUCCEEDED(hr);
}

TextureFile::TextureFile(void)
{
    HRESULT hr = CoInitializeEx(nullptr, COINITBASE_MULTITHREADED);
    if(FAILED(hr))
        std::exit(-1);
}
