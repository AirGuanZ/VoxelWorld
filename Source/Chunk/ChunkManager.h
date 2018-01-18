/*================================================================
Filename: ChunkManager.h
Date: 2018.1.18
Created by AirGuanZ
================================================================*/
#ifndef VW_CHUNK_MANAGER_H
#define VW_CHUNK_MANAGER_H

#include <mutex>
#include <set>
#include <string>
#include <unordered_map>

#include "../Utility/Math.h"
#include "../Utility/Uncopiable.h"
#include "Chunk.h"

/*
    Chunk���ݼ��ؼ�ģ�ʹ���
        ���߳�ά��һ��Position -> Chunk��ӳ��M����ֻ̨����Chunk�����ݼ��أ�������ģ�ʹ���
        
        setCentrePos
            ��һ��Chunk�򳬳����ط�Χ����ж��ʱ�������ʣ�µ�ģ�ʹ�������
            ��һ���µ�λ�ý�����ط�Χ��loadDistance��ʱ�����̴߳������������ɺ�̨��ɼ���
            ��һ���µ�λ�ý�����Ⱦ��Χ��renderDistance��ʱ�����̴߳�����Ӧ����Chunk��ģ������
            
        ������һ��ģ������ʱ������о�ģ�ͣ����š���ģ���������ʱ���滻����ģ��
        
        ��̨�������ʱ���ύ��������ɡ�����Ϣ�����̣߳����԰�������M
        ����Ѿ����ˣ�����ʧ�ܣ������ٴ���һ���������������ú�̨������
        ��һ��Chunk���ɹ�����Mʱ�����������Ⱦ��Χ�ڣ�������Ӧ��ģ������
        
        ģ�������Ϊ���࣬һ���ǡ���Ҫ���񡱣�һ���ǡ�����Ҫ����
        ��Ҫ������ÿһ֡������ش�������ֵ�ϸߣ������ȼ�����
        ����Ҫ������ֵ�ϵͣ������ȼ�����
        ÿ֡�������Ҫ����Ͳ���Ҫ��������֮�Ͳ�����һ����ֵ
*/

class ChunkManager
{
public:
    ChunkManager(void);
    ~ChunkManager(void);

    //���ص�Chunk�ڱ�֡�ھ�����ʧЧ
    Chunk *GetChunk(int ckX, int ckZ);

    Block &GetBlock(int blkX, int blkY, int blkZ);
    void SetBlock(int blkX, int blkY, int blkZ, const Block &blk);

private:
    void LoadChunk(int ckX, int ckZ);

private:
    int loadDistance_;
    int renderDistance_;
    IntVectorXZ centrePos_;

    std::unordered_map<IntVectorXZ, Chunk*, IntVectorXZHasher> chunks_;
    
    std::set<IntVector3> importantModelUpdates_;
    std::set<IntVector3> unimportantModelUpdates_;
};

inline int BlockXZ_To_ChunkXZ(int blk)
{
    return (blk + 0x40000000) / CHUNK_SECTION_SIZE + 0x40000000 / CHUNK_SECTION_SIZE;
}

inline int BlockXZ_To_BlockXZInChunk(int blk)
{
    return (blk + 0x40000000) % CHUNK_SECTION_SIZE;
}

inline int BlockY_To_ChunkSectionIndex(int blk)
{
    return blk / CHUNK_SECTION_SIZE;
}

inline int BlockY_To_BlockYInChunkSection(int blk)
{
    return blk % CHUNK_SECTION_SIZE;
}

inline int ChunkXZ_To_BlockXZ(int ck)
{
    return ck * CHUNK_SECTION_SIZE;
}

inline int ChunkSectionIndex_To_BlockY(int cks)
{
    return cks * CHUNK_SECTION_SIZE;
}

#endif //VW_CHUNK_MANAGER_H
