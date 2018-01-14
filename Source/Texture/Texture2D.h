/*================================================================
Filename: Texture2D.h
Date: 2018.1.14
Created by AirGuanZ
================================================================*/
#ifndef VW_TEXTURE_2D_H
#define VW_TEXTURE_2D_H

#include <string>

#include "../Utility/D3D11Header.h"

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

    ID3D11ShaderResourceView *GetSRV(void);
    const ID3D11ShaderResourceView *GetSRV(void) const;

private:
    ID3D11Resource *tex_;
    ID3D11ShaderResourceView *SRV_;
};

#endif //VW_TEXTURE_2D_H
