/*================================================================
Filename: OWEShaderReflection.h
Date: 2017.11.25
Created by AirGuanZ
================================================================*/
#ifndef OWESHADER_REFLECTION_H
#define OWESHADER_REFLECTION_H

#include <cassert>
#include <cstring>
#include <string>

#include <d3d11.h>
#include <d3dcompiler.h>

#include "OWEShaderConstantBuffer.h"

namespace OWEShaderAux
{
    inline ID3D11ShaderReflection *GetShaderReflection(void *shaderData, SIZE_T length)
    {
        assert(shaderData != nullptr && length > 0);
        ID3D11ShaderReflection *rt = nullptr;
        if(FAILED(D3DReflect(shaderData, length, IID_ID3D11ShaderReflection, reinterpret_cast<void**>(&rt))))
            return nullptr;
        return rt;
    }

    struct CBInfo
    {
        UINT slot;
        UINT byteSize;
    };

    inline void GetConstantBuffers(ID3D11ShaderReflection *ref, std::map<std::string, CBInfo> *rt)
    {
        assert(ref && rt);
        rt->clear();
        
        D3D11_SHADER_DESC shaderDesc;
        ref->GetDesc(&shaderDesc);
        for(int cbIdx = 0; cbIdx != shaderDesc.ConstantBuffers; ++cbIdx)
        {
            int regIdx = -1; //°ó¶¨µÄ¼Ä´æÆ÷±àºÅ
            ID3D11ShaderReflectionConstantBuffer *cbuf = ref->GetConstantBufferByIndex(cbIdx);
            D3D11_SHADER_BUFFER_DESC bufDesc;
            cbuf->GetDesc(&bufDesc);
            for(int bpIdx = 0; bpIdx != shaderDesc.BoundResources; ++bpIdx)
            {
                D3D11_SHADER_INPUT_BIND_DESC bpDesc;
                ref->GetResourceBindingDesc(bpIdx, &bpDesc);
                if(std::strcmp(bpDesc.Name, bufDesc.Name) == 0 && bpDesc.Type == D3D_SIT_CBUFFER)
                {
                    regIdx = bpDesc.BindPoint;
                    break;
                }
            }

            assert(regIdx != -1);
            rt->insert(std::make_pair(std::string(bufDesc.Name), CBInfo{ static_cast<UINT>(regIdx), bufDesc.Size }));
        }
    }

    struct SRInfo
    {
        UINT slot;
        UINT cnt;
    };

    inline void GetShaderResources(ID3D11ShaderReflection *ref, std::map<std::string, SRInfo> *rt)
    {
        assert(ref && rt);
        rt->clear();

        D3D11_SHADER_DESC shaderDesc;
        ref->GetDesc(&shaderDesc);
        for(int rscIdx = 0; rscIdx != shaderDesc.BoundResources; ++rscIdx)
        {
            D3D11_SHADER_INPUT_BIND_DESC bdDesc;
            ref->GetResourceBindingDesc(rscIdx, &bdDesc);
            if(bdDesc.Type == D3D_SIT_TEXTURE ||
               bdDesc.Type == D3D_SIT_STRUCTURED ||
               bdDesc.Type == D3D_SIT_BYTEADDRESS)
                rt->insert(std::make_pair(std::string(bdDesc.Name), SRInfo{ bdDesc.BindPoint, bdDesc.BindCount }));
        }
    }

    inline void GetShaderSamplers(ID3D11ShaderReflection *ref, std::map<std::string, UINT> *rt)
    {
        assert(ref && rt);
        rt->clear();

        D3D11_SHADER_DESC shaderDesc;
        ref->GetDesc(&shaderDesc);
        for(int rscIdx = 0; rscIdx != shaderDesc.BoundResources; ++rscIdx)
        {
            D3D11_SHADER_INPUT_BIND_DESC bdDesc;
            ref->GetResourceBindingDesc(rscIdx, &bdDesc);
            if(bdDesc.Type == D3D_SIT_SAMPLER)
                rt->insert(std::make_pair(std::string(bdDesc.Name), bdDesc.BindPoint));
        }
    }
}

#endif //OWESHADER_REFLECTION_H
