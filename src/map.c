#include "../inc/Define.h"

Map loadmap(const char *filename){
    Map map = {0};
    FILE *file = fopen(filename, "r");
    if (!file){
        fprintf(stderr, "fopen failed %s\n", filename);
        return map;
    }

    char line[1024000];
    int row = 0;

    while (fgets(line, sizeof(line), file) && row < T_MAX_HEIGHT){
        int col = 0;
        while (line[col] != '\0' && line[col] != '\n' && col < T_MAX_WIDTH){
            Tile *t = &map.tiles[row][col];
            char c = line[col];

            t->type = E_TILE;
            t->active = true;
            t->pressed = false;
            t->id = 0;

            if (c == 'F')
                t->type = F_TILE;
            else if (c == '-')
                t->type = E_TILE;
            else if (c == 'T')
                t->type = T_TILE;
            else if (c == 'S') {
                t->type = F_TILE;
                map.Spos = (Vector3){col*TILE_SIZE, 1.25f, row*TILE_SIZE};
            }
            else if (c == 'O')
                t->type = O_TILE;
            else if (c >= 'A' && c <= 'Z') {
                t->type = SF_TILE;
                t->id = c;
            }
            else if (c >= '0' && c <= '9'){
                t->type = HF_TILE;
                t->id = 'A' + (c - '0');
            }
            else if (c >= 'a' && c <= 'z'){
                t->type = B_TILE;
                t->id = c;
                t->active = false;
            }
            col++;
        }
        
        if (col > map.width) map.width = col;
        row++;
    }
    map.height = row;
    fclose(file);
    return map;
}

