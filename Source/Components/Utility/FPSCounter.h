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
        clock_.Restart();
    }

    void Restart(void)
    {
        frameCnt_ = 0;
        fps_ = 0.0f;
        clock_.Restart();
    }

    void Tick(void)
    {
        clock_.Tick();
        if(clock_.TotalTime() >= 1000.0f)
        {
            fps_ = frameCnt_ / (clock_.TotalTime() / 1000.0f);
            frameCnt_ = 0;
            clock_.Restart();
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
    Clock clock_;
};
