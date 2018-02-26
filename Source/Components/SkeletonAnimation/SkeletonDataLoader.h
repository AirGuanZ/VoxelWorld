/*================================================================
Filename: SkeletonDataLoader.h
Date: 2018.2.9
Created by AirGuanZ
================================================================*/
#pragma once

#include <string>
#include <map>

#include "../Utility/Singleton.h"
#include "SkeletonData.h"

namespace Skeleton
{
    class SkeletonDataLoader : public Singleton<SkeletonDataLoader>
    {
    public:
        //������ΪArmature�Ľڵ��µĹ�����Ϣ
        bool LoadFromRawFile(const std::wstring &filename,
                             float timeFactor,
                             Skeleton &skeleton,
                             std::map<std::string, int> &boneIdx,
                             const std::string &defaultClipName,
                             std::string &errMsg);
        bool LoadFromVWFile(const std::wstring &filename,
                            float timeFactor,
                            float sizeFactor,
                            Skeleton &skeleton,
                            std::map<std::string, int> &boneMap,
                            std::string &errMsg);
        bool SaveToVWFile(const std::wstring &filename,
                          const Skeleton &skeleton,
                          const std::map<std::string, int> &boneMap);
    };
}
