#include <iostream>
#include "bind_functions.h"

int main(int argc, char** argv) {
    luaStateSingleton& LUA = luaStateSingleton::instance();
    bind_functions();

    LUA.run_lua_script("server.lua");
    system("PAUSE");
    return 0; 
}
