/*================================================================
Filename: Texture2D.cpp
Date: 2018.1.14
Created by AirGuanZ
================================================================*/
#include <cassert>
#include <utility>

#include "../Utility/HelperFunctions.h"
#include "../Window/Window.h"
#include "Texture2D.h"
#include "TextureFile.h"

Texture2D::Texture2D(void)
    : tex_(nullptr), SRV_(nullptr)
{

}

Texture2D::Texture2D(const Texture2D &other)
    : tex_(other.tex_), SRV_(other.SRV_)
{
    if(other.IsAvailable())
        Helper::AddRefForCOMObjects(tex_, SRV_);
}

Texture2D::Texture2D(Texture2D &&other)
    : tex_(other.tex_), SRV_(SRV_)
{
    other.tex_ = nullptr;
    other.SRV_ = nullptr;
}

Texture2D &Texture2D::operator=(const Texture2D &other)
{
    Destroy();
    tex_ = other.tex_;
    SRV_ = other.SRV_;
    if(other.IsAvailable())
        Helper::AddRefForCOMObjects(tex_, SRV_);
    return *this;
}

Texture2D &Texture2D::operator=(Texture2D &&other)
{
    Destroy();
    std::swap(tex_, other.tex_);
    std::swap(SRV_, other.SRV_);
    return *this;
}

Texture2D::~Texture2D(void)
{
    Destroy();
}

bool Texture2D::LoadFromFile(const std::wstring &filename)
{
    Destroy();
    return TextureFile::GetInstance().LoadTexture2D(filename, tex_, SRV_);
}

void Texture2D::Destroy(void)
{
    Helper::ReleaseCOMObjects(tex_, SRV_);
}

bool Texture2D::IsAvailable(void) const
{
    assert((tex_ != nullptr) == (SRV_ != nullptr));
    return tex_ != nullptr;
}
