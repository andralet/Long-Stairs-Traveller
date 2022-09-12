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
    
    const int MAX_PROBABILITY = 1000000,
              TROUBLE_CHANCE = MAX_PROBABILITY / 2,
              BASIC_LUCK = MAX_PROBABILITY / 2,
              LUCK_BUFF = MAX_PROBABILITY / 100 * 30,
              LUCK_ON_EVEN = LUCK_BUFF / 3,
              LUCK_DEBUFF = LUCK_BUFF / 3,
              LUCK_STABILISER = LUCK_BUFF / 3;

    // loc_data
    #include "loc_data.hpp"

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