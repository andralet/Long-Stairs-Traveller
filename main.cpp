#include "stat.hpp"

const char *LANDING_NAME[LANDING_NUM] = {
    "Hellmouth",
    "Фабрика",
    "Кристальный грот",
    "Лабиринт",
    "Предел"
};

const int GEN_LOC_NUM = 4;

const int LUCK_LEVEL_NUM = 7;
const char *LUCK_LEVELS[LUCK_LEVEL_NUM] = {
    "Отвратительное",
    "Так себе",
    "Терпимое",
    "Норм",
    "Приятное",
    "Существенное",
    "Шикарное"
};

void show_info(const ConcreteLocation &l, int level, int map_quality, int goal, int luck) {
    printf("Уровень - %d\tЦель - %d\n", level, goal);
    const char *luck_level;
    if (luck >= MAX_PROBABILITY) {
        luck_level = "Умопомрачительное";
    } else if (luck < 0) {
        luck_level = "Антиумопомрачительное";
    } else {
        luck_level = LUCK_LEVELS[luck / (MAX_PROBABILITY / (LUCK_LEVEL_NUM))];
    }
    printf("Качество карты - ±%.1lf%%\tВезение - %s\n", map_quality / 2.0, luck_level);
    if (level % LANDING_DIST == 0 && level < int(LANDING_DIST * LANDING_NUM)) {
        printf("Добро пожаловать в якорную точку: %s!\n", LANDING_NAME[level / LANDING_DIST]);
    }
    print_loc(l, map_quality, level);
}

