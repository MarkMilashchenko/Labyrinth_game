#pragma once
#include <iostream>

extern "C"
{
    #include "lua.h"
    #include "lualib.h"
    #include "lauxlib.h"
}

#include "compat-5.3.h"

class luaStateSingleton {
        luaStateSingleton();
        
        luaStateSingleton(luaStateSingleton const& input) = delete;
        
        luaStateSingleton& operator=(luaStateSingleton const& input) = delete;
        
        luaStateSingleton(luaStateSingleton const&& input) = delete;
        
        luaStateSingleton& operator=(luaStateSingleton&& input) = delete;
        
        ~luaStateSingleton();
        
        lua_State* L;
    public:
        static luaStateSingleton& instance() {
            static luaStateSingleton instance;
            return instance;
        };

        void register_fcn(char const *name, lua_CFunction fcn);

        void run_lua_script(char const* name);
};