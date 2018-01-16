/*================================================================
Filename: ChunkSection.h
Date: 2018.1.15
Created by AirGuanZ
================================================================*/
#ifndef VW_CHUNK_SECTION_H
#define VW_CHUNK_SECTION_H

#include "../Model/BasicModel.h"
#include "../Utility/D3D11Header.h"
#include "../Utility/Uncopiable.h"
#include "Block.h"

/*
    ChunkSection�е����ݷ�Ϊ�����֣�

    1. Block����
        �ⲿ������һ��ʼ��loader�߳�д�룬�������̺߳�������߳�д��
        ����ʱ�����߳��ύ���������loader�߳�

    2. Model����
        �ⲿ��������Զ��loader�̸߳��𴴽�������
        ������߳�needNewModel����ô�Ϳ���һ��block + ��ΧһȦblock�����ݣ�
        �ÿ�����������Ϊ����model��loader���������Դ

    ��֤��loader�������������߳�����κ�ChunkSection������
*/

constexpr int CHUNK_SECTION_SIZE = 16;

class ChunkSection : public Uncopiable
{
public:
    friend class Chunk;

    ChunkSection(void);
    ~ChunkSection(void);

    void SetBlock(int xInSection, int yInSection, int zInSection, const Block &block);
    const Block &GetBlock(int xInSection, int yInSection, int zInSection) const;
    Block &GetBlock(int xInSection, int yInSection, int zInSection);

    bool IsModelsAvailable(void) const;

    void GetBlockData(Block (&data)[CHUNK_SECTION_SIZE][CHUNK_SECTION_SIZE][CHUNK_SECTION_SIZE]) const;
    
    void GetPosXFaceBlockData(Block (&data)[CHUNK_SECTION_SIZE][CHUNK_SECTION_SIZE]) const;
    void GetNegXFaceBlockData(Block (&data)[CHUNK_SECTION_SIZE][CHUNK_SECTION_SIZE]) const;

    void GetPosYFaceBlockData(Block (&data)[CHUNK_SECTION_SIZE][CHUNK_SECTION_SIZE]) const;
    void GetNegYFaceBlockData(Block (&data)[CHUNK_SECTION_SIZE][CHUNK_SECTION_SIZE]) const;

    void GetPosZFaceBlockData(Block (&data)[CHUNK_SECTION_SIZE][CHUNK_SECTION_SIZE]) const;
    void GetNegZFaceBlockData(Block (&data)[CHUNK_SECTION_SIZE][CHUNK_SECTION_SIZE]) const;

private:
    Block blocks_[CHUNK_SECTION_SIZE][CHUNK_SECTION_SIZE][CHUNK_SECTION_SIZE];

    BasicModel *basicModel_;
    bool blockChanged_;
};

#endif //VW_CHUNK_SECTION_H
