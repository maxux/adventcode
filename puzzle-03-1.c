#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

typedef struct number_t {
    int value;
    int used;
    int checked;

} number_t;


typedef struct map_t {
    char **map;
    int lines;
    int width;

    char *symbols;

    number_t ***groups;

} map_t;

void diep(char *str) {
    fprintf(stderr, "[-] %s: %s\n", str, strerror(errno));
    exit(EXIT_FAILURE);
}

map_t map_load(char *filename) {
    map_t map;
    char buffer[1024];

    FILE *fp = fopen(filename, "r");

    // compute lines number
    map.lines = 0;

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
            if(!map->groups[line][width]) {
                if(strchr(map->symbols, map->map[line][width]))
                    printf("\033[1;36m%c\033[0m", map->map[line][width]);

                else
                    printf("%c", map->map[line][width]);

                continue;
            }

            if(map->groups[line][width]->used)
                printf("\033[1;32m%c\033[0m", map->map[line][width]);
            else
                printf("\033[1;31m%c\033[0m", map->map[line][width]);
        }

        printf("\n");
    }
}

// real algorythm implementation, ugly, make that better
void groups_validate_around(map_t *map, int line, int width) {
    if(map->groups[line - 1][width - 1])
        map->groups[line - 1][width - 1]->used = 1;

    if(map->groups[line - 1][width])
        map->groups[line - 1][width]->used = 1;

    if(map->groups[line - 1][width + 1])
        map->groups[line - 1][width + 1]->used = 1;

    if(map->groups[line][width - 1])
        map->groups[line][width - 1]->used = 1;

    // if(map->groups[line][width])
    //    map->groups[line][width]->used = 1;

    if(map->groups[line][width + 1])
        map->groups[line][width + 1]->used = 1;

    if(map->groups[line + 1][width - 1])
        map->groups[line + 1][width - 1]->used = 1;

    if(map->groups[line + 1][width])
        map->groups[line + 1][width]->used = 1;

    if(map->groups[line + 1][width + 1])
        map->groups[line + 1][width + 1]->used = 1;
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
    map_dump(&schematic);

    // build a list of symbols we need to match
    symbols_init(&schematic);
    printf(">> symbols: %s\n", schematic.symbols);

    // build list of groups on the map
    groups_init(&schematic);
    groups_detect(&schematic);
    groups_dump(&schematic);

    int sum = groups_sum(&schematic);
    printf(">> %d\n", sum);

    return 0;
}
