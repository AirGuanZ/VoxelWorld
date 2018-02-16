/*================================================================
Filename: ChunkDataPool.h
Date: 2018.1.29
Created by AirGuanZ
================================================================*/
#ifndef VW_CHUNK_DATA_POOL_H
#define VW_CHUNK_DATA_POOL_H

#include <mutex>
#include <unordered_map>
#include <vector>

#include <Utility/LinkedMap.h>
#include <Utility/Uncopiable.h>

#include "Chunk.h"

/*
LRU���������ݣ����Ӵ�СӦ���Դ�����Ҫ����������������
*/
class ChunkDataPool : public Uncopiable
{
public:
    ChunkDataPool(size_t maxDataCnt);
    ~ChunkDataPool(void);

    void Destroy(void);

    //IMPROVE��Ŀǰ�������ݻ�����������ͼ�������ݵ��߳�
    //�ĳ�����Ҫ���ݵ��߳��Լ���������ʽ
    //�����������Ҫ��Ϊ���ӵ�ͬ������
    bool GetChunk(Chunk &ck);

    //���add֮��洢��������������maxDataCnt��
    //��ɾ�����һ��ʹ�þ����Զ������
    void AddChunk(Chunk *ck);

private:
    size_t maxDataCnt_;

    std::mutex mapMutex_;
    LinkedMap<IntVectorXZ, Chunk*> map_;
};

#endif //VW_CHUNK_DATA_POOL_H
