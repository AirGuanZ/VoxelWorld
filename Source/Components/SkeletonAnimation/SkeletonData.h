/*================================================================
Filename: SkeletonData.h
Date: 2018.2.9
Created by AirGuanZ
================================================================*/
#ifndef VW_SKELETON_DATA_H
#define VW_SKELETON_DATA_H

#include <cstdint>
#include <map>
#include <string>
#include <vector>

#include "../Utility/Math.h"

namespace Skeleton
{
    //һ�������һ���ؼ�֡����
    struct Keyframe
    {
        float time;

        Vector3 scale;
        Vector3 translate;
        Quaternion rotate;
    };

    //һ�������һ������
    struct BoneAni
    {
        void GetTransMatrix(float t, Matrix &mat) const;

        float StartTime(void) const
        {
            return keyframes.size() ? keyframes.front().time : 0.0f;
        }

        float EndTime(void) const
        {
            return keyframes.size() ? keyframes.back().time : 1000.0f;
        }

        std::vector<Keyframe> keyframes;
    };

    //һϵ�й�����һ������
    struct AniClip
    {
        void GetTransMatrix(float t, std::vector<Matrix> &mats) const;

        void UpdateStartEndTime(void);

        float Start(void) const
        {
            return startTime;
        }

        float End(void) const
        {
            return endTime;
        }
        
        std::vector<BoneAni> boneAnis;
        
        float startTime, endTime;
    };

    //һϵ�й�����һ�鶯��
    class Skeleton
    {
    public:
        void Initialize(std::vector<int> &&parents);
        void Clear(void);

        bool AddClip(const std::string &name, AniClip &&clipData);

        bool GetTransMatrix(const std::string &clip, float t,
                            std::vector<Matrix> &mats) const;

        const AniClip *GetAniClip(const std::string &name) const
        {
            auto it = aniClips_.find(name);
            return it != aniClips_.end() ? &it->second : nullptr;
        }

    private:
        std::vector<int> parents_;
        std::map<std::string, AniClip> aniClips_;
    };
}

#endif //VW_SKELETON_DATA_H
