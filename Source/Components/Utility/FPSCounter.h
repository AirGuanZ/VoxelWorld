/*================================================================
Filename: FPSCounter.h
Date: 2018.1.27
Created by AirGuanZ
================================================================*/
#pragma once

#include "Clock.h"
#include "Uncopiable.h"

class FPSCounter : public Uncopiable
{
public:
    FPSCounter(void)
        : fps_(0.0f), frameCnt_(0)
    {

    }

    void Restart(void)
    {
        frameCnt_ = 0;
        fps_ = 0.0f;
    }

    void Tick(float dT)
    {
        totalTime_ += dT;
        if(totalTime_ >= 1000.0f)
        {
            int sec = 0;
            while(totalTime_ >= 1000.0f)
            {
                ++sec;
                totalTime_ -= 1000.0f;
            }
            fps_ = frameCnt_ / static_cast<float>(sec);
            frameCnt_ = 0;
        }
        else
            ++frameCnt_;
    }

    float GetFPS(void) const
    {
        return fps_;
    }

private:
    int frameCnt_;
    float fps_;
    float totalTime_;
};
