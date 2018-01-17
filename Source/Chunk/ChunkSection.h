/*================================================================
Filename: ChunkSection.h
Date: 2018.1.15
Created by AirGuanZ
================================================================*/
#ifndef VW_CHUNK_SECTION_H
#define VW_CHUNK_SECTION_H

#include "../Block/Block.h"
#include "../Model/BasicModel.h"
#include "../Utility/D3D11Header.h"
#include "../Utility/Uncopiable.h"

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

    using ChunkFace = Block[CHUNK_SECTION_SIZE][CHUNK_SECTION_SIZE];
    using ChunkData = Block[CHUNK_SECTION_SIZE][CHUNK_SECTION_SIZE][CHUNK_SECTION_SIZE];

    ChunkSection(void);
    ~ChunkSection(void);

    void SetBlock(int xInSection, int yInSection, int zInSection, const Block &block);
    const Block &GetBlock(int xInSection, int yInSection, int zInSection) const;
    Block &GetBlock(int xInSection, int yInSection, int zInSection);

    bool IsModelsAvailable(void) const;

    void GetBlockData(ChunkData &data) const;

    void GetPosXFaceBlockData(ChunkFace &data) const;
    void GetNegXFaceBlockData(ChunkFace &data) const;

    void GetPosYFaceBlockData(ChunkFace &data) const;
    void GetNegYFaceBlockData(ChunkFace &data) const;

    void GetPosZFaceBlockData(ChunkFace &data) const;
    void GetNegZFaceBlockData(ChunkFace &data) const;

private:
    ChunkData blocks_;

    BasicModel *basicModel_;
    bool blockChanged_;
};

#endif //VW_CHUNK_SECTION_H
