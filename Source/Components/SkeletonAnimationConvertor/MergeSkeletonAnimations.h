/*================================================================
Filename: MergeSkeletonAnimations.h
Date: 2018.2.14
Created by AirGuanZ
================================================================*/
#pragma once

#include <map>
#include <vector>

#include <SkeletonAnimation\SkeletonData.h>

struct SingleAnimation
{
    Skeleton::Skeleton skt;
    std::map<std::string, int> boneMap;
};

//根据std重排skt中的骨骼顺序
bool SortBones(const std::map<std::string, int> &std,
               const std::map<std::string, int> &act,
               Skeleton::Skeleton &skt);

//把一堆skeleton合并成一个
//要求parents和bongMap相同，合并的实际上是动画
bool MergeSkeletonData(std::vector<Skeleton::Skeleton> &skts,
                       Skeleton::Skeleton &output);
