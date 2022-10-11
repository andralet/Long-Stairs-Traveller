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
    for (TroubleId trouble : l.troubles) {
        printf("\t%s <%s>\n", TROUBLE_GROUPS[trouble.group].elements[trouble.ingroup_id], TROUBLE_GROUPS[trouble.group].name);
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

void gen_troubles(struct ConcreteLocation &l) {
    static int trouble_group_sum = -1;
    if (trouble_group_sum < 0) {
        int trouble_count = 0;
        trouble_group_sum = 0;
        for (unsigned i = 0; i < TROUBLE_GROUP_NUM; i++) {
            trouble_group_sum += TROUBLE_GROUPS[i].weight;
            trouble_count += TROUBLE_GROUPS[i].size;
        }
        if (trouble_count != TROUBLE_NUM) {
            fprintf(stderr, "Trouble number does not match!\n");
            exit(1);
        }
    }
    l.troubles.clear();
    while (rand() % MAX_PROBABILITY <= TROUBLE_CHANCE && l.troubles.size() < MAX_TROUBLES) {
        int trouble_weight = rand() % trouble_group_sum;
        int trouble_group = 0;
        while (TROUBLE_GROUPS[trouble_group].weight <= trouble_weight) {
            trouble_weight -= TROUBLE_GROUPS[trouble_group].weight;
            trouble_group++;
        }
        while (std::any_of(l.troubles.begin(), l.troubles.end(), [&trouble_group](TroubleId id) {return id.group == trouble_group; })) {
            trouble_group = rand() % TROUBLE_GROUP_NUM;
        }
        TroubleId result = {trouble_group, rand() % TROUBLE_GROUPS[trouble_group].size};
        l.troubles.push_back(result);
    }
}