#pragma once

#include <lua.hpp>
#include "../core/value.hpp"
#include "../interface/interface.hpp"

namespace omlit
{
    bool init_lua_interop(lua_State * L);
    bool register_lua_library(lua_State * L, IModuleRegistry * module);
    Value lua_value_to_omlit(lua_State * L, int inx);
    void omlit_value_to_lua(lua_State * L, const Value & value);
}