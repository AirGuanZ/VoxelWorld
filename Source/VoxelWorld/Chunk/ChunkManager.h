/*================================================================
Filename: ChunkManager.h
Date: 2018.1.18
Created by AirGuanZ
================================================================*/
#ifndef VW_CHUNK_MANAGER_H
#define VW_CHUNK_MANAGER_H

#include <map>
#include <mutex>
#include <deque>
#include <string>
#include <unordered_map>
#include <unordered_set>

#include <Utility/Math.h>
#include <Utility/Uncopiable.h>

#include "../Block/BlockInfoManager.h"
#include "../Camera/Camera.h"
#include "Chunk.h"
#include "ChunkLoader.h"

/*
    Chunk���ݼ��ؼ�ģ�ʹ���
        ���߳�ά��һ��Position -> Chunk��ӳ��M����ֻ̨����Chunk�����ݼ��أ�������ģ�ʹ���
        
        setCentrePos
            ��һ��Chunk�򳬳����ط�Χ����ж��ʱ�������ʣ�µ�ģ�ʹ�������
            ��һ���µ�λ�ý�����ط�Χ��loadDistance��ʱ�����̴߳������������ɺ�̨��ɼ���
            ��һ���µ�λ�ý�����Ⱦ��Χ��renderDistance��ʱ�����̴߳�����Ӧ����Chunk��ģ������
        implemented in SetCentrePosition
        
        ��̨�������ʱ���ύ��������ɡ�����Ϣ�����̣߳����԰�������M
        ����Ѿ����ˣ�����ʧ�ܣ������ٴ���һ���������������ú�̨������
        ��һ��Chunk���ɹ�����Mʱ�����������Ⱦ��Χ�ڣ�������Ӧ��ģ������
        implemented in AddChunkData & ProcessChunkLoaderMessages

        ������һ��ģ������ʱ������о�ģ�ͣ����š���ģ���������ʱ���滻����ģ��
        implemented in AddSectionModel

        ģ�������Ϊ���࣬һ���ǡ���Ҫ���񡱣�һ���ǡ�����Ҫ����
        ��Ҫ������ÿһ֡������ش�������ֵ�ϸߣ������ȼ�����
        ����Ҫ������ֵ�ϵͣ������ȼ�����
        ÿ֡�������Ҫ����Ͳ���Ҫ��������֮�Ͳ�����һ����ֵ
        implemented in ProcessModelUpdates
*/

class ChunkManager
{
public:
    ChunkManager(int loadDistance, int renderDistance, int unloadDistance);
    ~ChunkManager(void);

    void StartLoading(int loaderCount);
    void Destroy(void);

    //���ص�Chunk�ڱ�֡�ھ�����ʧЧ
    Chunk *GetChunk(int ckX, int ckZ)
    {
        auto it = chunks_.find({ ckX, ckZ });
        if(it != chunks_.end())
            return it->second;

        LoadChunk(ckX, ckZ);

        assert((chunks_[{ ckX, ckZ }] != nullptr));
        return chunks_[{ ckX, ckZ }];
    }

    BlockType GetBlockType(int blkX, int blkY, int blkZ)
    {
        if(blkY < 0 || blkY >= CHUNK_MAX_HEIGHT)
        {
            return BlockType::Air;
        }
        return GetChunk(BlockXZ_To_ChunkXZ(blkX), BlockXZ_To_ChunkXZ(blkZ))
            ->GetBlockType(
                BlockXZ_To_BlockXZInChunk(blkX),
                blkY,
                BlockXZ_To_BlockXZInChunk(blkZ));
    }

    BlockLight GetBlockLight(int blkX, int blkY, int blkZ)
    {
        if(blkY < 0 || blkY >= CHUNK_MAX_HEIGHT)
            return LIGHT_ALL_MAX;
        return GetChunk(BlockXZ_To_ChunkXZ(blkX), BlockXZ_To_ChunkXZ(blkZ))
            ->GetBlockLight(
                BlockXZ_To_BlockXZInChunk(blkX),
                blkY,
                BlockXZ_To_BlockXZInChunk(blkZ));
    }

    Block GetBlock(int blkX, int blkY, int blkZ)
    {
        if(blkY < 0 || blkY >= CHUNK_MAX_HEIGHT)
        {
            return { BlockType::Air, LIGHT_ALL_MAX };
        }

        IntVectorXZ ck = BlockXZ_To_ChunkXZ({ blkX, blkZ });
        IntVectorXZ cb = BlockXZ_To_BlockXZInChunk({ blkX, blkZ });
        return GetChunk(ck.x, ck.z)->GetBlock(cb.x, blkY, cb.z);
    }

