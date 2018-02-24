/*================================================================
Filename: ChunkManager.cpp
Date: 2018.1.18
Created by AirGuanZ
================================================================*/
#include <cstdlib>
#include <limits>
#include <mutex>

#include <Utility/HelperFunctions.h>

#include <Block/BlockInfoManager.h>
#include "ChunkLoader.h"
#include "ChunkManager.h"
#include "ChunkModelBuilder.h"

ChunkManager::ChunkManager(int loadDistance,
                           int renderDistance,
                           int unloadDistance)
    : loadDistance_(loadDistance),
      renderDistance_(renderDistance),
      unloadDistance_(unloadDistance),
      ckLoader_((loadDistance + 2) * (loadDistance + 2))
{
    centrePos_.x = (std::numeric_limits<decltype(centrePos_.x)>::min)();
    centrePos_.z = (std::numeric_limits<decltype(centrePos_.z)>::min)();
}

ChunkManager::~ChunkManager(void)
{
    Destroy();
}

void ChunkManager::StartLoading(int loaderCount)
{
    ckLoader_.Initialize(loaderCount);
}

void ChunkManager::Destroy(void)
{
    ckLoader_.Destroy();
    ProcessChunkLoaderMessages();

    for(auto it : chunks_)
        Helper::SafeDeleteObjects(it.second);
    chunks_.clear();
    modelUpdates_.clear();
}

namespace
{
    inline void UpdateMinDis(float &minDis, BlockFace &face, float newDis, BlockFace newFace)
    {
        if(minDis < newDis)
        {
            minDis = newDis;
            face = newFace;
        }
    }
}

//IMPROVE
void ChunkManager::RemoveLightSource(int x, int y, int z)
{
    UpdateLight(x, y, z);
}

void ChunkManager::UpdateLight(int x, int y, int z)
{
    std::deque<IntVector3> pgQueue = { { x, y, z } };
    while(pgQueue.size())
    {
        IntVector3 pos = pgQueue.front();
        pgQueue.pop_front();

        int ckX = BlockXZ_To_ChunkXZ(pos.x);
        int ckZ = BlockXZ_To_ChunkXZ(pos.z);

        int blkX = BlockXZ_To_BlockXZInChunk(pos.x);
        int blkZ = BlockXZ_To_BlockXZInChunk(pos.z);

        Chunk *ck = GetChunk(ckX, ckZ);
        Block blk = GetBlock(pos.x, pos.y, pos.z);
        const BlockInfo &blkInfo = BlockInfoManager::GetInstance().GetBlockInfo(blk.type);
        int lightDec = blkInfo.lightDec;

        BlockLight pX = GetBlockLight(pos.x + 1, pos.y, pos.z),
                   nX = GetBlockLight(pos.x - 1, pos.y, pos.z),
                   pY = GetBlockLight(pos.x, pos.y + 1, pos.z),
                   nY = GetBlockLight(pos.x, pos.y - 1, pos.z),
                   pZ = GetBlockLight(pos.x, pos.y, pos.z + 1),
                   nZ = GetBlockLight(pos.x, pos.y, pos.z - 1);

        BlockLight newLight = MakeLight(
            blkInfo.lightEmission.x,
            blkInfo.lightEmission.y,
            blkInfo.lightEmission.z,
            pos.y > ck->GetHeight(blkX, blkZ) ? LIGHT_COMPONENT_MAX : LIGHT_COMPONENT_MIN);

        auto Max6 = [=](int ori, int _0, int _1, int _2, int _3, int _4, int _5) -> int
        {
            return static_cast<std::uint8_t>((std::max)(
            { (std::max)({ _0, _1, _2, _3, _4, _5 }) - lightDec, ori, static_cast<int>(LIGHT_COMPONENT_MIN) }));
        };

        newLight = SetRed(newLight, Max6(GetRed(newLight), GetRed(pX), GetRed(nX),
            GetRed(pY), GetRed(nY),
            GetRed(pZ), GetRed(nZ)));
        newLight = SetGreen(newLight, Max6(GetGreen(newLight), GetGreen(pX), GetGreen(nX),
            GetGreen(pY), GetGreen(nY),
            GetGreen(pZ), GetGreen(nZ)));
        newLight = SetBlue(newLight, Max6(GetBlue(newLight), GetBlue(pX), GetBlue(nX),
            GetBlue(pY), GetBlue(nY),
            GetBlue(pZ), GetBlue(nZ)));
        newLight = SetSunlight(newLight, Max6(GetSunlight(newLight), GetSunlight(pX), GetSunlight(nX),
            GetSunlight(pY), GetSunlight(nY),
            GetSunlight(pZ), GetSunlight(nZ)));

        if(newLight != blk.light)
        {
            ck->SetBlockLight(blkX, pos.y, blkZ, newLight);
            pgQueue.push_front({ pos.x + 1, pos.y, pos.z });
            pgQueue.push_front({ pos.x - 1, pos.y, pos.z });
            if(pos.y < CHUNK_MAX_HEIGHT - 1)
                pgQueue.push_front({ pos.x, pos.y + 1, pos.z });
            if(pos.y > 0)
                pgQueue.push_front({ pos.x, pos.y - 1, pos.z });
            pgQueue.push_front({ pos.x, pos.y, pos.z + 1 });
            pgQueue.push_front({ pos.x, pos.y, pos.z - 1 });
        }

        ComputeModelUpdates(pos.x, pos.y, pos.z, modelUpdates_);
    }
}

