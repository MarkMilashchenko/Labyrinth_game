#include "game_map.h"

gameMap::gameMap(short const& w, short const& h, short unsigned const& players_num)
    : width(w)
    , height(h)
    , players_count(players_num) 
    {
        // GENERATE_MAP
        map = new borders*[width];
        for (size_t i = 0; i < width; i++) {
            map[i] = new borders[height];
            map[i][0].top = 1;
        }
        for (size_t i = 0; i < height; i++)
            map[0][i].left = 1;
        for (size_t i = 0; i < width; i++) {
            for (size_t j = 0; j < height; j++) {
                if (map[i][j].left == 0)
                    map[i][j].left = gameMap::get_random(2);
                if (map[i][j].top == 0)
                    map[i][j].top = gameMap::get_random(2);
            }
        }
        // CREATE_PLAYERS
        persons = new Player[players_num];
        for (size_t i = 0; i < players_num; i++) {
            coord* new_player_coord = new coord(gameMap::get_random(width), gameMap::get_random(height));
            while (is_player_in_cell(*new_player_coord)) {
                new_player_coord->x = gameMap::get_random(width);
                new_player_coord->y = gameMap::get_random(height);
            }
            persons[i].set_coord(*new_player_coord);
        }
        // ADD CHEST
        chest = new coord(gameMap::get_random(width), gameMap::get_random(height));
        while (is_player_in_cell(*chest)) {
            chest->x = gameMap::get_random(width);
            chest->y = gameMap::get_random(height);
        }
        // CREATE OBJECTS
        for (size_t i = 0; i < OBJECTS_MAX; i++) {
            coord pos(gameMap::get_random(width), gameMap::get_random(height));
            while (objects.find(pos) != objects.end()) {
                pos.x = gameMap::get_random(width);
                pos.y = gameMap::get_random(height);
            }
            unsigned short type = gameMap::get_random(3);
            if (type == TELEPORT) {
                objects[pos] = type;
                while (objects.find(pos) != objects.end()) {
                    pos.x = gameMap::get_random(width);
                    pos.y = gameMap::get_random(height);
                }
            }
            objects[pos] = type;
        }
}

short unsigned gameMap::getWidth() const {
    return width;
}

short unsigned gameMap::getHeight() const {
    return height;
}

short unsigned gameMap::getPlayersCount() const {
    return players_count;
}

coord* gameMap::getChestCoord() const {
    return chest;
}

std::map<coord, unsigned short> gameMap::getObjects() const {
    return objects;
}

Player* gameMap::getPlayers() const {
    return persons;
};

borders** gameMap::getMap() const {
    return map;
}

gameMap::gameMap(gameMap const& input)
    : width(input.getWidth())
    , height(input.getHeight())
    , players_count(input.getPlayersCount())
    {
        coord* chest_info = input.getChestCoord();
        chest = new coord(chest_info -> x, chest_info -> y);

        objects = input.getObjects();

        persons = new Player[players_count];
        Player* player_info = input.getPlayers();
        for (size_t i = 0; i < players_count; i++) {
            persons[i] = player_info[i];
        }

        borders** map_info = input.getMap();
        map = new borders*[width];
        for (size_t i = 0; i < width; i++) {
            map[i] = new borders[height];
        }
        for (size_t i = 0; i < width; i++) {
            for (size_t j = 0; j < height; j++) {
                map[i][j] = map_info[i][j];
            }
        }
}

gameMap::~gameMap() {
    for (size_t i = 0; i < width; i++) {
        delete[] map[i];
        map[i] = nullptr;
    }
    delete[] map;
    map = nullptr;
    if (chest) {
        delete chest;
        chest = nullptr;
    }
}

bool gameMap::is_player_in_cell(coord const& cell) {
    for (size_t i = 0; i < players_count; i++) {
        if (persons[i].get_coord() == cell && persons[i].get_health() > 0)
            return true;
    }
    return false;
}

Player& gameMap::getPlayerByIndex(unsigned short const& idx) const {
    return persons[idx];
}

turn_info gameMap::changeWorld(unsigned short const& player_id, unsigned short const& action_type, unsigned short const& dir_id) {
    coord dir = id_to_dir(dir_id);
    Player& plr = getPlayerByIndex(player_id);
    coord from = plr.get_coord();
    switch (action_type) {
        case NOTHING: {
            std::cout << "Action type: NOTHING" << std::endl;
            return turn_info();
        }
        case MOVE: {
            turn_info info = can_move_player(from, dir);
            if (info.status) {
                move_player(plr, dir);
                if (get_object_type(from + dir) > 0) {
                    info.id = 3;
                    info.key_1 = get_object_type(from + dir);
                    activate_object(plr, get_object_type(from + dir));
                }
                if (is_chest_in_cell(from + dir)) {
                    add_chest_to_player(plr);
                    info.id = 2;
                }
            }
            return info;
        }
        case SHOOT: {
            return shoot(from, dir);
        }
        case BOMB: {
            return put_bomb(plr, dir);
        }
        default: {
            std::cout << "Unknown action type: " << action_type << std::endl;
            return turn_info();
        }
    }
}