    void SetBlockType(int blkX, int blkY, int blkZ, BlockType type)
    {
        if(blkY < 0 || blkY >= CHUNK_MAX_HEIGHT)
            return;

        Chunk *ck = GetChunk(BlockXZ_To_ChunkXZ(blkX), BlockXZ_To_ChunkXZ(blkZ));

        int cx = BlockXZ_To_BlockXZInChunk(blkX);
        int cz = BlockXZ_To_BlockXZInChunk(blkZ);

        ck->SetBlockType(cx, blkY, cz, type);

        if(blkY >= ck->heightMap[Chunk::XZ(cx, cz)])
        {
            int newH = CHUNK_MAX_HEIGHT - 1;
            while(newH > 0 && ck->GetBlockType(cx, newH, cz) == BlockType::Air)
                --newH;

            if(newH != ck->GetHeight(cx, cz))
            {
                int L, H;
                std::tie(L, H) = std::minmax(newH, ck->heightMap[Chunk::XZ(cx, cz)]);

                while(H >= L)
                    UpdateLight(blkX, H--, blkZ);

                ck->heightMap[Chunk::XZ(cx, cz)] = newH;
            }
        }

        if(BlockInfoManager::GetInstance().IsGlow(type))
            RemoveLightSource(blkX, blkY, blkZ);
        else
            UpdateLight(blkX, blkY, blkZ);
    }

    void RemoveLightSource(int x, int y, int z);

    void UpdateLight(int x, int y, int z);

    //�Ը��������ߺ����еķ�����
    //����true���ҽ�����maxLen���ҵ�������PickBlockFunc�ķ���
    //������ֵΪtrue����
    //      blk��ŵ�һ�����������ķ���
    //      faceָ���Ǵ���һ�����÷����
    //      rtPosָ���÷�����Block����ϵ�е�λ��
    //��ԭ��������������ķ����ڲ�����faceֵΪ����ֵ�е���һ��
    //ע�⣺λ��Խ���dummyBlockҲ�������һ�󽻹���
    using PickBlockFunc = bool(*)(const Block&);
    bool PickBlock(const Vector3 &origin, const Vector3 &dir,
                   float maxLen, float step, PickBlockFunc func,
                   Block &blk, BlockFace &face, IntVector3 &rtPos);

    bool InRenderRange(int ckX, int ckZ)
    {
        return std::abs(ckX - centrePos_.x) <= renderDistance_ &&
            std::abs(ckZ - centrePos_.z) <= renderDistance_;
    }

    bool InLoadingRange(int ckX, int ckZ)
    {
        return centrePos_.x - loadDistance_ <= ckX && ckX <= centrePos_.x + loadDistance_ &&
            centrePos_.z - loadDistance_ <= ckZ && ckZ <= centrePos_.z + loadDistance_;
    }

    bool InUnloadingRange(int ckX, int ckZ)
    {
        return centrePos_.x - unloadDistance_ <= ckX && ckX <= centrePos_.x + unloadDistance_ &&
            centrePos_.z - unloadDistance_ <= ckZ && ckZ <= centrePos_.z + unloadDistance_;
    }

    void SetCentrePosition(int cenCkX, int cenCkZ);

    void MakeSectionModelInvalid(int xSection, int ySection, int zSection);

    void ProcessChunkLoaderMessages(void);

    void ProcessModelUpdates(void);

    void Render(const Camera &cam, ChunkSectionRenderQueue *renderQueue);

    bool DetectCollision(const Vector3 &pnt);
    bool DetectCollision(const AABB &aabb);

private:
    //����һ�����غõ�Chunk
    void AddChunkData(Chunk *ck);
    //����һ�������õ�Model
    void AddSectionModel(const IntVector3 &pos, ChunkSectionModels *models);
    //���������̼߳�����������
    void LoadChunk(int ckX, int ckZ);

    void ComputeModelUpdates(int x, int y, int z, std::unordered_set<IntVector3, IntVector3Hasher> &updates);

private:
    int loadDistance_;
    int renderDistance_;
    int unloadDistance_;
    IntVectorXZ centrePos_;

    std::unordered_map<IntVectorXZ, Chunk*, IntVectorXZHasher> chunks_;
    
    std::unordered_set<IntVector3, IntVector3Hasher> modelUpdates_;

    ChunkLoader ckLoader_;
};

#endif //VW_CHUNK_MANAGER_H