bool ChunkManager::PickBlock(const Vector3 &origin, const Vector3 &dir,
                             float maxLen, float step, PickBlockFunc func,
                             Block &blk, BlockFace &face, IntVector3 &rtPos)
{
    float t = 0;
    Vector3 pos = origin, posStep = step * dir;
    while(t < maxLen)
    {
        IntVector3 tBlkPos =
        {
            Camera_To_Block(pos.x),
            Camera_To_Block(pos.y),
            Camera_To_Block(pos.z)
        };
        const Block &tBlk = GetBlock(tBlkPos.x, tBlkPos.y, tBlkPos.z);

        if(func(tBlk))
        {
            blk = tBlk;
            rtPos = tBlkPos;

            //tBlkPos���ĸ���������ͺ��ĸ����ཻ
            float disNegX = pos.x - static_cast<float>(tBlkPos.x);
            float disNegY = pos.y - static_cast<float>(tBlkPos.y);
            float disNegZ = pos.z - static_cast<float>(tBlkPos.z);
            float disPosX = 1.0f - disNegX;
            float disPosY = 1.0f - disNegY;
            float disPosZ = 1.0f - disNegZ;

            face = BlockFace::NegX; float minDis = disPosX;
            UpdateMinDis(minDis, face, disNegX, BlockFace::PosX);
            UpdateMinDis(minDis, face, disPosY, BlockFace::NegY);
            UpdateMinDis(minDis, face, disNegY, BlockFace::PosY);
            UpdateMinDis(minDis, face, disPosZ, BlockFace::NegZ);
            UpdateMinDis(minDis, face, disNegZ, BlockFace::PosZ);

            return true;
        }

        t += step;
        pos += posStep;
    }

    return false;
}

void ChunkManager::SetCentrePosition(int ckX, int ckZ)
{
    if(centrePos_.x == ckX && centrePos_.z == ckZ)
        return;
    centrePos_ = { ckX, ckZ };

    //�ɵ����˷�Χ��Chunk
    decltype(chunks_) newChunks_;
    for(auto it : chunks_)
    {
        if(!InUnloadingRange(it.first.x, it.first.z))
            Helper::SafeDeleteObjects(it.second);
        else
            newChunks_[it.first] = it.second;
    }
    chunks_ = std::move(newChunks_);

    //ȡ�����˷�Χ�ļ�������
    ckLoader_.DelTaskIf([=](ChunkLoaderTask *task) -> bool
    {
        assert(task != nullptr);
        auto [x, z] = task->GetPosition();
        return !InLoadingRange(x, z);
    });

    //�����µ�����
    std::vector<IntVectorXZ> possibleLoadingTasks;
    int loadRangeXEnd = centrePos_.x + loadDistance_;
    int loadRangeZEnd = centrePos_.z + loadDistance_;
    for(int newCkX = centrePos_.x - loadDistance_; newCkX <= loadRangeXEnd; ++newCkX)
    {
        for(int newCkZ = centrePos_.z - loadDistance_; newCkZ <= loadRangeZEnd; ++newCkZ)
        {
            assert(InLoadingRange(newCkX, newCkZ));
            if(chunks_.find({ newCkX, newCkZ }) == chunks_.end()) //��û�������������ݣ�Ҳû������
                ckLoader_.TryAddLoadingTask(this, newCkX, newCkZ);
        }
    }

    ckLoader_.Sort([=](const IntVectorXZ &lhs, const IntVectorXZ &rhs) -> bool
    {
        return (lhs.x - ckX) * (lhs.x - ckX) + (lhs.z - ckZ) * (lhs.z - ckZ) >
               (rhs.x - ckX) * (rhs.x - ckX) + (rhs.z - ckZ) * (rhs.z - ckZ);
    });
}

