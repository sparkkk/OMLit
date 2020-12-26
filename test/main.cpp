#include <stdio.h>

#include <lua.hpp>
#include <omlit/omlit.hpp>
#include <omlit/lua/binding.hpp>

const char * luaCode = R"(
    local calc = require "demo".Calc(1)
    print(calc.Value)
    calc:Add(3)
    print(calc.Value)
    calc:Mul(3)
    print(calc.Value)
    calc:Sub(3)
    print(calc.Value)
    calc:Div(3)
    print(calc.Value)
    calc:Mod(3)
    print(calc.Value)
)";


int main(int argc, const char * argv[])
{

    std::string msg1;
    auto manager = omlit::IModuleManager::Create();
    if (!manager->LoadLibrary(
        omlit::IModuleManager::IMAGE_PATH, 
        msg1))
    {
        printf("ModuleManager::LoadLibrary failed: %s\n", msg1.c_str());
        return 0;
    }
    omlit::IModuleRegistry * module = nullptr;
    if (!manager->QueryModule(
        { "demo", omlit::ModuleVersion::LATEST },
        module))
    {
        printf("ModuleManager::QueryModule failed\n");
        return 0;
    }

    lua_State * L = luaL_newstate();
    luaL_openlibs(L);
    
    omlit::init_lua_interop(L);
    omlit::register_lua_library(L, module);

    luaL_loadstring(L, luaCode);
    if (lua_pcall(L, 0, 0, 0) != LUA_OK)
    {
        printf("error %s\n", lua_tostring(L, -1));
    }

    lua_close(L);
    L = nullptr;
    return 0;
}