int main(void) {
    srand(time(NULL));

    ConcreteLocation l = LANDING[0];
    int level = 0, goal = 30;
    int map_quality = 10;
    int picture_id = 0;
    int luck = BASIC_LUCK;

    gen_doors(l, GEN_LOC_NUM, level, goal, luck);
    show_info(l, level, map_quality, goal, luck);

    char cmd[256] = {};
    printf(">");
    scanf("%s", cmd);
    while (cmd[0] != 'q') {
        if (!strcmp(cmd, "info")) {
            show_info(l, level, map_quality, goal, luck);
        } else if (!strcmp(cmd, "go")) {
            int door = 0;
            // printf("Дверь (0-%d): ", l.door_num - 1);
            scanf("%d", &door);
            if (door >= 0 && door < l.door_num) {
                int new_level = level;
                int new_luck = luck;
                int new_loc_id = use_door(l, door, new_level, goal, new_luck);
                if (new_loc_id == -1)
                    return 1;
                ConcreteLocation new_l = make_loc(new_loc_id);
                if (new_level % LANDING_DIST == 0 && level < int(LANDING_DIST * LANDING_NUM)) {
                    new_l = LANDING[new_level / LANDING_DIST];
                    gen_doors(new_l, GEN_LOC_NUM, new_level, goal, new_luck);
                    printf("Добро пожаловать в якорную точку: %s!\n", LANDING_NAME[new_level / LANDING_DIST]);
                } else {
                    gen_doors(new_l, GEN_LOC_NUM, new_level, goal, new_luck);
                    gen_troubles(new_l);
                    if (new_l.troubles.empty()) {
                        // greater chances for at least one trouble
                        gen_troubles(new_l);
                    }
                }
                show_info(new_l, new_level, map_quality, goal, new_luck);
                printf("Захлопнуть дверь?(-1 - да): ");
                scanf("%d", &door);
                if (door < 0) {
                    printf("RDP breach detected!\n");
                    map_quality += 4;
                } else {
                    printf("Удачи!\n");
                    l = new_l;
                    level = new_level;
                    luck = new_luck;
                    if (level == goal && goal > 0) {
                        printf("Ура!!! Цель достигнута!!! Новая цель - Hellmouth:)\n");
                        goal = 0;
                    }
                }
            }
        } else if (!strcmp(cmd, "loc")) {
            if (level % LANDING_DIST == 0 && level < int(LANDING_DIST * LANDING_NUM)) {
                ConcreteLocation new_l = LANDING[level / LANDING_DIST];
                gen_doors(new_l, GEN_LOC_NUM, level, goal, luck);
                gen_troubles(new_l);
                if (new_l.troubles.empty()) {
                    // greater chances for at least one trouble
                    gen_troubles(new_l);
                }
                printf("Добро пожаловать в якорную точку: %s!\n", LANDING_NAME[level / LANDING_DIST]);
                print_loc(new_l, map_quality, level);
                int ans = -1;
                printf("Отменить?(-1 - да): ");
                scanf("%d", &ans);
                if (ans >= 0) {
                    printf("Удачи!\n");
                    l = new_l;
                }
            } else {
                int loc_id = 0;
                printf("ID (0-%d): ", LOC_NUM - 1);
                scanf("%d", &loc_id);
                if (loc_id >= 0 && loc_id < int(LOC_NUM)) {
                    ConcreteLocation new_l = make_loc(loc_id);
                    gen_doors(new_l, GEN_LOC_NUM, level, goal, luck);
                    gen_troubles(new_l);
                    if (new_l.troubles.empty()) {
                        // greater chances for at least one trouble
                        gen_troubles(new_l);
                    }
                    print_loc(new_l, map_quality, level);
                    printf("Отменить?(-1 - да): ");
                    scanf("%d", &loc_id);
                    if (loc_id >= 0) {
                        printf("Удачи!\n");
                        l = new_l;
                    }
                }
            }
        } else if (!strcmp(cmd, "trouble")) {
            gen_troubles(l);
            if (l.troubles.empty()) {
                // greater chances for at least one trouble
                gen_troubles(l);
            }
            show_info(l, level, map_quality, goal, luck);
        } else if (!strcmp(cmd, "door")) {
            gen_doors(l, GEN_LOC_NUM, level, goal, luck);
            show_info(l, level, map_quality, goal, luck);
        } else if (!strcmp(cmd, "focus")) {
            printf("Какой меняем (0 - 1ый, 1 - 2ой, 2 - оба)? ");
            int ans = -1;
            scanf("%d", &ans);
            if (ans == 2) {
                l.focus[0] = rand() % FOCUS_NUM;
                do {
                    l.focus[1] = rand() % FOCUS_NUM;
                } while (l.focus[1] == l.focus[0]);
            } else if (ans == 0 || ans == 1) {
                do {
                    l.focus[ans] = rand() % FOCUS_NUM;
                } while (l.focus[ans] == l.focus[!ans]);
            } else {
                printf("Неверный ввод!\n");
            }
            show_info(l, level, map_quality, goal, luck);
        } else if (!strcmp(cmd, "show")) {
            FILE *out = fopen("showed_map.json", "w");
            MapSettings data = {};
            if (l.loc_id >= 0) {
                int use_crates = 0;
                printf("Враги используют ящики? (1 - да, 0 - нет): ");
                scanf("%d", &use_crates);
                data = LOC[l.loc_id].map_data;
                data.width = l.x; data.height = l.y;
                data.door_num = l.door_num + 1;
                data.plant_chance = l.plants; data.stone_chance = l.stones;
                data.crate_chance = l.enemy * use_crates;
                if (out) {
                    make_map(out, data);
                    fclose(out);
                    printf("DONE\n");
                    printf("Сохраняйте по ссылке: https://dnd.alex2000.ru/alex/Data/Pictures/Карты/LongStairs/%d.jpg\n", picture_id);
                    picture_id++;
                }
            } else {
                printf("Рисуй сам, ленивый ты ДМ!!!\n");
            }
        } else if (!strcmp(cmd, "goal")) {
            printf("Текущая цель - %d\nЗадайте новую: ", goal);
            scanf("%d", &goal);
        } else if (!strcmp(cmd, "luck")) {
            printf("Текущая удача - %d\nЗадайте новую (%d<=>100%%): ", luck, MAX_PROBABILITY);
            scanf("%d", &luck);
        } else if (!strcmp(cmd, "map")) {
            printf("Текущее качество - %d\nЗадайте новое: ", map_quality);
            scanf("%d", &map_quality);
        } else if (!strcmp(cmd, "level")) {
            printf("Текущая глубина - %d\nЗадайте новую: ", level);
            scanf("%d", &level);
        } else if (!strcmp(cmd, "pic")) {
            printf("Текущий id карты - %d\nЗадайте новый: ", picture_id);
            scanf("%d", &picture_id);
        } else if (!strcmp(cmd, "save")) {
            save(l, level, map_quality, picture_id, goal, luck);
        } else if (!strcmp(cmd, "load")) {
            load(l, level, map_quality, picture_id, goal, luck);
            show_info(l, level, map_quality, goal, luck);
        } else {
            printf("Хелп:\n\tinfo - о локации, go - идти,\n\tПерегенерировать: loc - локацию по id, trouble - особенности, door - двери, focus - фокус монстров\n\tshow - сгенерить json,\n\tУправление: goal - целью, luck - удачей, map - качеством карты, level - глубиной, pic - id карты для отображения\n\tsave/load - сохранить/загрузить игру, quit - сдаться\n");
        }
        printf(">");
        scanf("%s", cmd);
    }
    
    printf("Прощай, слабак!\n");
    return 0;
}
