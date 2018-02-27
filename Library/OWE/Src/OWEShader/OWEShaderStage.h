/*================================================================
Filename: OWEShaderStage.h
Date: 2017.11.26
Created by AirGuanZ
================================================================*/
#ifndef OWESHADER_STAGE_H
#define OWESHADER_STAGE_H

#include <cassert>
#include <d3d11.h>

#include "OWEShaderObjectBinding.h"
#include "OWEShaderReflection.h"
#include "OWEShaderReleaseCOMObjects.h"
#include "OWEShaderUncopiable.h"

#include "OWEShaderConstantBuffer.h"
#include "OWEShaderSampler.h"
#include "OWEShaderResource.h"

namespace OWEShaderAux
{
    template<ShaderStageSelector>
    class ShaderStageSpec;

    template<>
    class ShaderStageSpec<SS_VS>
    {
    public:
        using D3DShaderType = ID3D11VertexShader;

        static std::string DefaultCompileTarget(void)
        {
            return "vs_5_0";
        }

        static ID3D10Blob *CompileShader(
            const std::string &src, std::string *errMsg,
            const std::string &target, const std::string &entry)
        {
            if(errMsg)
                *errMsg = "";

            ID3D10Blob *rt = nullptr, *err = nullptr;
            HRESULT hr = D3DCompile(src.data(), src.size(),
                                    nullptr, nullptr, nullptr, entry.c_str(),
                                    target.c_str(), 0, 0, &rt, &err);
            if(FAILED(hr))
            {
                if(err && errMsg)
                    *errMsg = reinterpret_cast<char*>(err->GetBufferPointer());
                ReleaseCOMObjects(rt);
            }

            ReleaseCOMObjects(err);
            return rt;
        }

        static D3DShaderType *InitShader(ID3D11Device *dev, void *shaderCompiled, SIZE_T length)
        {
            assert(dev != nullptr);
            assert(shaderCompiled != nullptr && length > 0);
            D3DShaderType *rt = nullptr;
            if(FAILED(dev->CreateVertexShader(shaderCompiled, length, nullptr, &rt)))
                return nullptr;
            return rt;
        }

        static void BindShader(ID3D11DeviceContext *DC, D3DShaderType *shader)
        {
            assert(DC != nullptr);
            DC->VSSetShader(shader, nullptr, 0);
        }
    };

    template<>
    class ShaderStageSpec<SS_GS>
    {
    public:
        using D3DShaderType = ID3D11GeometryShader;

        static std::string DefaultCompileTarget(void)
        {
            return "gs_5_0";
        }

        static ID3D10Blob *CompileShader(
            const std::string &src, std::string *errMsg,
            const std::string &target, const std::string &entry)
        {
            if(errMsg)
                *errMsg = "";

            ID3D10Blob *rt = nullptr, *err = nullptr;
            HRESULT hr = D3DCompile(src.data(), src.size(),
                                    nullptr, nullptr, nullptr, entry.c_str(),
                                    target.c_str(), 0, 0, &rt, &err);
            if(FAILED(hr))
            {
                if(err && errMsg)
                    *errMsg = reinterpret_cast<char*>(err->GetBufferPointer());
                ReleaseCOMObjects(rt);
            }

            ReleaseCOMObjects(err);
            return rt;
        }

        static D3DShaderType *InitShader(ID3D11Device *dev, void *shaderCompiled, SIZE_T length)
        {
            assert(dev != nullptr);
            assert(shaderCompiled != nullptr && length > 0);
            D3DShaderType *rt = nullptr;
            if(FAILED(dev->CreateGeometryShader(shaderCompiled, length, nullptr, &rt)))
                return nullptr;
            return rt;
        }

        static void BindShader(ID3D11DeviceContext *DC, D3DShaderType *shader)
        {
            assert(DC != nullptr);
            DC->GSSetShader(shader, nullptr, 0);
        }
    };

    template<>
    class ShaderStageSpec<SS_PS>
    {
    public:
        using D3DShaderType = ID3D11PixelShader;

        static std::string DefaultCompileTarget(void)
        {
            return "ps_5_0";
        }

        static ID3D10Blob *CompileShader(
            const std::string &src, std::string *errMsg,
            const std::string &target, const std::string &entry)
        {
            if(errMsg)
                *errMsg = "";

            ID3D10Blob *rt = nullptr, *err = nullptr;
            HRESULT hr = D3DCompile(src.data(), src.size(),
                nullptr, nullptr, nullptr, entry.c_str(),
                target.c_str(), 0, 0, &rt, &err);
            if(FAILED(hr))
            {
                if(err && errMsg)
                    *errMsg = reinterpret_cast<char*>(err->GetBufferPointer());
                ReleaseCOMObjects(rt);
            }

            ReleaseCOMObjects(err);
            return rt;
        }

        static D3DShaderType *InitShader(ID3D11Device *dev, void *shaderCompiled, SIZE_T length)
        {
            assert(dev != nullptr);
            assert(shaderCompiled != nullptr && length > 0);
            D3DShaderType *rt = nullptr;
            if(FAILED(dev->CreatePixelShader(shaderCompiled, length, nullptr, &rt)))
                return nullptr;
            return rt;
        }

