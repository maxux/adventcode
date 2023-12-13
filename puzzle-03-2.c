#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

typedef struct number_t {
    int value;
    int used;
    int adjacent;
    int checked;
    int flag;

} number_t;


typedef struct map_t {
    char **map;
    int lines;
    int width;

    char *symbols;

    number_t ***groups;
    int ratio;

} map_t;

map_t map_load(char *filename) {
    map_t map;
    char buffer[1024];

    FILE *fp = fopen(filename, "r");

    // compute lines number
    map.lines = 0;
    map.ratio = 0;

    while(fgets(buffer, sizeof(buffer), fp))
        map.lines += 1;

    map.map = malloc(sizeof(char *) * map.lines);
    map.groups = malloc(sizeof(number_t **) * map.lines);

    rewind(fp);

    // load map line by line, removing new line at the end
    int line = 0;
    int length;

    while(fgets(buffer, sizeof(buffer), fp)) {
        length = strlen(buffer);

        map.map[line] = strdup(buffer);
        map.map[line][length - 1] = '\0';
        map.groups[line] = calloc(sizeof(number_t *), length);

        line += 1;
    }

    fclose(fp);

    // we know map is a square, let's keep only last width
    map.width = length;

    return map;
}

void map_dump(map_t *map) {
    for(int i = 0; i < map->lines; i++)
        printf("[%3d] %s\n", i, map->map[i]);
}

void symbols_init(map_t *map) {
    char *discards = "0123456789.";

    // there is less than 64 symbols
    map->symbols = calloc(sizeof(char), 64);

    for(int line = 0; line < map->lines; line++) {
        for(int width = 0; width < map->width; width++) {
            char input = map->map[line][width];

            if(strchr(discards, input))
                continue;

            if(strchr(map->symbols, input))
                continue;

            strncat(map->symbols, map->map[line] + width, 1);
        }
    }
}

void groups_init(map_t *map) {
    char *match = "0123456789";

    for(int line = 0; line < map->lines; line++) {
        for(int width = 0; width < map->width; ) {
            char input = map->map[line][width];

            if(input == '\0' || !strchr(match, input)) {
                width += 1;
                continue;
            }

            // new group found
            number_t *number = malloc(sizeof(number_t));
            number->value = atoi(map->map[line] + width);
            number->used = 0;
            number->checked = 0;

            while(map->map[line][width] && strchr(match, map->map[line][width])) {
                map->groups[line][width] = number;
                width += 1;
            }
        }
    }
}

void groups_dump(map_t *map) {
    for(int line = 0; line < map->lines; line++) {
        printf("[%3d] ", line);

        for(int width = 0; width < map->width; width++) {
            int color = 0;

            if(map->map[line][width] == '.')
                color = 30;

            else if(map->map[line][width] == '*')
                color = 33;

            else if(strchr(map->symbols, map->map[line][width]))
                color = 34;

            else if(map->groups[line][width] && map->groups[line][width]->adjacent == 1)
                color = 36;

            else if(map->groups[line][width] && map->groups[line][width]->used == 1)
                color = 32;

            else if(map->groups[line][width] && map->groups[line][width]->used == 0)
                color = 31;

            else
                color = 37;

            printf("\033[1;%dm%c\033[0m", color, map->map[line][width]);
        }

        printf("\n");
    }
}

// real algorythm implementation, ugly, make that better
void groups_validate_around(map_t *map, int line, int width) {
    for(int a = line - 1; a <= line + 1; a++)
        for(int b = width - 1; b <= width + 1; b++)
            if(map->groups[a][b])
                map->groups[a][b]->used = 1;
}

void groups_detect(map_t *map) {
    // ignoring first and last line
    for(int line = 1; line < map->lines - 1; line++) {
        // checking each characters
        for(int width = 1; width < map->width - 1; width++) {
            char input = map->map[line][width];

            if(!strchr(map->symbols, input))
                continue;

            groups_validate_around(map, line, width);
        }
    }
}

void groups_reset_flags(map_t *map) {
    for(int line = 0; line < map->lines; line++)
        for(int width = 0; width < map->width; width++)
            if(map->groups[line][width])
                map->groups[line][width]->flag = 0;
}

void groups_adjacent_validate_around_commit(map_t *map, int line, int width) {
    for(int a = line - 1; a <= line + 1; a++)
        for(int b = width - 1; b <= width + 1; b++)
            if(map->groups[a][b])
                map->groups[a][b]->adjacent = 1;
}

int groups_adjacent_ratio(map_t *map) {
    int ratio = 0;

    for(int line = 0; line < map->lines; line++) {
        for(int width = 0; width < map->width; width++) {
            if(!map->groups[line][width])
                continue;

            if(!map->groups[line][width]->flag)
                continue;

            if(ratio > 0)
                return ratio * map->groups[line][width]->value;

            if(ratio == 0) {
                ratio = map->groups[line][width]->value;
                map->groups[line][width]->flag = 0;
            }
        }
    }

    return 0;
}

void groups_adjacent_validate_around(map_t *map, int line, int width) {
    int found = 0;
    groups_reset_flags(map);

    for(int a = line - 1; a <= line + 1; a++) {
        for(int b = width - 1; b <= width + 1; b++) {
            if(map->groups[a][b] && map->groups[a][b]->flag == 0) {
                map->groups[a][b]->flag = 1;
                found += 1;
            }
        }
    }

    if(found == 2) {
        groups_adjacent_validate_around_commit(map, line, width);
        map->ratio += groups_adjacent_ratio(map);
    }
}

void groups_adjacent(map_t *map) {
    // ignoring first and last line
    for(int line = 1; line < map->lines - 1; line++) {
        // checking each characters
        for(int width = 1; width < map->width - 1; width++) {
            char input = map->map[line][width];

            if(input != '*')
                continue;

            groups_adjacent_validate_around(map, line, width);
        }
    }
}

int groups_sum(map_t *map) {
    int sum = 0;

    for(int line = 0; line < map->lines; line++) {
        for(int width = 0; width < map->width; width++) {
            if(!map->groups[line][width])
                continue;

            if(map->groups[line][width]->used && !map->groups[line][width]->checked) {
                sum += map->groups[line][width]->value;
                map->groups[line][width]->checked = 1;
            }
        }
    }

    return sum;
}

int main() {
    map_t schematic;

    // load original map
    schematic = map_load("puzzle-03.input");
    // map_dump(&schematic);

    // build a list of symbols we need to match
    symbols_init(&schematic);
    printf("[XXX] symbols: %s\n", schematic.symbols);

    // build list of groups on the map
    groups_init(&schematic);
    groups_detect(&schematic);
    groups_adjacent(&schematic);
    groups_dump(&schematic);

    int sum = groups_sum(&schematic);
    printf("[XXX] sum: %d\n", sum);

    printf("[XXX] ratio: %d\n", schematic.ratio);

    return 0;
}
