#pragma once
#include "room.h"
#include <map>
#include <vector>

class Server {
        Server();
        
        Server(Server const& input) = delete;
        Server& operator=(Server const& input) = delete;
        Server(Server const&& input) = delete;
        Server& operator=(Server&& input) = delete;

        std::map<unsigned short, Room> rooms;
    public:
        unsigned short createRoom(unsigned short const& player_num);
        unsigned short addPlayerToRoom(unsigned short const& room_id);
        bool removePlayerFromRoom(unsigned short const& room_id, unsigned short const& player_idx);
        
        void Update();

        bool isRoomActive(unsigned short const& room_id) const;

        Player getPlayerFromRoom(unsigned short const& room_id, unsigned short const& player_idx) const;
        unsigned short getActivePlayerID(unsigned short const& room_id) const;

        turn_info makeTurn(unsigned short const& room_id, unsigned short const& player_idx, unsigned short const& action_type, unsigned short const& dir);

        static Server& instance() {
            static Server instance;
            return instance;
        };
};