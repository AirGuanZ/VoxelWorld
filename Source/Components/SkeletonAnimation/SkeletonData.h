/*================================================================
Filename: SkeletonData.h
Date: 2018.2.9
Created by AirGuanZ
================================================================*/
#pragma once

#include <cstdint>
#include <map>
#include <string>
#include <vector>

#include "..\Utility\Math.h"

namespace Skeleton
{
    //一块骨骼的一个关键帧数据
    struct Keyframe
    {
        float time;

        Vector3 scale;
        Vector3 translate;
        Quaternion rotate;
    };

    //一块骨骼的一个动作
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

    //一系列骨骼的一个动作
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

    //一系列骨骼的一组动作
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

        std::vector<int> &GetParents(void)
        {
            return parents_;
        }

        std::map<std::string, AniClip> &GetAniClips(void)
        {
            return aniClips_;
        }

        float GetEndTime(const std::string &aniName) const;

        void Scale(float time, float size, Skeleton &output);

        int GetBoneIndex(const std::string &boneName) const;

        void _setBoneMap(const std::map<std::string, int> &m)
        {
            boneMap_ = m;
        }

        const std::map<std::string, int> &_getBoneMap(void) const
        {
            return boneMap_;
        }

    private:
        friend class SkeletonDataLoader;

        std::vector<int> parents_;
        std::map<std::string, AniClip> aniClips_;

        std::map<std::string, int> boneMap_;
    };
}
