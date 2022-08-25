#ifndef LOC_DATA_HPP
    #define LOC_DATA_HPP

    #include <vector>
    #include <set>
    #include <map>

    struct Door {
        std::map<int, int> chances;
        int up, same, down;
    };

    #include "colors.hpp"
    extern const ColorMap DEFAULT_COLORS;

    struct MapSettings {
        int width, height;
        int ground, wall;
        int door_num;
        int plant_chance, stone_chance, crate_chance;
        ColorMap color;
    };

    /*const struct Location DEFAULT_LOC = {
        "none", 10, 50, 10, 0, 10, 0, 10, 3, 9, -1, 1, "none", "none"
    };*/

    struct Location {
        char name[256];
        int size_min, size_max; // in 5ft squares
        int door_max;
        int plants_min, plants_max, fluid_min, fluid_max;
        int stones_min, stones_max;
        int enemy_min, enemy_max, power_min, power_max;
        char common_fluid[128];
        char common_enemy[128];
        MapSettings map_data;
    };

    struct ConcreteLocation {
        int loc_id;
        int x, y, z; // in 5ft squares
        int door_num;
        int plants, fluid;
        int stones;
        int enemy;
        double power;
        std::vector<Door> doors;
        std::set<int> troubles;
    };

    // walls, grounds and pools
    enum {SQUARE_WALLS = 0, CAVERN_WALLS = 1, DIAGONAL_WALLS = 2};
    enum {COBBLESTONE = 0, GRASS = 1, STONE_BRICK = 2, WOOD_BOARDS = 3, IRON_FLOOR = 4, DIRT_ROAD = 5, SWAMP = 6, SAND = 7, CHESS = 8, GROUND = 9, SOLID_STONE = 10};
    enum {WATER = 0, LAVA = 1, SWAMP_WATER = 2};

    // objects
    enum {HEAVY_FURNITURE = 13, LOW_FURNITURE = 0, SMALL_FURNITURE = 11, CRATES = 1, DOOR = 2, WINDOW = 10, FENCE = 3,
            HEAVY_ROCKS = 14, SMALL_ROCKS = 4, TREE = 5, BUSH = 15};
    enum {LADDER_LEFT = 6, LADDER_RIGHT = 7, LADDER_TOP = 8, LADDER_BOTTOM = 10};

    const unsigned LOC_NUM = 5;
    const struct Location LOC[LOC_NUM] = {
        {"Пустыня", 10, 50, 10, 0, 4, 0, 2, 0, 8, 0, 6, -1, 1, "Лава или вода", "Дракониды", {5, 5, SAND, SQUARE_WALLS, 0, 0, 0, 0,
            DEFAULT_COLORS}},
        {"Лес", 30, 50, 7, 6, 10, 2, 7, 0, 5, 0, 10, -1, 1, "Вода", "Звери", {5, 5, GRASS, CAVERN_WALLS, 0, 0, 0, 0,
            DEFAULT_COLORS}},
        {"Болото", 10, 50, 3, 1, 4, 6, 9, 0, 3, 0, 9, -1, 2, "Вода", "Монстры или нежить", {5, 5, SWAMP, SQUARE_WALLS, 0, 0, 0, 0,
            DEFAULT_COLORS}},
        {"Каменная комната", 10, 40, 5, 0, 1, 10, 0, 0, 0, 0, 7, -1, 1, "Нет", "Кто угодно", {5, 5, SOLID_STONE, SQUARE_WALLS, 0, 0, 0, 0,
            DEFAULT_COLORS}},
        {"Стальная комната", 10, 40, 5, 0, 0, 6, 0, 0, 0, 2, 10, 0, 1, "Нет", "Кто угодно", {5, 5, IRON_FLOOR, SQUARE_WALLS, 0, 0, 0, 0,
            DEFAULT_COLORS}}
    };

    const unsigned LANDING_DIST = 3,
                   LANDING_NUM = 4;
    const struct ConcreteLocation LANDING[LANDING_NUM] = {
        {-1, 10, 10, 50, 4, 0, 0, 0, 0, 0, {}, {}},
        {-1, 80, 80, 60, 10, 0, 0, 0, 7, 1, {}, {}},
        {-1, 100, 100, 60, 10, 2, 1, 0, 3, 0, {}, {}},
        {-1, 50, 50, 30, 1, 0, 0, 0, 8, 2, {}, {}}
    };

    const unsigned TROUBLE_NUM = 39;
    
#endif