/*================================================================
Filename: Uncopiable.h
Date: 2018.1.13
Created by AirGuanZ
================================================================*/
#pragma once

class Uncopiable
{
public:
    Uncopiable(void) = default;
    Uncopiable(const Uncopiable&) = delete;
    Uncopiable(const Uncopiable&&) = delete;
};
