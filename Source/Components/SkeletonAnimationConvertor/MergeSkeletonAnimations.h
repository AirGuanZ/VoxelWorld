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

//����std����skt�еĹ���˳��
bool SortBones(const std::map<std::string, int> &std,
               const std::map<std::string, int> &act,
               Skeleton::Skeleton &skt);

//��һ��skeleton�ϲ���һ��
//Ҫ��parents��bongMap��ͬ���ϲ���ʵ�����Ƕ���
bool MergeSkeletonData(std::vector<Skeleton::Skeleton> &skts,
                       Skeleton::Skeleton &output);
