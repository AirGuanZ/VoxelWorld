/*================================================================
Filename: V2/LandGenerator.cpp
Date: 2018.2.3
Created by AirGuanZ
================================================================*/
#include <cassert>
#include <numeric>

#include <World\Chunk\Chunk.h>
#include "Area_V2.h"
#include "Biome_V2.h"
#include "LandGenerator_V2.h"

using namespace LandGenerator_V2;

LandGenerator::LandGenerator(Seed seed)
    : seed_(seed)
{
    mountLayers_.emplace_back(seed * 7, 64.0f);
    mountLayers_.emplace_back(seed * 13 + 6, 32.0f);
    mountLayers_.emplace_back(seed * 17 + 3, 20.0f);
    mountLayers_.emplace_back(seed * 37 + 1, 8.0f);

    fieldLayers_.emplace_back(seed * 5 + 2, 64.0f);
    fieldLayers_.emplace_back(seed * 11, 32.0f);

    mountHeights_ = { 15.0f, 5.0f, 2.0f, 1.0f };
    fieldHeights_ = { 3.0f, 2.0f };

    mountHeightSum_ = std::accumulate(std::begin(mountHeights_),
                                      std::end(mountHeights_),
                                      0.0f, std::plus<float>());
    fieldHeightSum_ = std::accumulate(std::begin(fieldHeights_),
                                      std::end(fieldHeights_),
                                      0.0f, std::plus<float>());
}

void LandGenerator::GenerateLand(Chunk *ck) const
{
    assert(ck != nullptr);

    Area area(seed_);
    area.Generate(ck->GetPosition().x, ck->GetPosition().z);

    Biome biome(seed_);

    int xBase = ck->GetXPosBase();
    int zBase = ck->GetZPosBase();
    for(int x = 0; x != CHUNK_SECTION_SIZE; ++x)
    {
        int gx = xBase + x;
        for(int z = 0; z != CHUNK_SECTION_SIZE; ++z)
        {
            int gz = zBase + z;

            float _h1 = 0.0f, _h2 = 0.0f;
            for(int i = 0; i != mountLayers_.size(); ++i)
                _h1 += mountHeights_[i] * mountLayers_[i].Get((float)gx, (float)gz);
            for(int i = 0; i != fieldLayers_.size(); ++i)
                _h2 += fieldHeights_[i] * fieldLayers_[i].Get((float)gx, (float)gz);
            _h1 = _h1 / mountHeightSum_;
            _h2 = _h2 / fieldHeightSum_;

            float baseHeight = 10.0f + area.GetResult(x, z).factor *
                biome.BaseHeight(area.GetResult(x, z).type);
            float variHeight = 10.0f + area.GetResult(x, z).factor *
                biome.VariHeight(area.GetResult(x, z).type);

            int h = static_cast<int>((std::max)(
                variHeight / 2.5f * _h2 + baseHeight,
                variHeight * _h1 + (variHeight / 5.0f + baseHeight - variHeight / 2.0f)));

            biome.SetBlockColumn(ck, area.GetResult(x, z).type, x, z, h);
        }
    }

    for(int x = 0; x != CHUNK_SECTION_SIZE; ++x)
    {
        for(int z = 0; z != CHUNK_SECTION_SIZE; ++z)
        {
            int H = ck->GetHeight(x, z);
            for(int y = 0; y <= H; ++y)
                ck->SetBlockLight(x, y, z, LIGHT_ALL_MIN);

            for(int y = H + 1; y < CHUNK_MAX_HEIGHT; ++y)
                ck->SetBlockLight(x, y, z, LIGHT_MIN_MIN_MIN_MAX);
        }
    }
}
