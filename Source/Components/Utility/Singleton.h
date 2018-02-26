/*================================================================
Filename: Singleton.h
Date: 2018.1.12
Created by AirGuanZ
================================================================*/

/*
    �̰߳�ȫ�ĵ���ʵ��
    ������TӦ��public��ʽ�̳�Singleton<T>
    ����ʵ���ļ���д��
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
