#include <omlit/core/function.hpp>

#include <lua.hpp>
#include <omlit/lua/binding.hpp>

namespace omlit
{
    Error NativeFunc::Invoke(const Params & paramsIn, Params & paramsOut)
    {
        Params cvtParamsIn, cvtParamsOut;
        size_t i;
        bool varInputBegan = false;
        bool varOutputBegan = false;
        for (i = 0; i < paramsIn.size(); ++i)
        {
            Value cvtValue;
            if (i >= typesIn.size())
            {
                return INCORRECT_INPUT_TYPE;
            }
            if (typesIn[i] == Value::NIL)
            {
                varInputBegan = true;
                break;
            }
            
            bool nilValue = paramsIn[i] == nullptr;
            
            if (!nilValue && !paramsIn[i].tryConvert(typesIn[i], cvtValue))
            {
                return INCORRECT_INPUT_TYPE;
            }
            
            cvtParamsIn.emplace_back(std::move(cvtValue));
        }
        if (varInputBegan)
        {
            for (; i < paramsIn.size(); ++i)
            {
                cvtParamsIn.emplace_back(paramsIn[i]);
            }
        }
        Error err = func(cvtParamsIn, cvtParamsOut);
        if (err != SUCCEED)
        {
            return err;
        }
        for (i = 0; i < cvtParamsOut.size(); ++i)
        {
            Value cvtValue;
            if (i >= typesOut.size())
            {
                return INCORRECT_OUTPUT_TYPE;
            }
            if (typesOut[i] == Value::NIL)
            {
                varOutputBegan = true;
                break;
            }
            bool nilValue = cvtParamsOut[i] == nullptr;
            if (!nilValue && !cvtParamsOut[i].tryConvert(typesOut[i], cvtValue))
            {
                return INCORRECT_OUTPUT_TYPE;
            }
            paramsOut.emplace_back(std::move(cvtValue));
        }
        if (varOutputBegan)
        {
            for (; i < cvtParamsOut.size(); ++i)
            {
                paramsOut.emplace_back(std::move(cvtParamsOut[i]));
            }
        }
        return SUCCEED;
    }

    LuaFunc::~LuaFunc()
    {
        luaL_unref((lua_State *) L, LUA_REGISTRYINDEX, ref);
    }
    Error LuaFunc::Invoke(const Params & paramsIn, Params & paramsOut)
    {
        lua_rawgeti((lua_State *) L, LUA_REGISTRYINDEX, ref);
        for (auto & value : paramsIn)
        {
            omlit_value_to_lua((lua_State *) L, value);
        }
        lua_call((lua_State *) L, paramsIn.size(), LUA_MULTRET);
        int rets = lua_gettop((lua_State *) L);
        for (int i = 1; i <= rets; ++i)
        {
            paramsOut.emplace_back(lua_value_to_omlit((lua_State *) L, i));
        }
        lua_pop((lua_State *) L, rets);
        return SUCCEED;
    }
}