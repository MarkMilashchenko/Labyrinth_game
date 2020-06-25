#pragma once
#include <cmath>

short const MAX_HEALTH = 5;
short const MAX_AMMO = 6;
short const MAX_BOMB = 10;
short const AMMO_DAMAGE = 2;

struct coord {
    coord() : x(0), y(0) {};
    coord(short x, short y) : x(x), y(y) {};

    coord operator+(coord const& dir) const {
        coord res(0 ,0);
        res.x = x + dir.x;
        res.y = y + dir.y;
        return res;
    };

    bool operator<(coord const& dir) const {
        return sqrt(x * x + y * y) < sqrt(dir.x * dir.x + dir.y * dir.y);
    }

    bool operator==(coord const& dir) const {
        return (dir.x == x && dir.y == y);
    };

    bool operator!=(coord const& dir) const {
        return !(*this == dir);
    };

    short x;
    short y;
};

struct Player {
        Player();

        void set_coord(coord const &pos);
        coord get_coord() const;

        void add_health(short const &value);
        short const& get_health() const;

        void add_ammo(short const &value);
        short const& get_ammo() const;

        void add_bomb(short const &value);
        short const& get_bomb() const;

        void add_chest();
        bool const& has_chest() const;

    private:
        coord position;
        short health;
        short ammo;
        short bomb;
        bool isChest;
};