#include "stat.hpp"

int gen_range(int min, int max) {
    return rand() % (max - min + 1) + min;
}

const char *name_count(int count) {
    if (count == 0) return "нет";
    else if (count < RARE_LIMIT) return "мало";
    else if (count < NORMAL_LIMIT) return "нормально";
    else if (count < OFTEN_LIMIT) return "много";
    else return "повсюду";
}

struct ConcreteLocation make_loc(int loc_id) {
    struct ConcreteLocation res = {};
    res.loc_id = loc_id;
    res.x = gen_range(LOC[loc_id].size_min, LOC[loc_id].size_max);
    res.y = gen_range(LOC[loc_id].size_min, LOC[loc_id].size_max);
    res.z = rand() % 10 + 1;
    res.door_num = gen_range(2, LOC[loc_id].door_max);
    res.plants = gen_range(LOC[loc_id].plants_min, LOC[loc_id].plants_max);
    res.fluid = gen_range(LOC[loc_id].fluid_min, LOC[loc_id].fluid_max);
    res.stones = gen_range(LOC[loc_id].stones_min, LOC[loc_id].stones_max);
    res.enemy = gen_range(LOC[loc_id].enemy_min, LOC[loc_id].enemy_max);
    res.power = gen_range(LOC[loc_id].power_min, LOC[loc_id].power_max);
    res.focus[0] = rand() % FOCUS_NUM;
    do {
        res.focus[1] = rand() % FOCUS_NUM;
    } while (res.focus[1] == res.focus[0]);
    return res;
}

int get_probability(int chance, int map_quality) {
    int prob = int(100.0 * chance / MAX_PROBABILITY);
    if (prob % map_quality < map_quality / 2 + map_quality % 2) prob -= prob % map_quality;
    else prob = prob - prob % map_quality + map_quality;
    return prob;
}

void print_loc(ConcreteLocation l, int map_quality, int level) {
    if (l.loc_id >= 0) {
        printf("%s ", LOC[l.loc_id].name);
    } else {
        printf("Специальная точка ");
    }
	printf("%dx%dx%d клеток (%dx%dx%d футов):\n", l.x, l.y, l.z, l.x * 5, l.y * 5, l.z * 5);
    printf("Растительность: %d<%s>\t", l.plants, name_count(l.plants));
    printf("Жидкость: %d<%s>\t", l.fluid, name_count(l.fluid));
    printf("Камни: %d<%s>\n", l.stones, name_count(l.stones));
    printf("Противники: %d<%s>\t", l.enemy, name_count(l.enemy));
    if (l.loc_id >= 0) {
        printf("Модификатор силы противников: %+.2lf (из них %+.2lf от глубины)\n", l.power / 10.0 + int(level / LANDING_DIST) * 0.25, level / LANDING_DIST * 0.25);
        printf("Типичная жидкость: %s\tТипичные существа: %s\n", LOC[l.loc_id].common_fluid, LOC[l.loc_id].common_enemy);
    } else {
        printf("Модификатор силы противников: %+.2lf\n", l.power / 10.0);
    }
    if (l.focus[0] || l.focus[1]) {
        // mostly inspiration
        printf("Фокус (если врагов достаточно для боя):\n");
        if (l.focus[0]) printf("\t%s\n", FOCUSES[l.focus[0]]);
        if (l.focus[1]) printf("\t%s\n", FOCUSES[l.focus[1]]);
    }
    printf("Особенности:\n");
    for (int trouble : l.troubles) {
        printf("\t%s\n", TROUBLES[trouble]);
    }
    printf("Всего дверей - %ld:\n", l.doors.size());
    int door_id = 0;
    for (Door d: l.doors) {
        printf("  %d:\t", door_id);
        int average_skulls = 0, sum_pob = 0;
        for (auto el = d.chances.begin(); el != d.chances.end(); el++) {
            int prob = get_probability(el->second, map_quality);
            average_skulls += prob * LOC[el->first].skull_level;
            sum_pob += prob;
        }
        int possible_skulls = average_skulls + MAX_SKULLS * std::max(100 - sum_pob, 0);
        for (int i = 100; i <= 100 * MAX_SKULLS; i += 100) {
            if (i <= average_skulls + 100 * (average_skulls % 100 >= 50)) printf("\xE2\x98\xA0"); // ☠, 0xF0 0x9F 0x92 0x80 - for 💀
            else if (i <= possible_skulls + 100 * (possible_skulls % 100 >= 50)) printf("?");
            else printf("-");
        }
        printf("\n");
        door_id++;
		int printed_chances = 0;
        for (auto el = d.chances.begin(); el != d.chances.end(); el++) {
            int prob = get_probability(el->second, map_quality);
            if (prob > 0) {
                printf("\t");
                for (int i = 0; i < MAX_SKULLS; i++) {
                    if (i < LOC[el->first].skull_level) printf("\xE2\x98\xA0");
                    else printf("-");
                }
                printf("\t%s - ~%d%%\n", LOC[el->first].name, prob);
				printed_chances++;
            }
        }

        int prob_up = get_probability(d.up, map_quality), prob_same = get_probability(d.same, map_quality), prob_down = get_probability(d.down, map_quality);

        if (prob_up >= prob_same && prob_up >= prob_down) printf("\t^^^");
        else if (prob_down >= prob_up && prob_down >= prob_same) printf("\tVVV");
        else printf("\t===");
        
        if (prob_up > 0) printf("\tВверх - ~%d%%", prob_up);
        if (prob_same > 0) printf("\tНа уровне - ~%d%%", prob_same);
        if (prob_down > 0) printf("\tВниз - ~%d%%", prob_down);
        printf("\n");
		int i = 2;
		while ((i + printed_chances) % 6 != 0) {
	        printf("\t-----\n");
			i++;
		}
    }
}

