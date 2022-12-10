#include "stat.hpp"

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

double get_door_danger(const Door &d) {
    double average_skulls = 0;
    for (auto el = d.chances.begin(); el != d.chances.end(); el++) {
        average_skulls += (double(el->second) / MAX_PROBABILITY) * LOC[el->first].skull_level;
    }
    return average_skulls;
}

void gen_doors(struct ConcreteLocation &l, unsigned loc_num, int level, int goal, int luck) {
    int up_buff = (luck - MAX_PROBABILITY / 2) / 2;
    if (0 < level && level < goal) {
        up_buff *= -1; // we shall buff down, not up
    }
    l.doors.clear();
    int danger_level = level / int(LANDING_DIST);
    int landing_id = -1; // not in landing by default
    if (level % LANDING_DIST == 0 && level < int(LANDING_DIST * LANDING_NUM)) {
        landing_id = level / LANDING_DIST;
    }
    Door d1 = create_door(loc_num, landing_id, up_buff), d2 = create_door(loc_num, landing_id, up_buff);
    double d1_danger = get_door_danger(d1), d2_danger = get_door_danger(d2);
    for (int i = 0; i < l.door_num; i++) {
        do {
            if (d1_danger < danger_level) {
                if (d1_danger < d2_danger) {
                    d1 = d2;
                    d1_danger = d2_danger;
                }
                d2 = create_door(loc_num, landing_id, up_buff); // generating new door anyway
                d2_danger = get_door_danger(d2);
            }
        } while (d1_danger < danger_level - 1 &&  d1_danger < 3);
        l.doors.push_back(d1);
        d1 = d2;
        d1_danger = d2_danger;
        d2 = create_door(loc_num, landing_id, up_buff);
        d2_danger = get_door_danger(d2);
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