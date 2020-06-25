#pragma once
#include <conio.h>
#include <ctime>
#include <map>
#include <iostream>
#include "player.h"

coord const LEFT = {-1, 0};
coord const RIGHT = {1, 0};
coord const TOP = {0, -1};
coord const BOTTOM = {0, 1};

unsigned short const OBJECTS_MAX = 5;
enum {NONE, ARSENAL, DRUGSTORE, TELEPORT};
enum {NOTHING, MOVE, SHOOT, BOMB};

static coord id_to_dir(unsigned short const& id) {
    coord dir = BOTTOM;
    if (id == 0) {
        dir = LEFT;
    } else if (id == 1) {
        dir = TOP;
    } else if (id == 2) {
        dir = RIGHT;
    }
    return dir;
}

struct borders {
    borders() : left(0), top(0) {};
    short left;
    short top;
};

struct turn_info {
    turn_info(): status(false), id(0), key_1(0) {}
    
    turn_info(bool status, unsigned short const& id, unsigned short const& key_1)
        : status(status)
        , id(id)
        , key_1(key_1) 
    {}
    
    turn_info(turn_info const& input)
        : status(input.status)
        , id(input.id)
        , key_1(input.key_1) 
    {}
    
    turn_info(turn_info const&& input)
        : status(input.status)
        , id(input.id)
        , key_1(input.key_1)
    {}

    turn_info& operator=(turn_info const& input) {
        status = input.status;
        id = input.id;
        key_1 = input.key_1;
        return *this;
    }

    bool status;
    unsigned short id;
    unsigned short key_1;
};

struct gameMap {
        gameMap(short const& w, short const& h, short unsigned const& players_num);
        gameMap(gameMap const& input);
        ~gameMap();

        bool is_player_in_cell(coord const& cell);

        static short get_random(short const& value) {
            srand(time(NULL) * (rand() % 100) * (rand() % 100));
            return rand() % value;
        }

        short unsigned getWidth() const;
        short unsigned getHeight() const;
        short unsigned getPlayersCount() const;
        coord* getChestCoord() const;
        std::map<coord, unsigned short> getObjects() const;
        Player* getPlayers() const;
        borders** getMap() const;

        Player& getPlayerByIndex(unsigned short const& idx) const;

        turn_info changeWorld(unsigned short const& player_id, unsigned short const& action_type, unsigned short const& dir_id);
        turn_info can_move_player(coord const& from, coord const& dir);
        void move_player(Player& plr, coord const& dir);
        Player& getPlayer(coord const& cell) const;
        turn_info shoot(coord start, coord const& dir);
        turn_info put_bomb(Player& plr, coord const& dir);

        void add_chest_to_player(Player& plr);
        bool is_chest_in_cell(coord const& pos) const;
        void put_chest_to_map(coord const& pos);

        unsigned short get_object_type(coord const& pos) const;
        void activate_object(Player& plr, unsigned short type);
    private:
        gameMap& operator=(gameMap const& input) = delete;
        gameMap(gameMap&& input) = delete;
        gameMap& operator=(gameMap&& input) = delete;

        short unsigned width;
        short unsigned height;
        short unsigned players_count;
        coord* chest;
        std::map<coord, unsigned short> objects;
        Player* persons;
        borders** map;
};