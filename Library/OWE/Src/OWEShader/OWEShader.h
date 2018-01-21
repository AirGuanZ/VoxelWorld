/*================================================================
Filename: OWEShader.h
Date: 2017.12.6
Created by AirGuanZ
================================================================*/
#ifndef OWESHADER_H
#define OWESHADER_H

#include <tuple>
#include <utility>

#include "OWEShaderFatalError.h"
#include "OWEShaderReleaseCOMObjects.h"
#include "OWEShaderStage.h"
#include "OWEShaderUniforms.h"

namespace OWEShaderAux
{
    template<typename Func, typename Tu, size_t FI>
    inline void DoForTupleElementsAux(Func func, Tu &tu, std::index_sequence<FI>)
    {
        func(std::get<FI>(tu));
    }

    template<typename Func, typename Tu, size_t FI, size_t...OI>
    inline void DoForTupleElementsAux(Func func, Tu &tu, std::index_sequence<FI, OI...>)
    {
        func(std::get<FI>(tu));
        DoForTupleElementsAux<Func, Tu, OI...>(func, tu, std::index_sequence<OI...>());
    }

    template<typename Func, typename Tu>
    inline void DoForTupleElements(Func func, Tu &tu)
    {
        DoForTupleElementsAux<Func, Tu>(func, tu, std::make_index_sequence<std::tuple_size<Tu>::value>());
    }

    template<int v>
    inline constexpr int FindInNumListAux(void)
    {
        return v < 0 ? v : (v + 1);
    }

    template<typename NumType, NumType Dst>
    inline constexpr int FindInNumList(void)
    {
        return -1;
    }

    template<typename NumType, NumType Dst, NumType First, NumType...Others>
    inline constexpr int FindInNumList(void)
    {
        return Dst == First ? 0 : FindInNumListAux<FindInNumList<NumType, Dst, Others...>()>();
    }

    template<typename NumType>
    inline constexpr bool IsRepeated(void)
    {
        return false;
    }

    template<typename NumType, NumType First, NumType...Others>
    inline constexpr bool IsRepeated(void)
    {
        return (FindInNumList<NumType, First, Others...>() >= 0) || IsRepeated<NumType, Others...>();
    }

    struct ShaderStagePtrInitializer
    {
        template<typename T>
        void operator()(T *&ptr)
        {
            ptr = nullptr;
        }
    };

    struct ShaderStageDeleter
    {
        template<typename T>
        void operator()(T *&ptr)
        {
            SafeDeleteObjects(ptr);
        }
    };

    struct ShaderStageBinder
    {
        template<typename T>
        void operator()(T *pStage)
        {
            assert(pStage != nullptr);
            pStage->BindShader(DC_);
        }
        ID3D11DeviceContext *DC_;
    };

    struct ShaderStageAvailableRec
    {
        template<typename T>
        void operator()(const T *ptr)
        {
            *available = *available && (ptr != nullptr);
        }

        bool *available;
    };

    struct ShaderStageUnbinder
    {
        template<typename T>
        void operator()(T *pStage)
        {
            assert(pStage != nullptr);
            pStage->UnbindShader(DC_);
        }
        ID3D11DeviceContext *DC_;
    };

    template<typename TStages, size_t...I>
    inline auto CreateShaderUniformManagerAux(const TStages &stages, std::index_sequence<I...>)
    {
        return new ShaderUniformManager<std::remove_pointer_t<std::tuple_element_t<I, TStages>>::Stage...>
                        ((*std::get<I>(stages))...);
    }

    template<ShaderStageSelector...StageSelectors>
    class Shader
    {
    public:
        Shader(void)
        {
            static_assert((IsRepeated<ShaderStageSelector, StageSelectors...>() == false), "Shader stage repeated");
            static_assert((FindInNumList<ShaderStageSelector, SS_VS, StageSelectors...>() != -1), "Vertex shader not found");
            static_assert((FindInNumList<ShaderStageSelector, SS_PS, StageSelectors...>() != -1), "Pixel shader not found");

            ShaderStagePtrInitializer setter;
            DoForTupleElements(setter, stages_);
        }

        ~Shader(void)
        {
            Destroy();
        }

        template<ShaderStageSelector StageSelector>
        bool InitStage(ID3D11Device *dev, const std::string &src, std::string *errMsg = nullptr,
                       const std::string &target = ShaderStage<StageSelector>::StageSpec::DefaultCompileTarget(),
                       const std::string &entry = "main")
        {
            std::string dummyErrMsg, *tErrMsg = errMsg ? errMsg : &dummyErrMsg;
            auto &pStage = std::get<FindInNumList<ShaderStageSelector, StageSelector, StageSelectors...>()>(stages_);
            SafeDeleteObjects(pStage);
            pStage = new ShaderStage<StageSelector>;
            if(!pStage->Initialize(dev, src, *tErrMsg, target, entry))
            {
                SafeDeleteObjects(pStage);
#ifdef OWE_NO_EXCEPTION
                return false;
#else
                throw OWEShaderError(*tErrMsg);
#endif
            }
            return true;
        }

