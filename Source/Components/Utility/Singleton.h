/*================================================================
Filename: Singleton.h
Date: 2018.1.12
Created by AirGuanZ
================================================================*/

/*
    线程安全的单例实现
    单例类T应以public方式继承Singleton<T>
    并在实现文件中写下
        SINGLETON_CLASS_DEFINITION(T);
*/

#pragma once

#include <cassert>

#include "Uncopiable.h"

template<typename SingletonClassName>
class Singleton : public Uncopiable
{
private:
    static SingletonClassName singletonInstance_;

protected:
    Singleton(void)
    {

    }

    ~Singleton(void)
    {

    }

public:
    static SingletonClassName &GetInstance(void)
    {
        return singletonInstance_;
    }
};

#define SINGLETON_CLASS_DEFINITION(SingletonClassName) \
    SingletonClassName Singleton<SingletonClassName>::singletonInstance_
