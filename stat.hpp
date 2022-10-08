#ifndef STAT_HPP
    #define STAT_HPP

    #include <stdio.h>
    #include <stdlib.h>
    #include <string.h>
    #include <time.h>

    #include <vector>
    #include <set>
    #include <map>
    #include <algorithm>

    // 0 - NONE, 10 - EVERYWHERE
    const int RARE_LIMIT = 3,
              NORMAL_LIMIT = 6,
              OFTEN_LIMIT = 9;

    const int MAX_SKULLS = 5;

    #define PERCENT(x) ( (x) * MAX_PROBABILITY / 100 )

    const int MAX_PROBABILITY = 1000000,
              TROUBLE_CHANCE = PERCENT(50),
              BASIC_LUCK = PERCENT(50),
              LUCK_BUFF = PERCENT(40),
              LUCK_ON_EVEN = LUCK_BUFF / 2,
              LUCK_DEBUFF = LUCK_BUFF / 4,
              LUCK_STABILISER = LUCK_BUFF / 4,
              
              MAGIC_LOOT_CHANCE = PERCENT(10),
              CURSED_MAGIC_CHANCE = PERCENT(40),
              HEAVY_LOOT_CHANCE = PERCENT(40),
              VERY_HEAVY_LOOT_CHANCE = PERCENT(10),
              MONEY_LOOT_LIMIT = 5,
              MONEY_TOP_LIMIT = 30,
              MAGIC_LOOT_LIMIT = 20;

    // loc_data
    #include "loc_data.hpp"

    // names
    extern const char *LANDING_NAME[LANDING_NUM];
    const int LUCK_LEVEL_NUM = 7;
    extern const char *LUCK_LEVELS[LUCK_LEVEL_NUM];
    const int TREASURE_NUM = 7;
    extern const char *TREASURE[TREASURE_NUM];
    const int TRINKET_NUM = 545;
    extern const char *TRINKET[TRINKET_NUM];

    const unsigned TROUBLE_NUM = 252;
    extern const char *TROUBLES[TROUBLE_NUM];
    const unsigned FOCUS_NUM = 14;
    extern const char *FOCUSES[FOCUS_NUM];

    // loc
    int gen_range(int min, int max);
    const char *name_count(int count);
    struct ConcreteLocation make_loc(int loc_id);
    void print_loc(ConcreteLocation l, int map_quality, int level);
    struct Door create_door(unsigned loc_num, int landing_id, int up_buff);
    void gen_doors(struct ConcreteLocation &l, unsigned loc_num, int level, int goal, int luck);
    void gen_troubles(struct ConcreteLocation &l);
    int use_door(const struct ConcreteLocation &l, unsigned door_id, int &level, int &goal, int &luck);

    // map
    void make_map(FILE *out, MapSettings data);

    // save
    void save(const ConcreteLocation &l, int level, int map_quality, int picture_id, int goal, int luck);
    void load(ConcreteLocation &l, int &level, int &map_quality, int &picture_id, int &goal, int &luck);

#endif