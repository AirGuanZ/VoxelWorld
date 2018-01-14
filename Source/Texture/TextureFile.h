/*================================================================
Filename: TextureFile.h
Date: 2018.1.14
Created by AirGuanZ
================================================================*/
#ifndef VW_TEXTURE_FILE_H
#define VW_TEXTURE_FILE_H

#include <string>

#include "../Utility/D3D11Header.h"
#include "../Utility/Singleton.h"

class TextureFile : public Singleton<TextureFile>
{
public:
    bool LoadTexture2D(const std::wstring &filename, ID3D11Resource *&tex, ID3D11ShaderResourceView *&SRV_);
    bool SaveTexture2DToPNG(const std::wstring &filename, ID3D11Resource *tex);

    TextureFile(void);
};

#endif //VW_TEXTURE_FILE_H