void ChunkManager::MakeSectionModelInvalid(int x, int y, int z)
{
    auto it = chunks_.find({ x, z });
    if(it == chunks_.end() || !InRenderRange(x, z))
        return;
    modelUpdates_.insert({ x, y, z });
}

void ChunkManager::AddChunkData(Chunk *ck)
{
    assert(ck != nullptr);
    decltype(chunks_)::iterator it;
    IntVectorXZ pos = ck->GetPosition();

    //�Ѿ����ˣ�����ֱ�Ӷ���
    if((it = chunks_.find(pos)) != chunks_.end())
    {
        assert(it->second);
        Helper::SafeDeleteObjects(ck);
        return;
    }

    chunks_[pos] = ck;

    if(InRenderRange(pos.x, pos.z)) //�Ƿ���Ҫ����ģ������
    {
        for(int section = 0; section != CHUNK_SECTION_NUM; ++section)
        {
            if(!ck->GetModels(section))
                modelUpdates_.insert({ pos.x, section, pos.z });
        }
    }
}

void ChunkManager::AddSectionModel(const IntVector3 &pos, ChunkSectionModels *models)
{
    assert(models != nullptr);
    auto it = chunks_.find({ pos.x, pos.z });
    if(it == chunks_.end())
    {
        Helper::SafeDeleteObjects(models);
        return;
    }
    it->second->SetModels(pos.y, models);
}

void ChunkManager::LoadChunk(int ckX, int ckZ)
{
    assert(chunks_.find({ ckX, ckZ }) == chunks_.end());

    Chunk *ck = new Chunk(this, { ckX, ckZ });
    ckLoader_.LoadChunkData(ck);
    assert(ck != nullptr);
    AddChunkData(ck);
}

void ChunkManager::ProcessChunkLoaderMessages(void)
{
    std::queue<ChunkLoaderMessage*> msgs = ckLoader_.FetchAllMsgs();

    while(msgs.size())
    {
        ChunkLoaderMessage *msg = msgs.front();
        msgs.pop();

        switch(msg->type)
        {
        case ChunkLoaderMessage::ChunkLoaded:
            if(InLoadingRange(msg->ckLoaded->GetPosition().x, msg->ckLoaded->GetPosition().z))
                AddChunkData(msg->ckLoaded);
            else
                Helper::SafeDeleteObjects(msg->ckLoaded);
            break;
        default:
            std::abort();
        }

        Helper::SafeDeleteObjects(msg);
    }
}

void ChunkManager::ProcessModelUpdates(void)
{
    while(modelUpdates_.size())
    {
        IntVector3 pos = *modelUpdates_.begin();
        modelUpdates_.erase(pos);

        auto it = chunks_.find({ pos.x, pos.z });
        if(it == chunks_.end())
            continue;
        ChunkModelBuilder builder(this, it->second, pos.y);
        AddSectionModel(pos, builder.Build());
    }
}

void ChunkManager::Render(const Camera &cam, ChunkSectionRenderQueue *renderQueue)
{
    assert(renderQueue != nullptr);

    for(auto it : chunks_)
    {
        if(!InRenderRange(it.first.x, it.first.z))
            continue;
        it.second->Render(cam, renderQueue);
    }
}

