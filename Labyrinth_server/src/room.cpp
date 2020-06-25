#include "room.h"
#include <iterator>

Room::Room(unsigned short const& p_count)
    : player_idx(0)
    , active(false)
    , players_count(0) {
        map = new gameMap(MAP_WIDTH, MAP_HEIGHT, p_count);
        add_player();
}

Room::Room(Room&& input)
    : player_idx(input.getCurrentPlayerIndex())
    , active(input.is_active())
    , players_count(input.getPlayersCount())
    , players_list(input.getPlayersList()) {
        delete map;
        map = input.getMap();
        input.map = nullptr;
}

Room::Room(Room const& input)
    : player_idx(input.getCurrentPlayerIndex())
    , active(input.is_active())
    , players_count(input.getPlayersCount())
    , players_list(input.getPlayersList()) {
        gameMap* map_info = input.getMap();
        map = new gameMap(*map_info);
}

Room::~Room() {
    if (map) {
        delete map;
        map = nullptr;
    }
}

std::forward_list<player_ID> Room::getPlayersList() const {
    return players_list;
}

unsigned short Room::getPlayersCount() const {
    return players_count;
}

unsigned short Room::getMaxPlayersCount() const {
    return map->getPlayersCount();
}

short unsigned Room::add_player() {
    if (!is_active() && getPlayersCount() < getMaxPlayersCount()) {
        short unsigned next_idx = 0;
        if (!players_list.empty()) {
            auto last = players_list.begin();
            next_idx = last->idx + 1;
        }
        players_list.emplace_front(next_idx, next_idx);
        players_count++;
        return next_idx;
    }
    return 0;
}

bool Room::remove_player(unsigned short const& idx) {
    bool removed = false;
    players_list.remove_if([&](player_ID ID) {
        if (ID.idx == idx) {
            removed = true;
            return true;
        } else {
            return false;
        }
    });
    if (removed) {
        players_count--;
    }
    return removed;
}

bool Room::is_active() const {
    return active;
}

void Room::activate() {
    active = true;
    players_list.sort();
    short unsigned num = 0;
    for (auto & info : players_list) {
        player_idx = num == 0 ? info.idx : player_idx;
        info.map_id = num++;
    }
}

void Room::update() {
    if (!is_active() && getPlayersCount() == getMaxPlayersCount()) {
        activate();
        std::cout << "Game at room has been started" << std::endl;
    }
    bool need_change_idx = false;
    Player active_player = getPlayerByID(player_idx);
    if (active_player.get_health() < 1) {
        for (auto & info : players_list) {
            if (need_change_idx) {
                player_idx = info.idx;
                break;
            }
            if (info.idx == player_idx) {
                need_change_idx = true;
            }
        }
    }
}

Player Room::getPlayerByID(unsigned short const& idx) const {
    for (auto & info : players_list) {
        if (info.idx == idx) {
            return map->getPlayerByIndex(info.map_id);
        }
    }
    std::cout << "Can not get player with ID: " << idx << " Player is not exists." << std::endl;
    return Player();
}

unsigned short Room::getCurrentPlayerIndex() const {
    return player_idx;
}

turn_info Room::makeTurn(unsigned short const& idx, unsigned short const& action_type, unsigned short const& dir) {
    bool need_change_player_idx = false;
    for (auto & info : players_list) {
        if (need_change_player_idx) {
            player_idx = info.idx;
            return turn_info();
        }
        if (info.idx == idx) {
            // just pass turn
            if (action_type == 0) {
                need_change_player_idx = true;
            } else {
                turn_info res = map->changeWorld(info.map_id, action_type, dir);
                // convert map_player_id to player_idx
                if (action_type == 1) {
                    for (auto & info : players_list) {
                        if (info.map_id == res.key_1) {
                            res.key_1 = info.idx;
                            break;
                        }
                    }
                }
                return res;
            }
        }
    }
    if (need_change_player_idx) {
        auto it = players_list.begin();
        player_idx = it->idx;
    } else {
        std::cout << "Can not make turn for player with ID: " << idx << " Player is not exists." << std::endl;
    }
    return turn_info();
}

gameMap* Room::getMap() const {
    return map;
}