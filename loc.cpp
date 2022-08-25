#include "stat.hpp"

const char *TROUBLES[] = {
    "Слабый туман (40 фт зрения)",
    "Сильный туман (20 фт зрения)",
    "Жутчайший туман (5 фт зрения)",
    "Меметическая угроза",
    "Антимеметическая угроза",
    "До одной из дверей сложно добраться",
    "Случай из RDP: еда/вода",
    "Случай из RDP: торговец",
    "Ни одна из дверей не соответствует описанию (прибавьте к выбору игроков 1)",
    "Ни одна из дверей не соответствует описанию (прибавьте к выбору игроков 3)",
    "Ни одна из дверей не соответствует описанию (вычтите из выбора игроков 1)",
    "Ни одна из дверей не соответствует описанию (вычтите из выбора игроков 3)",
    "В комнате не работает магия",
    "Враги бестелесны и против них не работает материальное оружие",
    "Комната наводнена ловушками",
    "Комната находится во временной петле",
    "Если вы спускались вниз, эта комната ещё на уровень ниже",
    "Если вы поднимались вверх, эта комната ещё на уровень выше",
    "Вы попали в ближайшую якорную точку",
    "Вы попали в Hellmouth",
    "Комната погружена в магическую тьму",
    "Комната магически освещена",
    "Сильная вонь (плохо работает обоняние)",
    "Постоянно слышны шепотки и злобные смешки из не обозреваемых частей комнаты",
    "Комната наполнена несвязным криком (проблемы со слухом)",
    "Комната находится в зоне магической тишины",
    "Все двери одинаковы и согласно картам ведут к скоропостижной кончине (хотя по факту это не так)",
    "Все двери одинаковы и согласно картам ведут вниз в лес (хотя по факту это не так)",
    "Все двери одинаковы и согласно картам ведут вверх в болото (хотя по факту это не так)",
    "Здесь лежит тело бойца",
    "На постаменте аккуратно разложены карты (следующие им улучшают карту на 5)",
    "Здесь лежит тело неизвестного существа",
    "Здесь есть босс (одиночка силы +1)",
    "Здесь есть босс (одиночка силы +2)",
    "Монстров нет",
    "Все предметы (в том числе двери) из этой комнаты невидимы",
    "Сила гравитации вдвое выше",
    "Сила гравитации вдвое меньше",
    "Сила гравитации направлена к потолку"
};

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
    res.power = gen_range(LOC[loc_id].power_min * 10, LOC[loc_id].power_max * 10) / 10.0;
    return res;
}

int get_probability(int chance, int map_quality) {
    int prob = int(100.0 * chance / MAX_PROBABILITY);
    if (prob % map_quality <= map_quality / 2) prob -= prob % map_quality;
    else prob = prob - prob % map_quality + map_quality;
    return prob;
}

void print_loc(ConcreteLocation l, int map_quality) {
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
    printf("Модификатор силы противников: %.1lf\n", l.power);
    if (l.loc_id >= 0) {
        printf("Типичная жидкость: %s\tТипичные существа: %s\n", LOC[l.loc_id].common_fluid, LOC[l.loc_id].common_enemy);
    }
    printf("Особенности:\n");
    for (int trouble : l.troubles) {
        printf("\t%s\n", TROUBLES[trouble]);
    }
    printf("Всего дверей - %ld:\n", l.doors.size());
    int door_id = 0;
    for (Door d: l.doors) {
	    printf("\t-----\n");
        printf("  %d:", door_id);
        door_id++;
		int printed_chances = 0;
        for (auto el = d.chances.begin(); el != d.chances.end(); el++) {
            int prob = get_probability(el->second, map_quality);
            if (prob > 0) {
                printf("\t%s - ~%d%%\n", LOC[el->first].name, prob);
				printed_chances++;
            }
        }
        int prob = get_probability(d.up, map_quality);
        printf("\t!!!");
        if (prob > 0) printf("\tВверх - ~%d%%", prob);
        prob = get_probability(d.same, map_quality);
        if (prob > 0) printf("\tНа уровне - ~%d%%", prob);
        prob = get_probability(d.down, map_quality);
        if (prob > 0) printf("\tВниз - ~%d%%", prob);
        printf("\n");
		int i = 2;
		while ((i + printed_chances) % 6 != 0) {
	        printf("\t-----\n");
			i++;
		}
    }
}

struct Door create_door(unsigned loc_num, int landing_id) {
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
    if (landing_id != 0) {
        res.up = rand() % global_probability + 1;
        global_probability -= res.up;
    } else {
        res.up = 0;
    }
    if (landing_id < 0) res.same = rand() % global_probability + 1;
    else res.same = 0;
    res.down = global_probability - res.same;
    return res;
}

void gen_doors(struct ConcreteLocation &l, unsigned loc_num, int landing_id) {
    l.doors.clear();
    for (int i = 0; i < l.door_num; i++) {
        l.doors.push_back(create_door(loc_num, landing_id));
    }
}

void gen_troubles(struct ConcreteLocation &l) {
    l.troubles.clear();
    while (rand() % MAX_PROBABILITY <= TROUBLE_CHANCE) {
        int trouble_id = rand() % TROUBLE_NUM;
        while (l.troubles.find(trouble_id) != l.troubles.end()) {
            trouble_id = rand() % TROUBLE_NUM;
        }
        l.troubles.insert(trouble_id);
    }
}

int use_door(const struct ConcreteLocation &l, unsigned door_id, int &level) {
    if (door_id >= l.doors.size())
        return -1;
    // else
    int res_prob = rand() % MAX_PROBABILITY;
    if (l.doors[door_id].up > res_prob) {
        level--;
        printf("Поднялись на %d! (%.2lf%%)\n", level, 100.0 * l.doors[door_id].up / MAX_PROBABILITY);
    } else if (l.doors[door_id].up + l.doors[door_id].same > res_prob) {
        printf("На уровне (%.2lf%%)\n", 100.0 * l.doors[door_id].same / MAX_PROBABILITY);
    } else {
        level++;
        printf("Опустились на %d:( (%.2lf%%)\n", level, 100.0 * l.doors[door_id].down / MAX_PROBABILITY);
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