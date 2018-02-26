/*================================================================
Filename: Texture2D.h
Date: 2018.1.14
Created by AirGuanZ
================================================================*/
#pragma once

#include <string>

#include <Utility/D3D11Header.h>

class Texture2D
{
public:
    static const Texture2D EmptyTexture2D;

    Texture2D(void);
    Texture2D(const Texture2D &other);
    Texture2D(Texture2D &&other);
    Texture2D &operator=(const Texture2D &other);
    Texture2D &operator=(Texture2D &&other);
    ~Texture2D(void);

    bool LoadFromFile(const std::wstring &filename);
    void Destroy(void);
    bool IsAvailable(void) const;

    ID3D11ShaderResourceView *GetSRV(void)
    {
        return SRV_;
    }

    const ID3D11ShaderResourceView *GetSRV(void) const
    {
        return SRV_;
    }
    
    operator ID3D11ShaderResourceView* (void)
    {
        return SRV_;
    }

private:
    ID3D11Resource *tex_;
    ID3D11ShaderResourceView *SRV_;
};
