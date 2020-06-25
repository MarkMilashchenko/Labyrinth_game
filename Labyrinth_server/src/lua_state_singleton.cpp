#include "lua_state_singleton.h"

luaStateSingleton::luaStateSingleton() {
    L = luaL_newstate();
    luaL_openlibs(L);

    static luaL_Reg const lualibs[] = {
        {"base", luaopen_base},
        {"io", luaopen_io},
        {NULL, NULL}
    };

    for (luaL_Reg const* lib = lualibs; lib->func != NULL; lib++) {
        luaL_requiref(L, lib->name, lib->func, 1);
        lua_settop(L, 0);
    }
}

luaStateSingleton::~luaStateSingleton() {
    lua_close(L);
}

void luaStateSingleton::register_fcn(char const* name, lua_CFunction fcn) {
    lua_register(L, name, fcn);
}

void luaStateSingleton::run_lua_script(char const* name) {
    int ret = luaL_dofile(L, name);
    if (ret != 0) {
        std::cout << "Error occurs when calling luaL_dofile() Hint Machine " << ret << std::endl;
        std::cout << "Error: " << lua_tostring(L, -1) << std::endl;
    }
}