turn_info gameMap::can_move_player(coord const& from, coord const& dir) {
    coord new_pos = from + dir;
    // check for players in cell
    if (is_player_in_cell(new_pos)) {
        for (size_t i = 0; i < players_count; i++) {
            if (persons[i].get_coord() == new_pos) {
                return turn_info(false, 2, i);
            }
        }
    }
    // check for walls
    bool status = true;
    if (dir == LEFT) {
        status = from.x > 0 && map[from.x][from.y].left == 0;
    } else if (dir == TOP) {
        status = from.y > 0 && map[from.x][from.y].top == 0;
    } else if (dir == RIGHT) {
        status = from.x < (width - 1) && map[from.x + 1][from.y].left == 0;
    } else {
        status = from.y < (height - 1) && map[from.x][from.y + 1].top == 0;
    }
    return turn_info(status, 1, 0);
}

void gameMap::move_player(Player& plr, coord const& dir) {
    coord current = plr.get_coord();
    current = current + dir;
    plr.set_coord(current);
}

Player& gameMap::getPlayer(coord const& cell) const {
    for (size_t i = 0; i < players_count; i++) {
        if (persons[i].get_coord() == cell) {
            return persons[i];
        }
    }
    std::cout << "Can not get player in cell: " << cell.x << " " << cell.y << std::endl;
    return persons[0];
}

turn_info gameMap::shoot(coord start, coord const& dir) {
    Player& active_player = getPlayer(start);
    if (active_player.get_ammo() < 1) {
        return turn_info(false, 2, 0);
    }
    active_player.add_ammo(-1);
    turn_info info = can_move_player(start, dir);
    while (info.status) {
        start = start + dir;
        info = can_move_player(start, dir);
    }
    start = start + dir;
    if (is_player_in_cell(start)) {
        Player& plr = getPlayer(start);
        plr.add_health(-AMMO_DAMAGE);
        if (plr.get_health() < 1) {
            put_chest_to_map(plr.get_coord());
        }
        unsigned short idx;
        for (size_t i = 0; i < players_count; i++) {
            if (persons[i].get_coord() == start) {
                idx = i;
                break;
            }
        }
        return turn_info(true, 1, idx);
    } else {
        return turn_info(false, 1, 0);
    }
}

turn_info gameMap::put_bomb(Player& plr, coord const& dir) {
    if (plr.get_bomb() < 1) {
        return turn_info(false, 3, 0);
    }
    plr.add_bomb(-1);
    coord pos = plr.get_coord();
    if (dir == LEFT) {
        if (pos.x > 0) {
            bool status = map[pos.x][pos.y].left == 1;
            map[pos.x][pos.y].left = 0;
            return turn_info(status, 1, 0);
        }
    } else if (dir == TOP) {
        if (pos.y > 0) {
            bool status = map[pos.x][pos.y].top == 1;
            map[pos.x][pos.y].top = 0;
            return turn_info(status, 1, 0);
        }
    } else if (dir == RIGHT) {
        if (pos.x < (width - 1)) {
            bool status = map[pos.x + 1][pos.y].left == 1;
            map[pos.x + 1][pos.y].left = 0;
            return turn_info(status, 1, 0);
        }
    } else {
        if (pos.y < (height - 1)) {
            bool status = map[pos.x][pos.y + 1].top == 1;
            map[pos.x][pos.y + 1].top = 0;
            return turn_info(status, 1, 0);
        }
    }
    return turn_info(false, 2, 0);
}

void gameMap::add_chest_to_player(Player& plr) {
    plr.add_chest();
    delete chest;
    chest = nullptr;
}

bool gameMap::is_chest_in_cell(coord const& pos) const {
    if (chest != nullptr)
        return *chest == pos;
    else
        return false;
}

void gameMap::put_chest_to_map(coord const& pos) {
    chest = new coord(pos.x, pos.y);
}

unsigned short gameMap::get_object_type(coord const& pos) const {
    auto obj = objects.find(pos);
    if (objects.end() != obj) {
        return obj->second;
    } else {
        return 0;
    }
}

void gameMap::activate_object(Player& plr, unsigned short type) {
    switch(type) {
        case ARSENAL: {
            plr.add_ammo(MAX_AMMO);
            plr.add_bomb(MAX_BOMB);
            break;
        }
        case DRUGSTORE: {
            plr.add_health(MAX_HEALTH);
            break;
        }
        case TELEPORT: {
            coord current = plr.get_coord();
            for (auto it : objects) {
                if (it.second == TELEPORT and it.first != current) {
                    plr.set_coord(it.first);
                    break;
                }
            }
            break;
        }
        default: {
            break;
        }
    }
}