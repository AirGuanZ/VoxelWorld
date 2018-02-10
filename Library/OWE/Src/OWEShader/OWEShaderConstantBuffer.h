/*================================================================
Filename: OWEShaderConstantBuffer.h
Date: 2017.11.23
Created by AirGuanZ
================================================================*/
#ifndef OWESHADER_CONSTANT_BUFFER_H
#define OWESHADER_CONSTANT_BUFFER_H

#include <cstring>
#include <map>
#include <typeinfo>
#include <vector>

#include "OWEShaderFatalError.h"
#include "OWEShaderGenBuffer.h"
#include "OWEShaderObjectBinding.h"
#include "OWEShaderReleaseCOMObjects.h"
#include "OWEShaderUncopiable.h"

namespace OWEShaderAux
{
    template<ShaderStageSelector ShaderSelector>
    class ConstantBufferObjectBase;

    template<typename BufferType_, ShaderStageSelector _StageSelector, bool _IsDynamic>
    class ConstantBufferAttributes;

    template<typename BufferType_, ShaderStageSelector _StageSelector, bool _IsDynamic>
    class ConstantBufferObject;

    template<ShaderStageSelector>
    class ShaderStage;

    template<ShaderStageSelector StageSelector>
    class ConstantBufferManager;

    template<ShaderStageSelector ShaderSelector>
    class ConstantBufferObjectBase
    {
    public:
        ConstantBufferObjectBase(UINT slot, ID3D11Buffer *buf)
            : slot_(slot), buf_(buf)
        {

        }

        virtual ~ConstantBufferObjectBase(void)
        {

        }

        void Bind(ID3D11DeviceContext *devCon) const
        {
            assert(devCon);
            BindConstantBuffer<ShaderSelector>(devCon, slot_, buf_);
        }

        void Unbind(ID3D11DeviceContext *devCon) const
        {
            assert(devCon);
            BindConstantBuffer<ShaderSelector>(devCon, slot_, nullptr);
        }
        
    protected:
        UINT slot_;
        ID3D11Buffer *buf_;
    };

    template<typename BufferType_, ShaderStageSelector _StageSelector, bool _IsDynamic>
    class ConstantBufferAttributes
    {
    public:
        using MyType = ConstantBufferObject<BufferType_, _StageSelector, _IsDynamic>;
        using BufferType = BufferType_;

        static constexpr ShaderStageSelector ShaderStage = _StageSelector;
        static constexpr bool IsDynamic = _IsDynamic;
    };

    //Static constant buffer object
    template<typename BufferType_, ShaderStageSelector _StageSelector>
    class ConstantBufferObject<BufferType_, _StageSelector, false>
        : public ConstantBufferAttributes<BufferType_, _StageSelector, false>,
          public ConstantBufferObjectBase<_StageSelector>,
          public Uncopiable
    {
    private:
        friend class ConstantBufferManager<_StageSelector>;

        ConstantBufferObject(void)
            : ConstantBufferObjectBase(0, nullptr)
        {

        }

        bool Initialize(ID3D11DeviceContext *devCon, UINT slot, const BufferType_ *data)
        {
            assert(devCon && data);
            slot_ = slot;
            D3D11_SUBRESOURCE_DATA dataDesc = { &data, 0, 0 };
            buf_ = GenConstantBuffer(dev, sizeof(BufferType_), false, &dataDesc);
            return buf_ != nullptr;
        }

        ~ConstantBufferObject(void)
        {
            ReleaseCOMObjects(buf_);
        }
    };