struct Door create_door(unsigned loc_num, int landing_id, int up_buff) {
    struct Door res = {};
    if (loc_num > LOC_NUM)
        return res;
    int global_probability = MAX_PROBABILITY;
    unsigned done_num = 0;
    while (done_num < loc_num && global_probability > 0) {
        int loc_id = rand() % LOC_NUM;
        if (res.chances.find(loc_id) == res.chances.end()) {
            int cur_probability = rand() % global_probability + 1;
            if (done_num == loc_num - 1)
                cur_probability = global_probability;
            global_probability -= cur_probability;
            done_num++;
            res.chances[loc_id] = cur_probability;
        }
    }
    
    global_probability = MAX_PROBABILITY;
    int door_boost = up_buff * 2 / 5;
    if (rand() % MAX_PROBABILITY - up_buff < MAX_PROBABILITY / 2) {
        // up-directed
        if (landing_id != 0) {
            res.up = rand() % (global_probability - door_boost) + 1 + door_boost;
            if (res.up < 0) res.up = 0;
            global_probability -= res.up;
        } else {
            res.up = 0;
        }
        if (landing_id < 0) res.same = rand() % global_probability + 1;
        else res.same = 0;
        res.down = global_probability - res.same;
    } else {
        // down-directed
        door_boost *= -1; // for right direction
        if (landing_id != 0) {
            res.down = rand() % (global_probability - door_boost) + 1 + door_boost;
            if (res.down < 0) res.down = 0;
            global_probability -= res.down;
            if (landing_id < 0) res.same = rand() % global_probability + 1;
            else res.same = 0;
            res.up = global_probability - res.same;
        } else {
            res.down = global_probability;
            res.same = res.up = 0;
        }
    }
    return res;
}

void gen_doors(struct ConcreteLocation &l, unsigned loc_num, int level, int goal, int luck) {
    int up_buff = (luck - MAX_PROBABILITY / 2) / 2;
    if (0 < level && level < goal) {
        up_buff *= -1; // we shall buff down, not up
    }
    l.doors.clear();
    for (int i = 0; i < l.door_num; i++) {
        if (level % LANDING_DIST == 0 && level < int(LANDING_DIST * LANDING_NUM)) {
            l.doors.push_back(create_door(loc_num, level / LANDING_DIST, up_buff));
        } else {
            l.doors.push_back(create_door(loc_num, -1, up_buff));
        }
    }
}

void gen_troubles(struct ConcreteLocation &l) {
    l.troubles.clear();
    while (rand() % MAX_PROBABILITY <= TROUBLE_CHANCE && l.troubles.size() < MAX_TROUBLES) {
        int trouble_id = rand() % TROUBLE_NUM;
        while (l.troubles.find(trouble_id) != l.troubles.end()) {
            trouble_id = rand() % TROUBLE_NUM;
        }
        l.troubles.insert(trouble_id);
    }
}

