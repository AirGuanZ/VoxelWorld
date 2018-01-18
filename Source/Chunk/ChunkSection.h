/*================================================================
Filename: ChunkSection.h
Date: 2018.1.15
Created by AirGuanZ
================================================================*/
#ifndef VW_CHUNK_SECTION_H
#define VW_CHUNK_SECTION_H

#include "../Block/Block.h"
#include "../Model/BasicModel.h"
#include "../Renderer/BasicRenderer.h"
#include "../Utility/D3D11Header.h"
#include "../Utility/Uncopiable.h"

/*
    ChunkSection�е����ݷ�Ϊ�����֣�

    1. Block����
        �ⲿ������һ��ʼ��loader�߳�д�룬�������̺߳�������߳�д��
        Block�ı䵼�µ�ģ���ؽ�Ҳ�����߳����
        ����ʱ�����߳��ύ���������loader�߳�

    2. Model����
        ��Chunk��Model��loader�̸߳��𴴽�
        Model��������Զ��loader�̸߳���
        ������߳�needNewModel����ô�Ϳ���һ��block + ��ΧһȦblock�����ݣ�
        �ÿ�����������Ϊ����model��loader���������Դ

    ��֤��loader�������������߳�����κ�ChunkSection������
*/

constexpr int CHUNK_SECTION_SIZE = 16;

struct ChunkSectionModels
{
    BasicModel basicModel[BASIC_RENDERER_TEXTURE_NUM];
};

class ChunkSection : public Uncopiable
{
public:
    friend class Chunk;

    using ChunkSectionData = Block[CHUNK_SECTION_SIZE][CHUNK_SECTION_SIZE][CHUNK_SECTION_SIZE];

    ChunkSection(void);
    ~ChunkSection(void);

    void SetBlock(int xInSection, int yInSection, int zInSection, const Block &block);
    const Block &GetBlock(int xInSection, int yInSection, int zInSection) const;
    Block &GetBlock(int xInSection, int yInSection, int zInSection);

    bool IsModelsAvailable(void) const;

    const ChunkSectionData &GetBlockData(void) const;

private:
    ChunkSectionData blocks_;

    ChunkSectionModels *models_;
    bool blockChanged_;
};

#endif //VW_CHUNK_SECTION_H
