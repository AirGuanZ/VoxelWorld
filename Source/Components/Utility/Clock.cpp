/*================================================================
Filename: Clock.cpp
Date: 2017.10.16
Created by AirGuanZ
================================================================*/
#include <Windows.h>

#include "Clock.h"

Clock::Clock(void)
{
    LARGE_INTEGER freq;
    QueryPerformanceFrequency(&freq);
    ratio_ = 1000.0f / freq.QuadPart;

    Restart();
}

void Clock::Restart(void)
{
    LARGE_INTEGER tick;
    QueryPerformanceCounter(&tick);

    startTime_ = ratio_ * tick.QuadPart;
    lastTick_ = startTime_;
    elapsedTime_ = 0.0;
}

void Clock::Tick(void)
{
    LARGE_INTEGER tick;
    QueryPerformanceCounter(&tick);

    InternalFormat thisTick = ratio_ * tick.QuadPart;
    elapsedTime_ = thisTick - lastTick_;
    lastTick_ = thisTick;
}

float Clock::ElapsedTime(void) const
{
    return elapsedTime_;
}

float Clock::TotalTime(void) const
{
    return lastTick_ - startTime_;
}
