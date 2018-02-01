/*================================================================
Filename: ChunkManager.cpp
Date: 2018.1.18
Created by AirGuanZ
================================================================*/
#include <cstdlib>
#include <limits>
#include <mutex>

#include "../Block/BlockInfoManager.h"
#include "../Utility/HelperFunctions.h"
#include "ChunkLoader.h"
#include "ChunkManager.h"
#include "ChunkModelBuilder.h"

ChunkManager::ChunkManager(int loadDistance,
                           int renderDistance,
                           int unloadDistance,
                           int maxImpModelUpdates,
                           int maxUniModelUpdates,
                           int maxModelUpdates,
                           int maxUniLightUpdates,
                           int uniLightUpdateDistance)
    : loadDistance_(loadDistance),
      renderDistance_(renderDistance),
      unloadDistance_(unloadDistance),
      maxImpModelUpdates_(maxImpModelUpdates),
      maxUniModelUpdates_(maxUniModelUpdates),
      maxModelUpdates_(maxModelUpdates),
      maxUniLightUpdates_(maxUniLightUpdates),
      uniLightUpdateDistance_(uniLightUpdateDistance),
      ckLoader_((loadDistance + 2) * (loadDistance + 2))
{
    centrePos_.x = (std::numeric_limits<decltype(centrePos_.x)>::min)();
    centrePos_.z = (std::numeric_limits<decltype(centrePos_.z)>::min)();
}

ChunkManager::~ChunkManager(void)
{
    Destroy();
}

void ChunkManager::StartLoading(void)
{
    ckLoader_.Initialize();
}

