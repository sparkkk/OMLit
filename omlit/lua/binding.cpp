#include <omlit/lua/binding.hpp>
#include <omlit/core/function.hpp>
#include <omlit/core/object.hpp>

namespace omlit
{

#define METANAME "OmLitValue"

static int cf_ValueGC(lua_State * L)
{
    const Value * p = reinterpret_cast<const Value *>(lua_touserdata(L, 1));
    p->~Value();
    lua_pop(L, lua_gettop(L));
    return 0;
}

static int cf_ValueInvoke(lua_State * L)
{
    int args = lua_gettop(L);
    Value value = lua_value_to_omlit(L, 1);
    if (value.asFunc() != nullptr)
    {
        BaseFunc::Params paramsIn, paramsOut;
        for (int i = 2; i <= args; ++i)
        {
            paramsIn.emplace_back(lua_value_to_omlit(L, i));
        }
        lua_pop(L, args);
        if (value.asFunc()->Invoke(paramsIn, paramsOut) != Error::SUCCEED)
        {
            return 0;
        }
        for (auto & out : paramsOut)
        {
            omlit_value_to_lua(L, out);
        }
        return paramsOut.size();
    }
    else if (value.asObj() != nullptr)
    {
        Value ctorFunc;
        if (value.asObj()->GetMember("<CTOR>", ctorFunc) == Error::SUCCEED)
        {
            BaseFunc::Params paramsIn, paramsOut;
            for (int i = 2; i <= args; ++i)
            {
                paramsIn.emplace_back(lua_value_to_omlit(L, i));
            }
            lua_pop(L, args);
            if (ctorFunc.asFunc()->Invoke(paramsIn, paramsOut) != Error::SUCCEED)
            {
                return 0;
            }
            for (auto & out : paramsOut)
            {
                omlit_value_to_lua(L, out);
            }
            return paramsOut.size();
        }
    }
    
    //TODO
    lua_pop(L, lua_gettop(L));
    return 0;
}

static int cf_ValueIndex(lua_State * L)
{
    Value obj = lua_value_to_omlit(L, 1);
    if (obj.asObj() == nullptr)
    {
        //TODO
        lua_pop(L, lua_gettop(L));
        return 0;
    }
    const char * id = lua_tostring(L, 2);
    lua_pop(L, lua_gettop(L));
    
    Value res;
    if (obj.asObj()->GetMember(id, res) == SUCCEED)
    {
        omlit_value_to_lua(L, res);
        return 1;
    }
    return 0;
}

static int cf_ValueNewIndex(lua_State * L)
{
    Value obj = lua_value_to_omlit(L, 1);
    if (obj.asObj() == nullptr)
    {
        //TODO
        lua_pop(L, lua_gettop(L));
        return 0;
    }
    const char * id = lua_tostring(L, 2);
    Value val = lua_value_to_omlit(L, 3);
    lua_pop(L, lua_gettop(L));
    if (obj.asObj()->SetMember(id, val) == SUCCEED)
    {
        return 0;
    }
    return 0;
}

static void registerMetatable(lua_State * L)
{
    luaL_newmetatable(L, METANAME);
    lua_pushcfunction(L, cf_ValueGC);
    lua_setfield(L, -2, "__gc");
    lua_pushcfunction(L, cf_ValueIndex);
    lua_setfield(L, -2, "__index");
    lua_pushcfunction(L, cf_ValueNewIndex);
    lua_setfield(L, -2, "__newindex");
    lua_pushcfunction(L, cf_ValueInvoke);
    lua_setfield(L, -2, "__call");
    lua_pop(L, 1);
}

bool init_lua_interop(lua_State * L)
{
    registerMetatable(L);
    return true;
}

bool register_lua_library(lua_State * L, IModuleRegistry * module)
{
    lua_getglobal(L, "package");
    lua_getfield(L, -1, "loaded");
    lua_newtable(L);

    std::vector<std::string> list;
    module->ListIDs(list);
    for (auto & id : list)
    {
        Value obj;
        module->Query(id, obj);
        omlit_value_to_lua(L, obj);
        lua_setfield(L, -2, id.c_str());
    }

    lua_setfield(L, -2, module->GetModuleID().name.c_str());
    lua_pop(L, 2);

    return true;
}

Value lua_value_to_omlit(lua_State * L, int inx)
{
    inx = lua_absindex(L, inx);
    int luaType = lua_type(L, inx);
    switch (luaType)
    {
    case LUA_TNIL:
        return nullptr;
    case LUA_TNUMBER:
        return (double) lua_tonumber(L, inx);
    case LUA_TBOOLEAN:
        return (bool) lua_toboolean(L, inx);
    case LUA_TSTRING:
        return std::string(lua_tostring(L, inx));
    case LUA_TTABLE:
        {
            auto table = std::make_shared<Value::TableType>();
            lua_pushnil(L);
            while (lua_next(L, inx) != 0)
            {
                table->insert({
                    lua_value_to_omlit(L, -2),
                    lua_value_to_omlit(L, -1)
                });
                lua_pop(L, 1);
            }
            return table;
        }
    case LUA_TFUNCTION:
        {
            lua_pushvalue(L, inx);
            int ref = luaL_ref(L, LUA_REGISTRYINDEX);
            return LuaFunc::CreateAsValue(L, ref);
        }
    case LUA_TUSERDATA:
        //TODO: Check metatable
        return *reinterpret_cast<const Value *>(lua_touserdata(L, inx));
    default:
        //TODO: Save Ref
        return nullptr;
    }
}

void omlit_value_to_lua(lua_State * L, const Value & value)
{
    switch (value.getType())
    {
    case Value::NIL:
        lua_pushnil(L);
        break;
    case Value::BLN:
        lua_pushboolean(L, Value::bool_t(value) ? 1 : 0);
        break;
    case Value::I32:
        lua_pushinteger(L, Value::i32_t(value));
        break;
    case Value::I64:
        lua_pushinteger(L, Value::i64_t(value));
        break;
    case Value::F32:
        lua_pushnumber(L, Value::f32_t(value));
        break;
    case Value::F64:
        lua_pushnumber(L, Value::f64_t(value));
        break;
    case Value::STR:
        lua_pushstring(L, value.asStr().c_str());
        break;
    case Value::CTM:
        {
            void * block = lua_newuserdata(L, sizeof(Value));
            new(reinterpret_cast<Value *>(block)) Value(value);
            luaL_setmetatable(L, METANAME);
        }
        break;
    case Value::OBJ:
        {
            void * block = lua_newuserdata(L, sizeof(Value));
            new(reinterpret_cast<Value *>(block)) Value(value);
            luaL_setmetatable(L, METANAME);
        }
        break;
    case Value::ARY:
        {
            lua_newtable(L);
            auto array = value.asArray();
            for (int i = 0; i < array->size(); ++i)
            {
                omlit_value_to_lua(L, array->at(i));
                lua_seti(L, -2, i + 1);
            }
        }
        break;
    case Value::TAB:
        {
            lua_newtable(L);
            auto table = value.asTable();
            for (auto & entry : *table)
            {
                omlit_value_to_lua(L, entry.first);
                omlit_value_to_lua(L, entry.second);
                lua_settable(L, -3);
            }
        }
        break;
    case Value::FCN:
        {
            if (value.asFunc()->type == BaseFunc::LUA)
            {
                const auto & luaFunc = std::static_pointer_cast<LuaFunc>(value.asFunc());
                if (L == luaFunc->L)
                {
                    lua_rawgeti((lua_State *) luaFunc->L, LUA_REGISTRYINDEX, luaFunc->ref);
                }
                else
                {
                    void * block = lua_newuserdata(L, sizeof(Value));
                    new(reinterpret_cast<Value *>(block)) Value(value);
                    luaL_setmetatable(L, METANAME);
                }
            }
            else
            {
                void * block = lua_newuserdata(L, sizeof(Value));
                new(reinterpret_cast<Value *>(block)) Value(value);
                luaL_setmetatable(L, METANAME);
            }
        }
        break;
    }
}
}