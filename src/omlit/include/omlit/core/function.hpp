#pragma once

#include "value.hpp"

namespace omlit
{
    struct BaseFunc
    {
        enum Type
        {
            NONE,
            NATIVE,
            LUA,
        };
        typedef std::vector<Value> Params;
        BaseFunc() = default;
        BaseFunc(const BaseFunc & other) = delete;
        virtual ~BaseFunc() = default;
        virtual Error Invoke(const Params & paramsIn, Params & paramsOut) = 0;
        Type type = NONE;
    };

    struct NativeFunc : public BaseFunc
    {
        typedef std::vector<Value::Type> Types;
        typedef std::function<Error(const Params &, Params &)> NativeFuncType;
        NativeFunc(const Types & typesIn, const Types & typesOut, const NativeFuncType & func)
        {
            type = NATIVE;
            this->typesIn = typesIn;
            this->typesOut = typesOut;
            this->func = func;
        }
        static Value CreateAsValue(const Types & typesIn, const Types & typesOut, const NativeFuncType & func)
        {
            return std::static_pointer_cast<BaseFunc>(std::make_shared<NativeFunc>(
                typesIn, typesOut, func
            ));
        }
        virtual Error Invoke(const Params & paramsIn, Params & paramsOut) override;
        std::vector<Value::Type> typesIn;
        std::vector<Value::Type> typesOut;
        NativeFuncType func;
    };

    struct LuaFunc : public BaseFunc
    {
        LuaFunc(void * L, int ref)
        {
            this->L = L;
            this->ref = ref;
        }
        virtual ~LuaFunc();
        static Value CreateAsValue(void * L, int ref)
        {
            return std::static_pointer_cast<BaseFunc>(std::make_shared<LuaFunc>(
                L, ref
            ));
        }
        virtual Error Invoke(const Params & paramsIn, Params & paramsOut) override;
        void * L;
        int ref;
    };
}