    //Dynamic constant buffer object
    template<typename BufferType_, ShaderStageSelector _StageSelector>
    class ConstantBufferObject<BufferType_, _StageSelector, true>
        : public ConstantBufferAttributes<BufferType_, _StageSelector, true>,
          public ConstantBufferObjectBase<_StageSelector>,
          public Uncopiable
    {
    public:
        void SetBufferData(ID3D11DeviceContext *devCon, const BufferType &data)
        {
            assert(devCon != nullptr && buf_ != nullptr);
            D3D11_MAPPED_SUBRESOURCE mappedRsc;
            devCon->Map(buf_, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedRsc);
            std::memcpy(mappedRsc.pData, &data, sizeof(BufferType_));
            devCon->Unmap(buf_, 0);
        }

    private:
        friend class ConstantBufferManager<_StageSelector>;

        ConstantBufferObject(void)
            : ConstantBufferObjectBase(0, nullptr)
        {

        }

        bool Initialize(ID3D11Device *dev, UINT slot, const BufferType_ *data = nullptr)
        {
            assert(dev);
            slot_ = slot;
            if(data)
            {
                D3D11_SUBRESOURCE_DATA dataDesc = { &data, 0, 0 };
                buf_ = GenConstantBuffer(dev, sizeof(BufferType_), true, &dataDesc);
            }
            else
                buf_ = GenConstantBuffer(dev, sizeof(BufferType_), true, nullptr);
            return buf_ != nullptr;
        }

        ~ConstantBufferObject(void)
        {
            ReleaseCOMObjects(buf_);
        }
    };

    template<ShaderStageSelector StageSelector>
    class ConstantBufferManager : public Uncopiable
    {
    public:
        ConstantBufferManager(void) = default;

        ~ConstantBufferManager(void)
        {
            for(auto &it : CBs_)
                SafeDeleteObjects(it.second.obj);
        }

        bool AddBuffer(const std::string &name, UINT slot, UINT byteSize)
        {
            assert(!name.empty() && byteSize > 0);
            if(CBs_.find(name) != CBs_.end())
                return false;
            CBs_[name] = CBRec{ slot, byteSize, nullptr };
            return true;
        }

        template<typename BufferType, bool IsDynamic = true>
        ConstantBufferObject<BufferType, StageSelector, IsDynamic> *
        GetConstantBuffer(ID3D11Device *dev, const std::string &name, const BufferType *data = nullptr)
        {
            using ResultType = ConstantBufferObject<BufferType, StageSelector, IsDynamic>;

            assert(dev);
            assert(IsDynamic || data);

            auto &it = CBs_.find(name);
            if(it == CBs_.end())
#ifdef OWE_NO_EXCEPTION
                return nullptr;
#else
                throw OWEShaderError(("Constant buffer not found: " + name).c_str());
#endif
            CBRec &rec = it->second;

            if(rec.obj)
            {
                if(typeid(*rec.obj) != typeid(ConstantBufferObject<BufferType, StageSelector, IsDynamic>))
                {
#ifdef OWE_NO_EXCEPTION
                    return nullptr;
#else
                    throw OWEShaderError("Inconsistent constant buffer type");
#endif
                }
                return reinterpret_cast<ResultType*>(rec.obj);
            }

            if(sizeof(BufferType) != rec.byteSize)
            {
#ifdef OWE_NO_EXCEPTION
                return nullptr;
#else
                throw OWEShaderError("Inconstent constant buffer size");
#endif
            }

            ResultType *rtObj = new ResultType;
            if(!rtObj->Initialize(dev, rec.slot, data))
            {
                delete rtObj;
#ifdef OWE_NO_EXCEPTION
                return nullptr;
#else
                throw OWEShaderError("Failed to initialize constant buffer object: " + name);
#endif
            }
            rec.obj = rtObj;
            return rtObj;
        }

        void Bind(ID3D11DeviceContext *DC)
        {
            for(auto it : CBs_)
            {
                if(it.second.obj)
                    it.second.obj->Bind(DC);
            }
        }

        void Unbind(ID3D11DeviceContext *DC)
        {
            for(auto it : CBs_)
            {
                if(it.second.obj)
                    it.second.obj->Unbind(DC);
            }
        }

    private:
        friend class ShaderStage<StageSelector>;

        struct CBRec
        {
            UINT slot;
            UINT byteSize;
            ConstantBufferObjectBase<StageSelector> *obj;
        };
        using CBTable = std::map<std::string, CBRec>;
        
        ConstantBufferManager(const CBTable &src)
            : CBs_(src)
        {

        }

    private:
        CBTable CBs_;
    };
}

#endif //OWESHADER_CONSTANT_BUFFER_H
