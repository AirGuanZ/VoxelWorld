/*================================================================
Filename: OWEShaderSampler.h
Date: 2017.11.25
Created by AirGuanZ
================================================================*/
#ifndef OWESHADER_SAMPLER_H
#define OWESHADER_SAMPLER_H

#include <map>

#include "OWEShaderFatalError.h"
#include "OWEShaderObjectBinding.h"
#include "OWEShaderReleaseCOMObjects.h"
#include "OWEShaderUncopiable.h"

namespace OWEShaderAux
{
    template<ShaderStageSelector StageSelector>
    class ShaderSamplerObject;

    template<ShaderStageSelector StageSelector>
    class ShaderStage;

    template<ShaderStageSelector StageSelector>
    class ShaderSamplerManager;

    template<ShaderStageSelector StageSelector>
    class ShaderSamplerObject : public Uncopiable
    {
    public:
        void SetSampler(ID3D11SamplerState *sampler)
        {
            ReleaseCOMObjects(sampler_);
            sampler_ = sampler;
            if(sampler_)
                sampler_->AddRef();
        }

        void Bind(ID3D11DeviceContext *DC)
        {
            assert(DC != nullptr);
            BindShaderSampler<StageSelector>(DC, slot_, sampler_);
        }

        void Unbind(ID3D11DeviceContext *DC)
        {
            assert(DC != nullptr);
            BindShaderSampler<StageSelector>(DC, slot_, nullptr);
        }

    private:
        friend class ShaderSamplerManager<StageSelector>;

        ShaderSamplerObject(UINT slot, ID3D11SamplerState *sampler = nullptr)
            : slot_(slot), sampler_(sampler)
        {
            if(sampler)
                sampler->AddRef();
        }

        ~ShaderSamplerObject(void)
        {
            ReleaseCOMObjects(sampler_);
        }

    private:
        UINT slot_;
        ID3D11SamplerState *sampler_;
    };

    template<ShaderStageSelector StageSelector>
    class ShaderSamplerManager : public Uncopiable
    {
    public:
        using SSObj = ShaderSamplerObject<StageSelector>;

        ~ShaderSamplerManager(void)
        {
            for(auto it : SSs_)
            {
                if(it.second.obj)
                    delete it.second.obj;
            }
        }

        bool AddShaderSampler(const std::string &name, UINT slot, ID3D11SamplerState *initSampler = nullptr)
        {
            auto it = SSs_.find(name);
            if(it != SSs_.end())
                return false;
            SSs_[name] = _SSRec{ slot, new SSobj(slot, initSampler) };
            return true;
        }

        SSObj *GetShaderSamplerObject(const std::string &name)
        {
            auto it = SSs_.find(name);
            if(it == SSs_.end())
            {
#ifdef OWE_NO_EXCEPTION
                return nullptr;
#else
                throw OWEShaderError(("Shader sampler not found: " + name).c_str());
#endif
            }

            assert(it->second.obj != nullptr);
            return it->second.obj;
        }

        void Bind(ID3D11DeviceContext *DC)
        {
            assert(DC != nullptr);
            for(auto it : SSs_)
            {
                assert(it.second.obj);
                it.second.obj->Bind(DC);
            }
        }

        void Unbind(ID3D11DeviceContext *DC)
        {
            assert(DC != nullptr);
            for(auto it : SSs_)
            {
                assert(it.second.obj);
                it.second.obj->Unbind(DC);
            }
        }

    private:
        friend class ShaderStage<StageSelector>;
        struct _SSRec
        {
            UINT slot;
            SSObj *obj;
        };

        ShaderSamplerManager(const std::map<std::string, _SSRec> &src)
            : SSs_(src)
        {
            for(auto &it : SSs_)
                it.second.obj = new ShaderSamplerObject<StageSelector>(it.second.slot);
        }

    private:
        std::map<std::string, _SSRec> SSs_;
    };
}

#endif //OWESHADER_SAMPLER_H
