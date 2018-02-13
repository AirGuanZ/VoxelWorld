/*================================================================
Filename: SkeletonData.cpp
Date: 2018.2.9
Created by AirGuanZ
================================================================*/
#include <algorithm>
#include <numeric>

#include <iostream>

#include "SkeletonData.h"

void Skeleton::BoneAni::GetTransMatrix(float t, Matrix &mat) const
{
    if(keyframes.empty())
    {
        mat = Matrix::Identity;
        return;
    }

    if(t <= StartTime())
    {
        const Keyframe &kf = keyframes.front();
        mat = Matrix::CreateScale(kf.scale) *
              Matrix::CreateFromQuaternion(kf.rotate) *
              Matrix::CreateTranslation(kf.translate);
    }
    else if(t >= EndTime())
    {
        const Keyframe &kf = keyframes.back();
        mat = Matrix::CreateScale(kf.scale) *
              Matrix::CreateFromQuaternion(kf.rotate) *
              Matrix::CreateTranslation(kf.translate);
    }
    else
    {
        //IMPROVE：二分查找关键帧
        for(int i = 0; i != keyframes.size() - 1; ++i)
        {
            if(keyframes[i].time <= t && t <= keyframes[i + 1].time)
            {
                const Keyframe &kf0 = keyframes[i], &kf1 = keyframes[i + 1];
                float lerpT = (t - kf0.time) / (kf1.time - kf0.time);
                mat = Matrix::CreateScale(Vector3::Lerp(kf0.scale, kf1.scale, lerpT)) *
                      Matrix::CreateFromQuaternion(Quaternion::Lerp(kf0.rotate, kf1.rotate, lerpT)) *
                      Matrix::CreateTranslation(Vector3::Lerp(kf0.translate, kf1.translate, lerpT));
                break;
            }
        }
    }
}

void Skeleton::AniClip::GetTransMatrix(float t, std::vector<Matrix>& mats) const
{
    mats.resize(boneAnis.size());
    for(int i = 0; i != boneAnis.size(); ++i)
        boneAnis[i].GetTransMatrix(t, mats[i]);
}

void Skeleton::AniClip::UpdateStartEndTime(void)
{
    startTime = std::accumulate(
        std::begin(boneAnis), std::end(boneAnis), (std::numeric_limits<float>::max)(),
        [](float accu, const BoneAni &ani) { return (std::min)(accu, ani.StartTime()); });
    endTime = std::accumulate(
        std::begin(boneAnis), std::end(boneAnis), std::numeric_limits<float>::lowest(),
        [](float accu, const BoneAni &ani) { return (std::max)(accu, ani.EndTime()); });
}

void Skeleton::Skeleton::Initialize(std::vector<int> &&parents)
{
    parents_ = std::move(parents);
    aniClips_.clear();
}

void Skeleton::Skeleton::Clear(void)
{
    parents_.clear();
    aniClips_.clear();
}

bool Skeleton::Skeleton::AddClip(const std::string &name, AniClip &&clipData)
{
    auto it = aniClips_.find(name);
    if(it != aniClips_.end())
        return false;

    AniClip &clip = aniClips_[name] =
    {
        std::move(clipData.boneAnis),
        0.0f, 0.0f
    };
    clip.UpdateStartEndTime();

    return true;
}

bool Skeleton::Skeleton::GetTransMatrix(const std::string &clip, float t,
                                        std::vector<Matrix> &mats) const
{
    decltype(parents_)::size_type boneCnt = parents_.size();
    mats.resize(boneCnt);

    std::vector<Matrix> toParentTrans(boneCnt);
    std::vector<Matrix> toRootTrans(boneCnt);

    auto it = aniClips_.find(clip);
    if(it == aniClips_.end())
        return false;

    it->second.GetTransMatrix(t, toParentTrans);

    for(size_t i = 0; i < boneCnt; ++i)
    {
        if(parents_[i] < 0)
            toRootTrans[i] = toParentTrans[i];
        else
            toRootTrans[i] = toParentTrans[i] * toRootTrans[parents_[i]];
    }

    mats = std::move(toRootTrans);

    return true;
}