        template<ShaderStageSelector StageSelector>
        bool InitStage(ID3D11Device *dev, ID3D10Blob *shaderByteCode)
        {
            auto &pStage = std::get<FindInNumList<ShaderStageSelector, StageSelector, StageSelectors...>()>(stages_);
            SafeDeleteObjects(pStage);
            pStage = new ShaderStage<StageSelector>;
            if(!pStage->Initialize(dev, shaderByteCode))
            {
                SafeDeleteObjects(pStage);
#ifdef OWE_NO_EXCEPTION
                return false;
#else
                throw OWEShaderError("Failed to initialize shader from shader byte code");
#endif
            }
            return true;
        }

        void Destroy(void)
        {
            ShaderStageDeleter deleter;
            DoForTupleElements(deleter, stages_);
        }

        bool IsAllStagesAvailable(void) const
        {
            bool available = true;
            ShaderStageAvailableRec rec = { &available };
            DoForTupleElements(rec, stages_);
            return available;
        }

        template<ShaderStageSelector StageSelector>
        ShaderStage<StageSelector> *GetStage(void)
        {
            return std::get<FindInNumList<ShaderStageSelector, StageSelector, StageSelectors...>()>(stages_);
        }

        template<ShaderStageSelector StageSelector>
        ConstantBufferManager<StageSelector> *CreateConstantBufferManager(void)
        {
            return GetStage<StageSelector>()->CreateConstantBufferManager();
        }

        template<ShaderStageSelector StageSelector>
        ShaderResourceManager<StageSelector> *CreateShaderResourceManager(void)
        {
            return GetStage<StageSelector>()->CreateShaderResourceManager();
        }

        template<ShaderStageSelector StageSelector>
        ShaderSamplerManager<StageSelector> *CreateShaderSamplerManager(void)
        {
            return GetStage<StageSelector>()->CreateShaderSamplerManager();
        }

        ShaderUniformManager<StageSelectors...> *CreateUniformManager(void)
        {
            return CreateShaderUniformManagerAux(stages_,
                std::make_index_sequence<std::tuple_size_v<
                    std::tuple<ShaderStage<StageSelectors>*...>>>());
        }

        const void *GetShaderByteCodeWithInputSignature(void)
        {
            return GetStage<SS_VS>()->GetShaderByteCode();
        }

        UINT GetShaderByteCodeSizeWithInputSignature(void)
        {
            return GetStage<SS_VS>()->GetShaderByteCodeSize();
        }

        void Bind(ID3D11DeviceContext *DC)
        {
            ShaderStageBinder binder = { DC };
            DoForTupleElements(binder, stages_);
        }

        void Unbind(ID3D11DeviceContext *DC)
        {
            ShaderStageUnbinder unbinder = { DC };
            DoForTupleElements(unbinder, stages_);
        }

    private:
        std::tuple<ShaderStage<StageSelectors>*...> stages_;
    };
}

namespace OWE
{
    using Error = OWEShaderAux::OWEShaderError;

    using ShaderStageSelector = OWEShaderAux::ShaderStageSelector;

    template<ShaderStageSelector StageSelector, typename BufferType, bool Dynamic = true>
    using ConstantBufferObject = OWEShaderAux::ConstantBufferObject<BufferType, StageSelector, Dynamic>;

    template<ShaderStageSelector StageSelector>
    using ShaderResourceObject = OWEShaderAux::ShaderResourceObject<StageSelector>;

    template<ShaderStageSelector StageSelector>
    using ShaderSamplerObject = OWEShaderAux::ShaderSamplerObject<StageSelector>;

    template<ShaderStageSelector StageSelector>
    using ConstantBufferManager = OWEShaderAux::ConstantBufferManager<StageSelector>;

    template<ShaderStageSelector StageSelector>
    using ShaderResourceManager = OWEShaderAux::ShaderResourceManager<StageSelector>;

    template<ShaderStageSelector StageSelector>
    using ShaderSamplerManager = OWEShaderAux::ShaderSamplerManager<StageSelector>;

    template<ShaderStageSelector StageSelector>
    using ShaderStage = OWEShaderAux::ShaderStage<StageSelector>;

    template<ShaderStageSelector...StageSelectors>
    using Shader = OWEShaderAux::Shader<StageSelectors...>;
}

using OWEShaderAux::SS_VS;
using OWEShaderAux::SS_GS;
using OWEShaderAux::SS_PS;

#endif //OWESHADER_H
