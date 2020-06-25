#include "server.h"

Server::Server() {}

unsigned short Server::createRoom(unsigned short const& player_num) {
    unsigned short idx = 0;
    auto it = rooms.find(idx);
    while (it != rooms.end()) {
        it = rooms.find(++idx);
    }
    rooms.emplace(idx, player_num);
    std::cout << "Create new room with ID: " << idx << std::endl;
    return idx;
}

unsigned short Server::addPlayerToRoom(unsigned short const& room_id) {
    auto it = rooms.find(room_id);
    if (it != rooms.end()) {
        Room& room = it->second;
        std::cout << "Add player to room with ID: " << room_id << std::endl;
        return room.add_player();
    } else {
        std::cout << "Can not add player to room with ID: " << room_id << std::endl;
        return 0;
    }
}

bool Server::removePlayerFromRoom(unsigned short const& room_id, unsigned short const& player_idx) {
    auto it = rooms.find(room_id);
    if (it != rooms.end()) {
        Room& room = it->second;
        std::cout << "Remove player with ID: " << player_idx <<  " from room with ID: " << room_id << std::endl;
        return room.remove_player(player_idx);
    } else {
        std::cout << "Can not remove player with ID: " << player_idx <<  " from room with ID: " << room_id << std::endl;
        return false;
    }
}

void Server::Update() {
    std::vector<unsigned short> need_remove;
    for (auto it = rooms.rbegin(); it != rooms.rend(); it++) {
        Room& room = it->second;
        room.update();
        if (room.getPlayersCount() == 0) {
            need_remove.push_back(it->first);
        }
    }
    for (auto & key : need_remove) {
        std::cout << "Remove room with ID: " << key << std::endl;
        rooms.erase(key);
    }
}

bool Server::isRoomActive(unsigned short const& room_id) const {
    auto it = rooms.find(room_id);
    if (it != rooms.end()) {
        Room const& room = it->second;
        return room.is_active();
    } else {
        std::cout << "Can not check status for room with ID: " << room_id << " Room doesn't exist." << std::endl;
        return false;
    }
}

Player Server::getPlayerFromRoom(unsigned short const& room_id, unsigned short const& player_idx) const {
    auto it = rooms.find(room_id);
    if (it != rooms.end()) {
        Room const& room = it->second;
        return room.getPlayerByID(player_idx);
    } else {
        std::cout << "Can not get player for room with ID: " << room_id << " Room doesn't exist." << std::endl;
        return Player();
    }
}
        
unsigned short Server::getActivePlayerID(unsigned short const& room_id) const {
    auto it = rooms.find(room_id);
    if (it != rooms.end()) {
        Room const& room = it->second;
        return room.getCurrentPlayerIndex();
    } else {
        std::cout << "Can not get active player ID for room with ID: " << room_id << " Room doesn't exist." << std::endl;
        return 0;
    }
}

turn_info Server::makeTurn(unsigned short const& room_id, unsigned short const& player_idx, unsigned short const& action_type, unsigned short const& dir) {
    auto it = rooms.find(room_id);
    if (it != rooms.end()) {
        Room& room = it->second;
        return room.makeTurn(player_idx, action_type, dir);
    } else {
        std::cout << "Can not make turn for player with ID " << player_idx <<  " in room with ID: " << room_id << " Room doesn't exist." << std::endl;
        return turn_info();
    }
}