#pragma once
#include "game_map.h"
#include <forward_list>
#include <array>

struct player_ID {
    player_ID(): idx(0), map_id(0) {}
    player_ID(unsigned short idx, unsigned short map_id) : idx(idx), map_id(map_id) {};

    bool operator<(player_ID const& ID) const {
        return idx < ID.idx;
    }

    bool operator==(player_ID const& ID) const {
        return idx == ID.idx;
    };

    bool operator!=(player_ID const& ID) const {
        return !(*this == ID);
    };

    unsigned short idx;
    unsigned short map_id;
};

unsigned short const MAP_WIDTH = 30;
unsigned short const MAP_HEIGHT = 30;

struct Room {
        Room(unsigned short const& p_count);
        Room(Room const& input);
        Room(Room&& input);
        ~Room();

        turn_info makeTurn(unsigned short const& idx, unsigned short const& action_type, unsigned short const& dir);

        unsigned short getCurrentPlayerIndex() const;
        Player getPlayerByID(unsigned short const& idx) const;

        unsigned short getPlayersCount() const;
        unsigned short getMaxPlayersCount() const;
        std::forward_list<player_ID> getPlayersList() const;
        short unsigned add_player();
        bool remove_player(unsigned short const& idx);

        bool is_active() const;
        void activate();
        void update();

        gameMap* getMap() const;
    private:
        Room& operator=(Room const& input) = delete;
        Room& operator=(Room&& input) = delete;

        unsigned short player_idx;
        bool active;
        unsigned short players_count;
        std::forward_list<player_ID> players_list;
        gameMap* map;
};