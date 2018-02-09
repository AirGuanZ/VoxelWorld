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
    struct BasicVertex
    {
        Vector3 pos;
        Vector3 nor;
        Vector2 uv;
        Vector3 boneWeights;
        std::uint8_t boneIndices[4];
    };

    struct Keyframe
    {
        float time;

        Vector3 scale;
        Vector3 translate;
        Quaternion rotate;
    };

    struct BoneAni
    {
        void GetTransMatrix(float t, Matrix &mat) const;

        float StartTime(void) const
        {
            assert(keyframes.size());
            return keyframes.front().time;
        }

        float EndTime(void) const
        {
            assert(keyframes.size());
            return keyframes.back().time;
        }

        std::vector<Keyframe> keyframes;
    };

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

    class Skeleton
    {
    public:
        void Initialize(std::vector<int> &&parents,
                        std::vector<Matrix> &&offsets);
        void Clear(void);

        bool AddClip(const std::string &name, AniClip &&clipData);

        bool GetTransMatrix(const std::string &clip, float t,
                            std::vector<Matrix> &mats) const;

    private:
        std::vector<int> parents_;
        std::vector<Matrix> offsets_;
        std::map<std::string, AniClip> aniClips_;
    };
}

#endif //VW_SKELETON_DATA_H
