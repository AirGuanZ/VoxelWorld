/*================================================================
Filename: MergeSkeletonAnimations.cpp
Date: 2018.2.14
Created by AirGuanZ
================================================================*/
#include <algorithm>

#include "MergeSkeletonAnimations.h"

bool SortBones(const std::map<std::string, int> &std,
               const std::map<std::string, int> &act,
               Skeleton::Skeleton &skt)
{
    if(std.size() != act.size())
        return false;

    //指出act中idx为i的bone在std中是多少
    std::map<int, int> act2std;
    act2std[-1] = -1;
    for(auto itAct : act)
    {
        auto it = std.find(itAct.first);
        if(it == std.end())
            return false;
        act2std[itAct.second] = it->second;
    }

    std::vector<int> &oldParents = skt.GetParents();
    int boneCount = static_cast<int>(oldParents.size());
    std::vector<int> newParents(boneCount);

    //构造新parents数组
    for(int actIdx = 0; actIdx < boneCount; ++actIdx)
        newParents[act2std[actIdx]] = act2std[oldParents[actIdx]];
    oldParents = std::move(newParents);

    //对每个动画，构造新的bone anis
    for(auto &itClip : skt.GetAniClips())
    {
        Skeleton::AniClip &clip = itClip.second;
        std::vector<Skeleton::BoneAni> newBoneAnis(boneCount);

        for(int actIdx = 0; actIdx < boneCount; ++actIdx)
        {
            int stdIdx = act2std[actIdx];
            newBoneAnis[stdIdx].keyframes =
                std::move(clip.boneAnis[actIdx].keyframes);
        }

        clip.boneAnis = std::move(newBoneAnis);
    }

    return true;
}

bool MergeSkeletonData(std::vector<Skeleton::Skeleton> &skts,
                       Skeleton::Skeleton &output)
{
    auto boneMap = skts[0]._getBoneMap();
    output.Clear();
    if(skts.empty())
        return true;

    output.Initialize(std::move(skts[0].GetParents()));

    for(Skeleton::Skeleton &skt : skts)
    {
        for(auto &itClip : skt.GetAniClips())
        {
            if(!output.AddClip(itClip.first, std::move(itClip.second)))
                return false;
        }
    }

    output._setBoneMap(boneMap);

    return true;
}