int use_door(const struct ConcreteLocation &l, unsigned door_id, int &level, int &goal, int &luck) {
    if (door_id >= l.doors.size())
        return -1;
    // else
    int res_prob = rand() % MAX_PROBABILITY;
    int luck_left = luck;
    if (level < goal && level > 0) {
        if (l.doors[door_id].up + l.doors[door_id].same > res_prob) {
            while (luck_left > 0) {
                if (rand() % MAX_PROBABILITY <= luck_left) {
                    printf("ШАНС! ");
                    luck -= LUCK_STABILISER;
                    res_prob = std::max(res_prob, rand() % MAX_PROBABILITY);
                    if (l.doors[door_id].up + l.doors[door_id].same <= res_prob) break;
                }
                luck_left -= MAX_PROBABILITY;
            }
        } else {
            while (luck_left < MAX_PROBABILITY) {
                if (rand() % MAX_PROBABILITY > luck_left) {
                    printf("Упс... ");
                    luck += LUCK_STABILISER;
                    res_prob = std::min(res_prob, rand() % MAX_PROBABILITY);
                    if (l.doors[door_id].up + l.doors[door_id].same > res_prob) break;
                }
                luck_left += MAX_PROBABILITY;
            }
        }
    } else if (level > goal) {
        if (l.doors[door_id].up > res_prob) {
            while (luck_left < MAX_PROBABILITY) {
                if (rand() % MAX_PROBABILITY > luck_left) {
                    printf("Упс... ");
                    luck += LUCK_STABILISER;
                    res_prob = std::max(res_prob, rand() % MAX_PROBABILITY);
                    if (l.doors[door_id].up <= res_prob) break;
                }
                luck_left += MAX_PROBABILITY;
            }
        } else {
            while (luck_left > 0) {
                if (rand() % MAX_PROBABILITY <= luck_left) {
                    printf("ШАНС! ");
                    luck -= LUCK_STABILISER;
                    res_prob = std::min(res_prob, rand() % MAX_PROBABILITY);
                    if (l.doors[door_id].up > res_prob) break;
                }
                luck_left -= MAX_PROBABILITY;
            }
        }
    }

    if (l.doors[door_id].up > res_prob) {
        if (0 < level && level < goal) {
            luck += LUCK_BUFF;
            // if (luck > MAX_PROBABILITY) luck = MAX_PROBABILITY;
        } else if (level > goal) {
            luck -= LUCK_DEBUFF;
            // if (luck < 0) luck = 0;
        }
        level--;
        printf("Поднялись на %d%s (%.2lf%%)\n", level, (level >= goal ? "!" : ":("), 100.0 * l.doors[door_id].up / MAX_PROBABILITY);
    } else if (l.doors[door_id].up + l.doors[door_id].same > res_prob) {
        printf("На уровне (%.2lf%%)\n", 100.0 * l.doors[door_id].same / MAX_PROBABILITY);
        luck += LUCK_ON_EVEN;
        // if (luck > MAX_PROBABILITY) luck = MAX_PROBABILITY;
    } else {
        if (0 < level && level < goal) {
            luck -= LUCK_DEBUFF;
            // if (luck < 0) luck = 0;
        } else if (level > goal) {
            luck += LUCK_BUFF;
            // if (luck > MAX_PROBABILITY) luck = MAX_PROBABILITY;
        }
        level++;
        printf("Опустились на %d%s (%.2lf%%)\n", level, (level <= goal ? "!" : ":("), 100.0 * l.doors[door_id].down / MAX_PROBABILITY);
    }    

    res_prob = rand() % MAX_PROBABILITY;
    int prob_sum = 0;
    for (auto el = l.doors[door_id].chances.begin(); el != l.doors[door_id].chances.end(); el++) {
        prob_sum += el->second;
        if (prob_sum > res_prob) {
            printf("Пошли в %s (%.2lf%%)\n", LOC[el->first].name, 100.0 * el->second / MAX_PROBABILITY);
            return el->first;
        }
    }
    return -1;
}
