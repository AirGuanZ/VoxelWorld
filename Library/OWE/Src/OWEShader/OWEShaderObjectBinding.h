/*================================================================
Filename: OWEShaderObjectBinding.h
Date: 2017.11.23
Created by AirGuanZ
================================================================*/
#ifndef OWESHADER_OBJECTBindING_H
#define OWESHADER_OBJECTBindING_H

#include <cassert>
#include <d3d11.h>

namespace OWEShaderAux
{
    using ShaderStageSelector = int;

    constexpr ShaderStageSelector SS_US = 0;
    constexpr ShaderStageSelector SS_VS  = 1;
    constexpr ShaderStageSelector SS_GS  = 2;
    constexpr ShaderStageSelector SS_PS  = 3;

    template<ShaderStageSelector StageSelector>
    inline void BindConstantBuffer(ID3D11DeviceContext *devCon, UINT slot, ID3D11Buffer *buf);

    template<>
    inline void BindConstantBuffer<SS_VS>(ID3D11DeviceContext *devCon, UINT slot, ID3D11Buffer *buf)
    {
        assert(devCon != nullptr);
        devCon->VSSetConstantBuffers(slot, 1, &buf);
    }

    template<>
    inline void BindConstantBuffer<SS_GS>(ID3D11DeviceContext *DC, UINT slot, ID3D11Buffer *buf)
    {
        assert(DC != nullptr);
        DC->GSSetConstantBuffers(slot, 1, &buf);
    }

    template<>
    inline void BindConstantBuffer<SS_PS>(ID3D11DeviceContext *devCon, UINT slot, ID3D11Buffer *buf)
    {
        assert(devCon != nullptr);
        devCon->PSSetConstantBuffers(slot, 1, &buf);
    }

    template<ShaderStageSelector StageSelector>
    inline void BindShaderResource(ID3D11DeviceContext *devCon, UINT slot, ID3D11ShaderResourceView *rsc);

    template<>
    inline void BindShaderResource<SS_VS>(ID3D11DeviceContext *devCon, UINT slot, ID3D11ShaderResourceView *rsc)
    {
        assert(devCon != nullptr);
        devCon->VSSetShaderResources(slot, 1, &rsc);
    }

    template<>
    inline void BindShaderResource<SS_GS>(ID3D11DeviceContext *DC, UINT slot, ID3D11ShaderResourceView *rsc)
    {
        assert(DC != nullptr);
        DC->GSSetShaderResources(slot, 1, &rsc);
    }

    template<>
    inline void BindShaderResource<SS_PS>(ID3D11DeviceContext *devCon, UINT slot, ID3D11ShaderResourceView *rsc)
    {
        assert(devCon != nullptr);
        devCon->PSSetShaderResources(slot, 1, &rsc);
    }

    template<ShaderStageSelector StageSelector>
    inline void BindShaderResourceArray(ID3D11DeviceContext *DC, UINT slot, UINT cnt, ID3D11ShaderResourceView **rsc);

    template<>
    inline void BindShaderResourceArray<SS_VS>(ID3D11DeviceContext *DC, UINT slot, UINT cnt, ID3D11ShaderResourceView **rsc)
    {
        assert(DC != nullptr);
        DC->VSSetShaderResources(slot, cnt, rsc);
    }

    template<>
    inline void BindShaderResourceArray<SS_GS>(ID3D11DeviceContext *DC, UINT slot, UINT cnt, ID3D11ShaderResourceView **rsc)
    {
        assert(DC != nullptr);
        DC->GSSetShaderResources(slot, cnt, rsc);
    }

    template<>
    inline void BindShaderResourceArray<SS_PS>(ID3D11DeviceContext *DC, UINT slot, UINT cnt, ID3D11ShaderResourceView **rsc)
    {
        assert(DC != nullptr);
        DC->PSSetShaderResources(slot, cnt, rsc);
    }

    template<ShaderStageSelector StageSelector>
    inline void BindShaderSampler(ID3D11DeviceContext *devCon, UINT slot, ID3D11SamplerState *sampler);

    template<>
    inline void BindShaderSampler<SS_VS>(ID3D11DeviceContext *devCon, UINT slot, ID3D11SamplerState *sampler)
    {
        assert(devCon != nullptr);
        devCon->VSSetSamplers(slot, 1, &sampler);
    }

    template<>
    inline void BindShaderSampler<SS_GS>(ID3D11DeviceContext *DC, UINT slot, ID3D11SamplerState *sampler)
    {
        assert(DC != nullptr);
        DC->GSSetSamplers(slot, 1, &sampler);
    }

    template<>
    inline void BindShaderSampler<SS_PS>(ID3D11DeviceContext *devCon, UINT slot, ID3D11SamplerState *sampler)
    {
        assert(devCon != nullptr);
        devCon->PSSetSamplers(slot, 1, &sampler);
    }
}

#endif //OWESHADER_OBJECTBindING_H