        static void BindShader(ID3D11DeviceContext *DC, D3DShaderType *shader)
        {
            assert(DC != nullptr);
            DC->PSSetShader(shader, nullptr, 0);
        }
    };

    template<ShaderStageSelector StageSelector>
    class ShaderStage : public Uncopiable
    {
    public:
        using StageSpec = ShaderStageSpec<StageSelector>;

        static constexpr ShaderStageSelector Stage = StageSelector;

        ShaderStage(void)
            : shaderByteCode_(nullptr), shader_(nullptr)
        {

        }

        bool Initialize(ID3D11Device *dev, const std::string &src, std::string &errMsg,
                        const std::string &target = StageSpec::DefaultCompileTarget(),
                        const std::string &entry = "main")
        {
            assert(dev != nullptr);

            shaderByteCode_ = StageSpec::CompileShader(src, &errMsg, target, entry);
            if(!shaderByteCode_)
                return false;

            shader_ = StageSpec::InitShader(dev, shaderByteCode_->GetBufferPointer(),
                shaderByteCode_->GetBufferSize());
            if(!shader_)
            {
                ReleaseCOMObjects(shaderByteCode_);
                return false;
            }

            return InitializeShaderRecords();
        }

        bool Initialize(ID3D11Device *dev, ID3D10Blob *shaderByteCode)
        {
            assert(dev != nullptr && shaderByteCode != nullptr);

            shaderByteCode_ = shaderByteCode;
            shaderByteCode_->AddRef();
            shader_ = StageSpec::InitShader(dev, shaderByteCode_->GetBufferPointer(),
                shaderByteCode_->GetBufferSize());
            if(!shader_)
            {
                ReleaseCOMObjects(shaderByteCode_);
                return false;
            }

            return InitializeShaderRecords();
        }

        ~ShaderStage(void)
        {
            ReleaseCOMObjects(shader_, shaderByteCode_);
        }

        void BindShader(ID3D11DeviceContext *DC)
        {
            assert(DC != nullptr);
            StageSpec::BindShader(DC, shader_);
        }

        void UnbindShader(ID3D11DeviceContext *DC)
        {
            assert(DC != nullptr);
            StageSpec::BindShader(DC, nullptr);
        }

        ConstantBufferManager<StageSelector> *CreateConstantBufferManager(void) const
        {
            return new ConstantBufferManager<StageSelector>(emptyCBRec_);
        }

        ShaderResourceManager<StageSelector> *CreateShaderResourceManager(void) const
        {
            return new ShaderResourceManager<StageSelector>(emptySRRec_);
        }

        ShaderSamplerManager<StageSelector> *CreateShaderSamplerManager(void) const
        {
            return new ShaderSamplerManager<StageSelector>(emptySSRec_);
        }

        size_t GetConstantBufferCount(void) const
        {
            return emptyCBRec_.size();
        }

        size_t GetShaderResourceCount(void) const
        {
            return emptySRRec_.size();
        }

        size_t GetShaderSamplerCount(void) const
        {
            return emptySSRec_.size();
        }

        const void *GetShaderByteCode(void) const
        {
            assert(shaderByteCode_);
            return shaderByteCode_->GetBufferPointer();
        }

        UINT GetShaderByteCodeSize(void) const
        {
            assert(shaderByteCode_);
            return static_cast<UINT>(shaderByteCode_->GetBufferSize());
        }

    private:
        bool InitializeShaderRecords(void)
        {
            ID3D11ShaderReflection *ref = GetShaderReflection(shaderByteCode_->GetBufferPointer(),
                                                               shaderByteCode_->GetBufferSize());
            if(!ref)
                return false;

            std::map<std::string, CBInfo> CBInfos;
            GetConstantBuffers(ref, &CBInfos);
            for(auto &it : CBInfos)
                emptyCBRec_[it.first] = { it.second.slot, it.second.byteSize, nullptr };
            CBInfos.clear();

            //将形如name[cnt]的名字删减为name本身
            auto delLexArr = [](const std::string &name)->std::string
            {
                return name.substr(0, name.find('['));
            };

            std::map<std::string, SRInfo> SRInfos;
            GetShaderResources(ref, &SRInfos);
            for(auto &it : SRInfos)
                emptySRRec_[delLexArr(it.first)] = { it.second.slot, it.second.cnt, nullptr };
            SRInfos.clear();

            std::map<std::string, UINT> SSamInfos;
            GetShaderSamplers(ref, &SSamInfos);
            for(auto &it : SSamInfos)
                emptySSRec_[it.first] = { it.second, nullptr };
            SSamInfos.clear();

            ReleaseCOMObjects(ref);
            return true;
        }

    private:
        typename StageSpec::D3DShaderType *shader_;
        ID3D10Blob *shaderByteCode_;

        std::map<std::string, typename ConstantBufferManager<StageSelector>::CBRec> emptyCBRec_;
        std::map<std::string, typename ShaderResourceManager<StageSelector>::_SRRec> emptySRRec_;
        std::map<std::string, typename ShaderSamplerManager<StageSelector>::_SSRec> emptySSRec_;
    };
}

#endif //OWESHADER_STAGE_H
