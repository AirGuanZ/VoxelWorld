/*================================================================
Filename: SkeletonDataLoader.h
Date: 2018.2.9
Created by AirGuanZ
================================================================*/
#ifndef VW_SKELETON_DATA_LOADER_H
#define VW_SKELETON_DATA_LOADER_H

#include <string>

#include "../Utility/Singleton.h"
#include "SkeletonData.h"

namespace Skeleton
{
    class SkeletonDataLoader : public Singleton<SkeletonDataLoader>
    {
    public:
        bool LoadFromFile(const std::wstring &filename,
                          Skeleton &skeleton,
                          std::vector<BasicVertex> &vertices,
                          std::string &errMsg);
    };
}

#endif //VW_SKELETON_DATA_LOADER_H
