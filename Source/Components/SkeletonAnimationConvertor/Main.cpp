/*================================================================
Filename: Main.cpp
Date: 2018.2.14
Created by AirGuanZ
================================================================*/
#include <codecvt>
#include <iostream>
#include <locale>
#include <string>

#include <SkeletonAnimation/SkeletonDataLoader.h>
#include <Utility/ConfigFile.h>

#include "MergeSkeletonAnimations.h"

struct InputUnit
{
    std::wstring filename;
    std::string newAniName;
};

bool Input(const std::wstring &confFilename,
           std::vector<InputUnit> &output,
           float &timeFactor,
           std::string &errMsg)
{
    output.clear();
    errMsg = "";

    std::wstring_convert<std::codecvt_utf8<wchar_t>> strConv;

    ConfigFile file(confFilename);
    if(!file)
    {
        errMsg = "Failed to open file" + strConv.to_bytes(confFilename);
        return false;
    }

    int fileCount = std::stoi(file("Global", "FileCount"));
    timeFactor = std::stof(file("Global", "TimeFactor"));

    output.resize(fileCount);
    for(int i = 0; i < fileCount; ++i)
    {
        std::string idxStr = std::to_string(i);
        output[i].filename = strConv.from_bytes(file("Global", "Filename" + idxStr));
        output[i].newAniName = file("Global", "NewAnimationName" + idxStr);

        if(output[i].filename.empty())
        {
            errMsg = "Filename " + idxStr + " not found";
            output.clear();
            return false;
        }
    }

    return true;
}

bool Load(std::vector<InputUnit> &input,
          float timeFactor,
          std::vector<Skeleton::Skeleton> &skts,
          std::vector<std::map<std::string, int>> &boneMaps,
          std::string &errMsg)
{
    skts.clear();
    boneMaps.clear();
    errMsg = "";

    skts.resize(input.size());
    boneMaps.resize(input.size());
    for(size_t i = 0; i < input.size(); ++i)
    {
        if(!Skeleton::SkeletonDataLoader::GetInstance().LoadFromRawFile(
            input[i].filename, timeFactor, skts[i], boneMaps[i], input[i].newAniName, errMsg))
        {
            skts.clear();
            boneMaps.clear();
            return false;
        }
    }

    return true;
}

int main(void)
{
    try
    {
        std::string errMsg;
        std::wstring confFilename;

        std::cout << "Enter configure: ";
        std::wcin >> confFilename;

        std::vector<InputUnit> files;
        float timeFactor;
        if(!Input(confFilename, files, timeFactor, errMsg))
        {
            std::cout << errMsg << std::endl;
            return -1;
        }

        std::vector<Skeleton::Skeleton> skts;
        std::vector<std::map<std::string, int>> boneMaps;
        if(!Load(files, timeFactor, skts, boneMaps, errMsg))
        {
            std::cout << errMsg << std::endl;
            return -1;
        }

        if(skts.empty())
        {
            std::cout << "Empty input" << std::endl;
            return -1;
        }

        assert(skts.size() == boneMaps.size());
        for(size_t i = 1; i < skts.size(); ++i)
        {
            if(!SortBones(boneMaps[0], boneMaps[i], skts[i]))
            {
                std::cout << "Failed to sort bones for skeleton " << i << std::endl;
                return -1;
            }
        }

        Skeleton::Skeleton finalSkt;
        if(!MergeSkeletonData(skts, finalSkt))
        {
            std::cout << "Failed to merge" << std::endl;
            return -1;
        }

        if(!Skeleton::SkeletonDataLoader::GetInstance().SaveToVWFile(
            L"output.vwani", finalSkt, boneMaps[0]))
        {
            std::cout << "Failed to save" << std::endl;
            return -1;
        }

        std::cout << "Successfully merged!" << std::endl;
    }
    catch(const std::invalid_argument &err)
    {
        std::cerr << err.what() << std::endl;
        return -1;
    }

    return 0;
}
