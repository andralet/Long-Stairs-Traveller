#include "stat.hpp"

const char *LANDING_NAME[LANDING_NUM] = {
    "Hellmouth",
    "Фабрика",
    "Лабиринт",
    "Предел"
};

const int GEN_LOC_NUM = 3;

void show_info(const ConcreteLocation &l, int level, int map_quality) {
    printf("Уровень - %d\t", level);
    printf("Качество карты - %d\n", map_quality);
    if (level % LANDING_DIST == 0 && level < int(LANDING_DIST * LANDING_NUM)) {
        printf("Добро пожаловать в якорную точку: %s!\n", LANDING_NAME[level / LANDING_DIST]);
    }
    print_loc(l, map_quality);
}

int main(void) {
    srand(time(NULL));

    ConcreteLocation l = LANDING[0];
    int level = 0;
    int map_quality = 10;
    int picture_id = 0;

    gen_doors(l, GEN_LOC_NUM, 0);
    if (level % LANDING_DIST == 0 && level < int(LANDING_DIST * LANDING_NUM)) {
        printf("Добро пожаловать в якорную точку: %s!\n", LANDING_NAME[level / LANDING_DIST]);
    }
    print_loc(l, map_quality);

    char cmd[256] = {};
    printf(">");
    scanf("%s", cmd);
    while (cmd[0] != 'q') {
        if (!strcmp(cmd, "info")) {
            show_info(l, level, map_quality);
        } else if (!strcmp(cmd, "go")) {
            int door = 0;
            printf("Дверь (0-%d): ", l.door_num - 1);
            scanf("%d", &door);
            if (door >= 0 && door < l.door_num) {
                int new_level = level;
                int new_loc_id = use_door(l, door, new_level);
                if (new_loc_id == -1)
                    return 1;
                ConcreteLocation new_l = make_loc(new_loc_id);
                if (new_level % LANDING_DIST == 0 && level < int(LANDING_DIST * LANDING_NUM)) {
                    new_l = LANDING[new_level / LANDING_DIST];
                    printf("Добро пожаловать в якорную точку: %s!\n", LANDING_NAME[new_level / LANDING_DIST]);
                    gen_doors(new_l, GEN_LOC_NUM, new_level / LANDING_DIST);
                } else {
                    gen_doors(new_l, GEN_LOC_NUM, -1);
                    gen_troubles(new_l);
                }
                print_loc(new_l, map_quality);
                printf("Захлопнуть дверь?(-1 - да): ");
                scanf("%d", &door);
                if (door < 0) {
                    printf("RDP breach detected!\n");
                    map_quality += 2;
                } else {
                    printf("Удачи!\n");
                    l = new_l;
                    level = new_level;
                }
            }
        } else if (!strcmp(cmd, "trouble")) {
            gen_troubles(l);
            show_info(l, level, map_quality);
        } else if (!strcmp(cmd, "door")) {
            if (level % LANDING_DIST == 0 && level < int(LANDING_DIST * LANDING_NUM)) {
                gen_doors(l, GEN_LOC_NUM, level / LANDING_DIST);
            } else {
                gen_doors(l, GEN_LOC_NUM, -1);
            }
            show_info(l, level, map_quality);
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
                    printf("Сохраняйте по ссылке: https://dnd.alex2000.ru/alex/Data/Pictures/Карты/LongStairs/%d.png\n", picture_id);
                    picture_id++;
                }
            } else {
                printf("Рисуй сам, ленивый ты ДМ!!!\n");
            }
        } else if (!strcmp(cmd, "map")) {
            printf("Текущее качество - %d\nЗадайте новое: ", map_quality);
            scanf("%d", &map_quality);
        } else if (!strcmp(cmd, "level")) {
            printf("Текущая глубина - %d\nЗадайте новую: ", level);
            scanf("%d", &level);
        } else if (!strcmp(cmd, "save")) {
            save(l, level, map_quality, picture_id);
        } else if (!strcmp(cmd, "load")) {
            load(l, level, map_quality, picture_id);
            show_info(l, level, map_quality);
        } else {
            printf("Хелп: info - о локации, go - идти, trouble - перегенерировать особенности, door - перегенерировать двери, show - сгенерить json,\nmap - управление качеством карты, level - управление глубиной,\nsave/load - сохранить/загрузить игру, quit - сдаться\n");
        }
        printf(">");
        scanf("%s", cmd);
    }
    
    printf("Прощай, слабак!\n");
    return 0;
}
