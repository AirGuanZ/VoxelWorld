/*================================================================
Filename: VMEComponentMeshBuilder.cpp
Date: 2018.3.24
Created by AirGuanZ
================================================================*/
#include <cassert>
#include <limits>
#include <vector>

#include <Utility\HelperFunctions.h>

#include "VMEComponentMeshBuilder.h"

namespace
{
    const VMEComponentVertex faceDataPosX[4] =
    {
        { { 1.0f, 0.0f, 1.0f }, { 1.0f, 0.0f, 0.0f }, {  } },
        { { 1.0f, 1.0f, 1.0f }, { 1.0f, 0.0f, 0.0f }, {  } },
        { { 1.0f, 1.0f, 0.0f }, { 1.0f, 0.0f, 0.0f }, {  } },
        { { 1.0f, 0.0f, 0.0f }, { 1.0f, 0.0f, 0.0f }, {  } }
    };
    const VMEComponentVertex faceDataNegX[4] =
    {
        { { 0.0f, 0.0f, 0.0f }, { -1.0f, 0.0f, 0.0f }, {  } },
        { { 0.0f, 1.0f, 0.0f }, { -1.0f, 0.0f, 0.0f }, {  } },
        { { 0.0f, 1.0f, 1.0f }, { -1.0f, 0.0f, 0.0f }, {  } },
        { { 0.0f, 0.0f, 1.0f }, { -1.0f, 0.0f, 0.0f }, {  } }
    };
    const VMEComponentVertex faceDataPosY[4] =
    {
        { { 0.0f, 1.0f, 1.0f }, { 0.0f, 1.0f, 0.0f }, {  } },
        { { 0.0f, 1.0f, 0.0f }, { 0.0f, 1.0f, 0.0f }, {  } },
        { { 1.0f, 1.0f, 0.0f }, { 0.0f, 1.0f, 0.0f }, {  } },
        { { 1.0f, 1.0f, 1.0f }, { 0.0f, 1.0f, 0.0f }, {  } }
    };
    const VMEComponentVertex faceDataNegY[4] =
    {
        { { 1.0f, 0.0f, 1.0f }, { 0.0f, -1.0f, 0.0f }, {  } },
        { { 1.0f, 0.0f, 0.0f }, { 0.0f, -1.0f, 0.0f }, {  } },
        { { 0.0f, 0.0f, 0.0f }, { 0.0f, -1.0f, 0.0f }, {  } },
        { { 0.0f, 0.0f, 1.0f }, { 0.0f, -1.0f, 0.0f }, {  } }
    };
    const VMEComponentVertex faceDataPosZ[4] =
    {
        { { 0.0f, 0.0f, 1.0f }, { 0.0f, 0.0f, 1.0f }, {  } },
        { { 0.0f, 1.0f, 1.0f }, { 0.0f, 0.0f, 1.0f }, {  } },
        { { 1.0f, 1.0f, 1.0f }, { 0.0f, 0.0f, 1.0f }, {  } },
        { { 1.0f, 0.0f, 1.0f }, { 0.0f, 0.0f, 1.0f }, {  } }
    };
    const VMEComponentVertex faceDataNegZ[4] =
    {
        { { 1.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, -1.0f }, {  } },
        { { 1.0f, 1.0f, 0.0f }, { 0.0f, 0.0f, -1.0f }, {  } },
        { { 0.0f, 1.0f, 0.0f }, { 0.0f, 0.0f, -1.0f }, {  } },
        { { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, -1.0f }, {  } }
    };

    bool IsFaceVisible(const VMEBindingContent::Component &cpt,
                       int vxlX, int vxlY, int vxlZ,
                       int neiX, int neiY, int neiZ)
    {
        assert(cpt.boundNegX <= vxlX && vxlX <= cpt.boundPosX);
        assert(cpt.boundNegY <= vxlY && vxlY <= cpt.boundPosY);
        assert(cpt.boundNegZ <= vxlZ && vxlZ <= cpt.boundPosZ);

        if(neiX < cpt.boundNegX || cpt.boundPosX < neiX ||
           neiY < cpt.boundNegY || cpt.boundPosY < neiY ||
           neiZ < cpt.boundNegZ || cpt.boundPosZ < neiZ)
            return true;

        auto it = cpt.voxels.find({ vxlX, vxlY, vxlZ });
        if(it == cpt.voxels.end())
            return false;
        return cpt.voxels.find({ neiX, neiY, neiZ }) == cpt.voxels.end();
    }

    void AddFace(const VMEComponentVertex (&templateData)[4],
                 const IntVector3 &offset,
                 const IntVector3 &color,
                 float voxelSize,
                 std::vector<VMEComponentVertex> &vtxData,
                 std::vector<UINT16> &idxData)
    {
        auto MakeVertex = [&](const VMEComponentVertex &vtx) -> VMEComponentVertex
        {
            VMEComponentVertex rt;
            rt.pos = voxelSize * vtx.pos + Vector3(voxelSize * offset.x,
                                                   voxelSize * offset.y,
                                                   voxelSize * offset.z);

            constexpr float COLOR_DEM = static_cast<float>((std::numeric_limits<decltype(color.x)>::max)());
            rt.color = Vector3(color.x / COLOR_DEM, color.y / COLOR_DEM, color.z / COLOR_DEM);

            rt.nor = vtx.nor;

            return rt;
        };

        for(auto &v : templateData)
            vtxData.push_back(MakeVertex(v));

        UINT16 idx0 = static_cast<UINT16>(idxData.size());
        idxData.push_back(idx0);
        idxData.push_back(idx0 + 1);
        idxData.push_back(idx0 + 2);
        idxData.push_back(idx0);
        idxData.push_back(idx0 + 2);
        idxData.push_back(idx0 + 3);
    }
}

bool VMEComponentMeshBuilder::BuildMeshFromComponent(const VMEBindingContent::Component &cpt,
                                                     VMEComponentMesh &mesh) const
{
    mesh.Destroy();
    std::vector<VMEComponentVertex> vtxData;
    std::vector<UINT16> idxData;

    for(auto &vxl : cpt.voxels)
    {
        //所有的面都建出来好了(╯‵□′)╯︵┻━┻
        //模型优化在导出为可用model的时候再做

        const IntVector3 offset = { vxl.first.x, vxl.first.y, vxl.first.z };

        AddFace(faceDataPosX, offset, vxl.second, cpt.voxelSize, vtxData, idxData);
        AddFace(faceDataNegX, offset, vxl.second, cpt.voxelSize, vtxData, idxData);
        AddFace(faceDataPosY, offset, vxl.second, cpt.voxelSize, vtxData, idxData);
        AddFace(faceDataNegY, offset, vxl.second, cpt.voxelSize, vtxData, idxData);
        AddFace(faceDataPosZ, offset, vxl.second, cpt.voxelSize, vtxData, idxData);
        AddFace(faceDataNegZ, offset, vxl.second, cpt.voxelSize, vtxData, idxData);
    }

    return mesh.Initialize(vtxData, idxData);
}
