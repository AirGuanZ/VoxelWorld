/*================================================================
Filename: Uncopiable.h
Date: 2018.1.13
Created by AirGuanZ
================================================================*/
#ifndef VW_UNCOPIABLE_H
#define VW_UNCOPIABLE_H

class Uncopiable
{
public:
    Uncopiable(void) = default;
    Uncopiable(const Uncopiable&) = delete;
    Uncopiable(const Uncopiable&&) = delete;
    Uncopiable &operator=(const Uncopiable&) = delete;
    Uncopiable &operator=(const Uncopiable&&) = delete;
};

#endif //VW_UNCOPIABLE_H