void ChunkManager::Destroy(void)
{
    ckLoader_.Destroy();
    ProcessChunkLoaderMessages();

    for(auto it : chunks_)
        Helper::SafeDeleteObjects(it.second);
    chunks_.clear();
    importantModelUpdates_.clear();
    unimportantModelUpdates_.clear();
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

void ChunkManager::RemoveLightSource(int x, int y, int z)
{
    struct RMNode
    {
        IntVector3 pos;
        BlockLight light;
    };
    std::deque<RMNode> rmQueue;
    std::deque<IntVector3> pgQueue;

    //TODO
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

        ComputeModelUpdates(pos.x, pos.y, pos.z, importantModelUpdates_);
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

            //tBlkPos离哪个面最近，就和哪个面相交
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

    //干掉出了范围的Chunk
    decltype(chunks_) newChunks_;
    for(auto it : chunks_)
    {
        if(!InUnloadingRange(it.first.x, it.first.z))
            Helper::SafeDeleteObjects(it.second);
        else
            newChunks_[it.first] = it.second;
    }
    chunks_ = std::move(newChunks_);

    //遍历新范围内的Chunk
    //缺数据的建立加载任务
    //不缺数据的看看需不需要建立模型任务

    ckLoader_.DelTaskIf([=](ChunkLoaderTask *task) -> bool
    {
        assert(task != nullptr);

        ChunkLoaderTask_LoadChunkData *t = dynamic_cast<ChunkLoaderTask_LoadChunkData*>(task);
        auto [x, z] = t->GetPosition();
        return !InLoadingRange(x, z);
    });

    int loadRangeXEnd = centrePos_.x + loadDistance_;
    int loadRangeZEnd = centrePos_.z + loadDistance_;
    for(int newCkX = centrePos_.x - loadDistance_; newCkX <= loadRangeXEnd; ++newCkX)
    {
        for(int newCkZ = centrePos_.z - loadDistance_; newCkZ <= loadRangeZEnd; ++newCkZ)
        {
            assert(InLoadingRange(newCkX, newCkZ));
            auto it = chunks_.find({ newCkX, newCkZ });
            if(it == chunks_.end()) //还没有这个区块的数据
            {
                ckLoader_.AddTask(
                    new ChunkLoaderTask_LoadChunkData(
                        new Chunk(this, { newCkX, newCkZ })));
            }
            else if(InRenderRange(newCkX, newCkZ)) //有数据了，看看在不在渲染范围内
            {
                for(int section = 0; section != CHUNK_SECTION_NUM; ++section)
                {
                    if(!it->second->GetModels(section))
                        unimportantModelUpdates_.insert({ newCkX, section, newCkZ });
                }
            }
        }
    }
}

void ChunkManager::MakeSectionModelInvalid(int x, int y, int z)
{
    auto it = chunks_.find({ x, z });
    if(it == chunks_.end() || !InRenderRange(x, z))
        return;
    importantModelUpdates_.insert({ x, y, z });
}

void ChunkManager::AddChunkData(Chunk *ck)
{
    assert(ck != nullptr);
    decltype(chunks_)::iterator it;
    IntVectorXZ pos = ck->GetPosition();

    //已经有了，所以直接丢掉
    if((it = chunks_.find(pos)) != chunks_.end())
    {
        assert(it->second);
        Helper::SafeDeleteObjects(ck);
        return;
    }

    chunks_[pos] = ck;

    if(InRenderRange(pos.x, pos.z)) //是否需要创建模型任务
    {
        for(int section = 0; section != CHUNK_SECTION_NUM; ++section)
            unimportantModelUpdates_.insert({ pos.x, section, pos.z });
    }

    for(int dx = -1; dx <= 1; ++dx)
    {
        for(int dz = -1; dz <= 1; ++dz)
        {
            for(int sec = 0; sec != CHUNK_SECTION_NUM; ++sec)
            {
                IntVector3 p = { pos.x + dx, sec, pos.z + dz };
                auto it = uniModelWaiting_.find(p);
                if(it != uniModelWaiting_.end())
                {
                    uniModelWaiting_.erase(it);
                    unimportantModelUpdates_.insert(p);
                }
            }
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

void ChunkManager::ProcessLightUpdates(void)
{
    int uniUpdatesCount = 0;
    size_t OORCount = 0;
    while(unimportantLightUpdates_.size() &&
          uniUpdatesCount < maxUniLightUpdates_)
    {
        ++uniUpdatesCount;

        IntVector3 pos = unimportantLightUpdates_.front();
        unimportantLightUpdates_.pop_front();

        int ckX = BlockXZ_To_ChunkXZ(pos.x);
        int ckZ = BlockXZ_To_ChunkXZ(pos.z);

        if(ckX < centrePos_.x - uniLightUpdateDistance_ || ckX > centrePos_.x + uniLightUpdateDistance_ ||
           ckZ < centrePos_.z - uniLightUpdateDistance_ || ckZ > centrePos_.z + uniLightUpdateDistance_)
        {
            if(!InLoadingRange(ckX, ckZ))
                continue;

            unimportantLightUpdates_.push_back(pos);
            if(++OORCount >= unimportantLightUpdates_.size())
                break;
            continue;
        }

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
            pos.y > ck->heightMap[Chunk::XZ(blkX, blkZ)] ? LIGHT_COMPONENT_MAX : LIGHT_COMPONENT_MIN);

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
            unimportantLightUpdates_.push_front({ pos.x + 1, pos.y, pos.z });
            unimportantLightUpdates_.push_front({ pos.x - 1, pos.y, pos.z });
            if(pos.y < CHUNK_MAX_HEIGHT - 1)
                unimportantLightUpdates_.push_front({ pos.x, pos.y + 1, pos.z });
            if(pos.y > 0)
                unimportantLightUpdates_.push_front({ pos.x, pos.y - 1, pos.z });
            unimportantLightUpdates_.push_front({ pos.x, pos.y, pos.z + 1 });
            unimportantLightUpdates_.push_front({ pos.x, pos.y, pos.z - 1 });
        }

        int blkY = pos.y;
        int secY = BlockY_To_ChunkSectionIndex(pos.y);

        ComputeModelUpdates(pos.x, pos.y, pos.z, unimportantModelUpdates_);
    }
}

void ChunkManager::ProcessModelUpdates(void)
{
    //优先处理重要更新
    int updatesCount = 0;
    while(!importantModelUpdates_.empty() && updatesCount < maxImpModelUpdates_)
    {
        IntVector3 pos = *importantModelUpdates_.begin();
        importantModelUpdates_.erase(pos);

        auto it = chunks_.find({ pos.x, pos.z });
        if(it == chunks_.end())
            continue;
        ChunkModelBuilder builder(this, it->second, pos.y);
        AddSectionModel(pos, builder.Build());
        ++updatesCount;
    }

    //然后才处理不重要更新
    int uniUpdatesCount = 0;
    while(!unimportantModelUpdates_.empty() &&
          uniUpdatesCount < maxUniModelUpdates_ &&
          updatesCount < maxModelUpdates_)
    {
        IntVector3 pos = *unimportantModelUpdates_.begin();
        unimportantModelUpdates_.erase(pos);

        auto it = chunks_.find({ pos.x, pos.z });
        if(it == chunks_.end())
            continue;

        bool con = false;
        for(int dx = -1; dx <= 1; ++dx)
        {
            for(int dz = -1; dz <= 1; ++dz)
            {
                if(chunks_.find({ pos.x + dx, pos.z + dz }) == chunks_.end())
                {
                    uniModelWaiting_.insert(pos);
                    con = true;
                    goto DEC_CON;
                }
            }
        }
    DEC_CON:
        if(con)
            continue;

        ChunkModelBuilder builder(this, it->second, pos.y);
        AddSectionModel(pos, builder.Build());
        ++updatesCount;
        ++uniUpdatesCount;
    }
}

void ChunkManager::Render(ChunkSectionRenderQueue *renderQueue)
{
    assert(renderQueue != nullptr);

    for(auto it : chunks_)
    {
        if(!InRenderRange(it.first.x, it.first.z))
            continue;
        it.second->Render(renderQueue);
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
