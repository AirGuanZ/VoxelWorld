/*================================================================
Filename: OWEShaderUniforms.h
Date: 2017.12.18
Created by AirGuanZ
================================================================*/
#ifndef OWESHADER_UNIFORMS_H
#define OWESHADER_UNIFORMS_H

#include <cassert>

#include "OWEShaderStage.h"

namespace OWEShaderAux
{
    struct ShaderStageUniformsBinder
    {
        template<typename T>
        void operator()(T &ref)
        {
            ref.Bind(DC);
        }
        
        ID3D11DeviceContext *DC;
    };
    
    struct ShaderStageUniformsUnbinder
    {
        template<typename T>
        void operator()(T &ref)
        {
            ref.Unbind(DC);
        }
        
        ID3D11DeviceContext *DC;
    };
    
    template<ShaderStageSelector StageSelector>
    class ShaderStageUniforms
    {
    public:
        ShaderStageUniforms(const ShaderStage<StageSelector> &stage)
        {
            CBMgr_ = stage.GetConstantBufferCount() ? stage.CreateConstantBufferManager() : nullptr;
            SRMgr_ = stage.GetShaderResourceCount() ? stage.CreateShaderResourceManager() : nullptr;
            SSMgr_ = stage.GetShaderSamplerCount()  ? stage.CreateShaderSamplerManager()  : nullptr;
        }

        ~ShaderStageUniforms(void)
        {
            SafeDeleteObjects(CBMgr_, SRMgr_, SSMgr_);
        }

        ConstantBufferManager<StageSelector> *GetConstantBufferManager(void)
        {
            return CBMgr_;
        }

        ShaderResourceManager<StageSelector> *GetShaderResourceManager(void)
        {
            return SRMgr_;
        }

        ShaderSamplerManager<StageSelector> *GetShaderSamplerManager(void)
        {
            return SSMgr_;
        }
        
        void Bind(ID3D11DeviceContext *DC)
        {
            assert(DC != nullptr);
            if(CBMgr_)
                CBMgr_->Bind(DC);
            if(SRMgr_)
                SRMgr_->Bind(DC);
            if(SSMgr_)
                SSMgr_->Bind(DC);
        }
        
        void Unbind(ID3D11DeviceContext *DC)
        {
            assert(DC != nullptr);
            if(CBMgr_)
                CBMgr_->Unbind(DC);
            if(SRMgr_)
                SRMgr_->Unbind(DC);
            if(SSMgr_)
                SSMgr_->Unbind(DC);
        }

        template<typename BufferType, bool Dynamic>
        ConstantBufferObject<BufferType, StageSelector, Dynamic>*
        GetConstantBuffer(ID3D11Device *dev, const std::string &name, const BufferType *data)
        {
            assert(CBMgr_ != nullptr);
            return CBMgr_->GetConstantBuffer<BufferType, Dynamic>(dev, name, data);
        }

        ShaderResourceObject<StageSelector> *GetShaderResource(const std::string &name)
        {
            assert(SRMgr_ != nullptr);
            return SRMgr_->GetShaderResourceObject(name);
        }

        ShaderSamplerObject<StageSelector> *GetShaderSampler(const std::string &name)
        {
            assert(SSMgr_ != nullptr);
            return SSMgr_->GetShaderSamplerObject(name);
        }

    private:
        ConstantBufferManager<StageSelector> *CBMgr_;
        ShaderResourceManager<StageSelector> *SRMgr_;
        ShaderSamplerManager<StageSelector> *SSMgr_;
    };

    template<ShaderStageSelector...StageSelectors>
    class ShaderUniformManager
    {
    public:
        ShaderUniformManager(const ShaderStage<StageSelectors>&...stages)
            : stageUniforms_(stages...)
        {

        }

        ~ShaderUniformManager(void)
        {

        }

        template<ShaderStageSelector StageSelector>
        ShaderStageUniforms<StageSelector> *GetStageUniforms(void)
        {
            return &std::get<FindInNumList<ShaderStageSelector, StageSelector, StageSelectors...>()>(stageUniforms_);
        }

        template<ShaderStageSelector StageSelector>
        ConstantBufferManager<StageSelector> *GetConstantBufferManager(void)
        {
            return GetStageUniforms<StageSelector>()->GetConstantBufferManager();
        }

        template<ShaderStageSelector StageSelector>
        ShaderResourceManager<StageSelector> *GetShaderResourceManager(void)
        {
            return GetStageUniforms<StageSelector>()->GetShaderResourceManager();
        }

        template<ShaderStageSelector StageSelector>
        ShaderSamplerManager<StageSelector> *GetShaderSamplerManager(void)
        {
            return GetStageUniforms<StageSelector>()->GetShaderSamplerManager();
        }

        template<ShaderStageSelector StageSelector, typename BufferType, bool Dynamic = true>
        ConstantBufferObject<BufferType, StageSelector, Dynamic>*
        GetConstantBuffer(ID3D11Device *dev, const std::string &name, const BufferType *data = nullptr)
        {
            return GetStageUniforms<StageSelector>()->GetConstantBuffer<BufferType, Dynamic>(dev, name, data);
        }

        template<ShaderStageSelector StageSelector>
        ShaderResourceObject<StageSelector> *GetShaderResource(const std::string &name)
        {
            return GetStageUniforms<StageSelector>()->GetShaderResource(name);
        }

        template<ShaderStageSelector StageSelector>
        ShaderSamplerObject<StageSelector> *GetShaderSampler(const std::string &name)
        {
            return GetStageUniforms<StageSelector>()->GetShaderSampler(name);
        }
        
        void Bind(ID3D11DeviceContext *DC)
        {
            DoForTupleElements(ShaderStageUniformsBinder{ DC }, stageUniforms_);
        }
        
        void Unbind(ID3D11DeviceContext *DC)
        {
            DoForTupleElements(ShaderStageUniformsUnbinder{ DC }, stageUniforms_);
        }

    private:
        std::tuple<ShaderStageUniforms<StageSelectors>...> stageUniforms_;
    };
}

namespace OWE
{
    template<OWEShaderAux::ShaderStageSelector...StageSelectors>
    using ShaderUniforms = OWEShaderAux::ShaderUniformManager<StageSelectors...>;
}

#endif //OWESHADER_UNIFORMS_H
