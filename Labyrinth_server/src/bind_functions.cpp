#include "bind_functions.h"

void bind_functions() {
    luaStateSingleton &LUA = luaStateSingleton::instance();
    // binded functions
    LUA.register_fcn("startGame", startGame);
    LUA.register_fcn("connectToGame", connectToGame);
    LUA.register_fcn("leaveGame", leaveGame);
    LUA.register_fcn("cpp_update", Update);
    LUA.register_fcn("isGameStarted", gameActive);
    LUA.register_fcn("getPlayerInfo", getPlayerInfo);
    LUA.register_fcn("makeTurn", makeTurn);
}