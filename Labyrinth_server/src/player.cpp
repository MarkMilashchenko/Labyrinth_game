#include "player.h"

Player::Player() 
    : position(coord{0, 0})
    , health(MAX_HEALTH)
    , ammo(MAX_AMMO)
    , bomb(MAX_BOMB)
    , isChest(false) {
}

void Player::set_coord(coord const& pos) {
    position = pos;
}

coord Player::get_coord() const {
    return position;
}

void Player::add_health(short const &value) {
    health += value;
    if (health < 0)
        health = 0;
    if (health > MAX_HEALTH)
        health = MAX_HEALTH;
}
        
short const& Player::get_health() const {
    return health;
}

void Player::add_ammo(short const &value) {
    ammo += value;
    if (ammo > MAX_AMMO)
        ammo = MAX_AMMO;
}

short const& Player::get_ammo() const {
    return ammo;
}

void Player::add_bomb(short const &value) {
    bomb += value;
    if (bomb > MAX_BOMB)
        bomb = MAX_BOMB;
}

short const& Player::get_bomb() const {
    return bomb;
}

void Player::add_chest() {
    isChest = true;
}

bool const & Player::has_chest() const {
    return isChest;
}