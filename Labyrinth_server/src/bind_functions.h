#pragma once
#include "lua_state_singleton.h"
#include "server.h"

static int startGame(lua_State *L)
{
    Server& server_inst = Server::instance();

    if (!lua_isnumber(L, 1)) {
        lua_pushstring(L, "Core: startGame: incorrect argument");
        lua_error(L);
        return 0;
    } else {
        short unsigned player_num = lua_tonumber(L, 1);
        lua_Number room_id = server_inst.createRoom(player_num);
        lua_pushnumber(L, room_id);
        return 1;
    }
}

static int connectToGame(lua_State *L)
{
    Server& server_inst = Server::instance();

    if (!lua_isnumber(L, 1)) {
        lua_pushstring(L, "Core: connectToGame: incorrect argument");
        lua_error(L);
        return 0;
    } else {
        short unsigned room_id = lua_tonumber(L, 1);
        lua_Number player_id = server_inst.addPlayerToRoom(room_id);
        lua_pushnumber(L, player_id);
        return 1;
    }
}

static int leaveGame(lua_State *L)
{
    Server& server_inst = Server::instance();

    if (!lua_isnumber(L, 1) || !lua_isnumber(L, 2)) {
        lua_pushstring(L, "Core: leaveGame: incorrect argument");
        lua_error(L);
        return 0;
    } else {
        short unsigned room_id = lua_tonumber(L, 1);
        short unsigned player_idx = lua_tonumber(L, 2);
        lua_pushboolean(L, server_inst.removePlayerFromRoom(room_id, player_idx));
        return 1;
    }
}

static int Update(lua_State *L)
{
    Server& server_inst = Server::instance();
    server_inst.Update();
    return 0;
}

static int gameActive(lua_State *L)
{
    Server& server_inst = Server::instance();

    if (!lua_isnumber(L, 1)) {
        lua_pushstring(L, "Core: gameActive: incorrect argument");
        lua_error(L);
        return 0;
    } else {
        short unsigned room_id = lua_tonumber(L, 1);
        lua_pushboolean(L, server_inst.isRoomActive(room_id));
        return 1;
    }
}

static int getPlayerInfo(lua_State *L)
{
    Server& server_inst = Server::instance();

    if (!lua_isnumber(L, 1) || !lua_isnumber(L, 2)) {
        lua_pushstring(L, "Core: getPlayerInfo: incorrect argument");
        lua_error(L);
        return 0;
    } else {
        short unsigned room_id = lua_tonumber(L, 1);
        short unsigned player_idx = lua_tonumber(L, 2);
        Player player_info = server_inst.getPlayerFromRoom(room_id, player_idx);
        lua_Number active_player_id = server_inst.getActivePlayerID(room_id);
        
        lua_pushnumber(L, player_info.get_health());
        lua_pushnumber(L, player_info.get_ammo());
        lua_pushnumber(L, player_info.get_bomb());
        lua_pushboolean(L, player_info.has_chest());
        lua_pushnumber(L, active_player_id);
        return 5;
    }
}

static int makeTurn(lua_State *L)
{
    Server& server_inst = Server::instance();

    if (!lua_isnumber(L, 1) || !lua_isnumber(L, 2) || !lua_isnumber(L, 3) || !lua_isnumber(L, 4)) {
        lua_pushstring(L, "Core: makeTurn: incorrect argument");
        lua_error(L);
        return 0;
    } else {
        short unsigned room_id = lua_tonumber(L, 1);
        short unsigned player_idx = lua_tonumber(L, 2);
        short unsigned action_type = lua_tonumber(L, 3);
        short unsigned dir = lua_tonumber(L, 4);
        turn_info info = server_inst.makeTurn(room_id, player_idx, action_type, dir);

        lua_pushboolean(L, info.status);
        lua_pushnumber(L, info.id);
        lua_pushnumber(L, info.key_1);
        return 3;
    }
}

void bind_functions();