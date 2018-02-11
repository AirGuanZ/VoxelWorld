/*================================================================
Filename: SkeletonDataLoader.h
Date: 2018.2.9
Created by AirGuanZ
================================================================*/
#ifndef VW_SKELETON_DATA_LOADER_H
#define VW_SKELETON_DATA_LOADER_H

#include <string>
#include <map>

#include "../Utility/Singleton.h"
#include "SkeletonData.h"

namespace Skeleton
{
    class SkeletonDataLoader : public Singleton<SkeletonDataLoader>
    {
    public:
        //ֻ������ΪArmature�Ľڵ��µĹ�����Ϣ
        bool LoadFromFile(const std::wstring &filename,
                          Skeleton &skeleton,
                          std::map<std::string, int> &boneIdx,
                          std::string &errMsg);
    };
}

#endif //VW_SKELETON_DATA_LOADER_H