void ChunkManager::ComputeModelUpdates(int x, int y, int z,
                                       std::unordered_set<IntVector3, IntVector3Hasher> &updates)
{
    int ckX = BlockXZ_To_ChunkXZ(x);
    int ckZ = BlockXZ_To_ChunkXZ(z);

    int secY = BlockY_To_ChunkSectionIndex(y);

    int blkX = BlockXZ_To_BlockXZInChunk(x);
    int blkY = BlockY_To_BlockYInChunkSection(y);
    int blkZ = BlockXZ_To_BlockXZInChunk(z);

    updates.insert({ ckX, secY, ckZ });

    if(blkX == 0 && InRenderRange(ckX - 1, ckZ))
        updates.insert({ ckX - 1, secY, ckZ });
    else if(blkX == CHUNK_SECTION_SIZE - 1 && InRenderRange(ckX + 1, ckZ))
        updates.insert({ ckX + 1, secY, ckZ });
    if(blkZ == 0 && InRenderRange(ckX, ckZ - 1))
        updates.insert({ ckX, secY, ckZ - 1 });
    else if(blkZ == CHUNK_SECTION_SIZE - 1 && InRenderRange(ckX, ckZ + 1))
        updates.insert({ ckX, secY, ckZ + 1 });
    if(blkY == 0 && secY > 0)
        updates.insert({ ckX, secY - 1, ckZ });
    else if(blkY == CHUNK_SECTION_SIZE - 1 && secY < CHUNK_SECTION_NUM - 1)
        updates.insert({ ckX, secY + 1, ckZ });
    if(blkX == 0 && blkZ == 0 && InRenderRange(ckX - 1, ckZ - 1))
    {
        if(blkY == 0 && secY > 0)
            updates.insert({ ckX - 1, secY - 1, ckZ - 1 });
        else if(blkY == CHUNK_SECTION_SIZE - 1 && secY < CHUNK_SECTION_NUM - 1)
            updates.insert({ ckX - 1, secY + 1, ckZ - 1 });
        updates.insert({ ckX - 1, secY, ckZ - 1 });
    }
    if(blkX == 0 && blkZ == CHUNK_SECTION_SIZE - 1 && InRenderRange(ckX - 1, ckZ + 1))
    {
        if(blkY == 0 && secY > 0)
            updates.insert({ ckX - 1, secY - 1, ckZ + 1 });
        else if(blkY == CHUNK_SECTION_SIZE - 1 && secY < CHUNK_SECTION_NUM - 1)
            updates.insert({ ckX - 1, secY + 1, ckZ + 1 });
        updates.insert({ ckX - 1, secY, ckZ + 1 });
    }
    if(blkX == CHUNK_SECTION_SIZE - 1 && blkZ == 0 && InRenderRange(ckX + 1, ckZ - 1))
    {
        if(blkY == 0 && secY > 0)
            updates.insert({ ckX + 1, secY - 1, ckZ - 1 });
        else if(blkY == CHUNK_SECTION_SIZE - 1 && secY < CHUNK_SECTION_NUM - 1)
            updates.insert({ ckX + 1, secY + 1, ckZ - 1 });
        updates.insert({ ckX + 1, secY, ckZ - 1 });
    }
    if(blkX == CHUNK_SECTION_SIZE - 1 && blkZ == CHUNK_SECTION_SIZE - 1 && InRenderRange(ckX + 1, ckZ + 1))
    {
        if(blkY == 0 && secY > 0)
            updates.insert({ ckX + 1, secY - 1, ckZ + 1 });
        else if(blkY == CHUNK_SECTION_SIZE - 1 && secY < CHUNK_SECTION_NUM - 1)
            updates.insert({ ckX + 1, secY + 1, ckZ + 1 });
        updates.insert({ ckX + 1, secY, ckZ + 1 });
    }
}

bool ChunkManager::DetectCollision(const Vector3 &pnt)
{
    const BlockInfoManager &infoMgr = BlockInfoManager::GetInstance();

    IntVector3 blkPos = Camera_To_Block(pnt);
    for(int dx = -1; dx <= 1; ++dx)
    {
        for(int dy = -1; dy <= 1; ++dy)
        {
            for(int dz = -1; dz <= 1; ++dz)
            {
                IntVector3 p = { blkPos.x + dx, blkPos.y + dy, blkPos.z + dz };
                Vector3 vp = { static_cast<float>(p.x),
                               static_cast<float>(p.y),
                               static_cast<float>(p.z) };
                if((infoMgr.GetAABB(GetBlockType(p.x, p.y, p.z)) + vp).IsPointIn(pnt))
                    return true;
            }
        }
    }

    return false;
}

bool ChunkManager::DetectCollision(const AABB &aabb)
{
    if(!aabb.IsValid())
        return false;
    const BlockInfoManager &infoMgr = BlockInfoManager::GetInstance();
    IntVector3 blkL = Camera_To_Block(aabb.L);
    IntVector3 blkH = Camera_To_Block(aabb.H);

    for(int x = blkL.x; x <= blkH.x; ++x)
    {
        for(int y = blkL.y; y <= blkH.y; ++y)
        {
            for(int z = blkL.z; z <= blkH.z; ++z)
            {
                Vector3 vp = { static_cast<float>(x),
                               static_cast<float>(y),
                               static_cast<float>(z) };
                if((infoMgr.GetAABB(GetBlockType(x, y, z)) + vp).IsAABBIntersected(aabb))
                    return true;
            }
        }
    }

    return false;
}
