/*================================================================
Filename: OWEShaderResource.h
Date: 2017.11.24
Created by AirGuanZ
================================================================*/
#ifndef OWESHADER_RESOURCE_H
#define OWESHADER_RESOURCE_H

#include <map>

#include "OWEShaderFatalError.h"
#include "OWEShaderObjectBinding.h"
#include "OWEShaderReleaseCOMObjects.h"
#include "OWEShaderUncopiable.h"

namespace OWEShaderAux
{
    template<ShaderStageSelector StageSelector>
    class ShaderResourceObject;

    template<ShaderStageSelector>
    class ShaderStage;

    template<ShaderStageSelector StageSelector>
    class ShaderResourceManager;

    template<ShaderStageSelector StageSelector>
    class ShaderResourceObject : public Uncopiable
    {
    public:
        void SetShaderResource(ID3D11ShaderResourceView *SRV)
        {
            assert(SRVs_.size() == 1);
            SRVs_[0] = SRV;
            if(SRV)
                SRV->AddRef();
        }

        void SetShaderResource(int idx, ID3D11ShaderResourceView *SRV)
        {
            assert(0 <= idx && idx < SRVs_.size());
            SRVs_[idx] = SRV;
            if(SRV)
                SRV->AddRef();
        }

        void SetShaderResource(ID3D11ShaderResourceView **SRVs)
        {
            assert(SRVs != nullptr);
            std::memcpy(SRVs_.data(), SRVs, sizeof(ID3D11ShaderResourceView*) * SRVs_.size());
            for(UINT i = 0; i != SRVs_.size(); ++i)
            {
                if(SRVs_[i])
                    SRVs_[i]->Release();
                SRVs_[i] = SRVs[i];
                if(SRVs_[i])
                    SRVs_[i]->AddRef();
            }
        }

        void Bind(ID3D11DeviceContext *DC)
        {
            assert(DC != nullptr);
            BindShaderResourceArray<StageSelector>(DC, slot_, static_cast<UINT>(SRVs_.size()), SRVs_.data());
        }

        void Unbind(ID3D11DeviceContext *DC)
        {
            assert(DC != nullptr);
            std::vector<ID3D11ShaderResourceView*> emptySRVs(SRVs_.size(), nullptr);
            BindShaderResourceArray<StageSelector>(DC, slot_, static_cast<UINT>(emptySRVs.size()), emptySRVs.data());
        }

    private:
        friend class ShaderResourceManager<StageSelector>;

        ShaderResourceObject(UINT slot, UINT cnt, ID3D11ShaderResourceView **SRVs = nullptr)
            : slot_(slot), SRVs_(cnt)
        {
            assert(cnt != 0);
            if(SRVs)
            {
                for(UINT i = 0; i != cnt; ++i)
                {
                    SRVs_[i] = SRVs[i];
                    if(SRVs[i])
                        SRVs[i]->AddRef();
                }
            }
        }
        
        ~ShaderResourceObject(void)
        {
            for(auto *p : SRVs_)
            {
                if(p)
                    p->Release();
            }
        }

    private:
        UINT slot_;
        std::vector<ID3D11ShaderResourceView*> SRVs_;
    };

    template<ShaderStageSelector StageSelector>
    class ShaderResourceManager : public Uncopiable
    {
    public:
        using RscObj = ShaderResourceObject<StageSelector>;

        ~ShaderResourceManager(void)
        {
            for(auto it : SRs_)
                delete it.second.obj;
        }

        bool AddShaderResource(const std::string &name, UINT slot, UINT cnt, ID3D11ShaderResourceView **initSRV = nullptr)
        {
            auto it = SRs_.find(name);
            if(it != SRs_.end())
                return false;
            SRs_[name] = _SRRec{ slot, cnt, new RscObj(slot, cnt, initSRV) };
            return true;
        }
        
        RscObj *GetShaderResourceObject(const std::string &name)
        {
            auto it = SRs_.find(name);
            if(it == SRs_.end())
            {
#ifdef OWE_NO_EXCEPTION
                return nullptr;
#else
                throw OWEShaderError("Shader resource not found: " + name);
#endif
            }
            
            assert(it->second.obj != nullptr);
            return it->second.obj;
        }

        void Bind(ID3D11DeviceContext *DC)
        {
            for(auto it : SRs_)
            {
                assert(it.second.obj != nullptr);
                it.second.obj->Bind(DC);
            }
        }

        void Unbind(ID3D11DeviceContext *DC)
        {
            for(auto it : SRs_)
            {
                assert(it.second.obj != nullptr);
                it.second.obj->Unbind(DC);
            }
        }

    private:
        friend class ShaderStage<StageSelector>;

        struct _SRRec
        {
            UINT slot;
            UINT cnt;
            RscObj *obj;
        };

        ShaderResourceManager(const std::map<std::string, _SRRec> &src)
            : SRs_(src)
        {
            for(auto &it : SRs_)
                it.second.obj = new RscObj(it.second.slot, it.second.cnt);
        }

    private:
        std::map<std::string, _SRRec> SRs_;
    };
}

#endif //OWESHADER_RESOURCE_H
