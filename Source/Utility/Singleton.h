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

#ifndef VW_SINGLETON_H
#define VW_SINGLETON_H

#include <cassert>

template<typename SingletonClassName>
class Singleton
{
private:
    static SingletonClassName *singletonInstancePtr_;

    friend class SingletonInstanceDeleter;
    static class SingletonInstanceDeleter
    {
    public:
        ~SingletonInstanceDeleter(void)
        {
            if(singletonInstancePtr_)
                delete singletonInstancePtr_;
        }
    } singletonInstanceDeleter_;

protected:
    Singleton(void)
    {

    }

    ~Singleton(void)
    {

    }

public:
    static SingletonClassName *GetInstancePtr(void)
    {
        assert(singletonInstancePtr_ != nullptr);
        return singletonInstancePtr_;
    }

    static SingletonClassName &GetInstance(void)
    {
        return *GetInstancePtr();
    }
};

#define SINGLETON_CLASS_DEFINITION(SingletonClassName) \
    SingletonClassName *Singleton<SingletonClassName>::singletonInstancePtr_ = new SingletonClassName; \
    typename Singleton<SingletonClassName>::SingletonInstanceDeleter \
    Singleton<SingletonClassName>::singletonInstanceDeleter_

#endif //VW_SINGLETON_H
