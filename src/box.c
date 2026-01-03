#include "../inc/Define.h"

void updateblox(Block *b, float dt){
    if (!b->mov) return;

    float speed = 180.0f;
    b->angle += speed * dt;
    if (b->angle > 90.0f) b->angle = 90.0f;

    float t = sinf(DEG2RAD * b->angle);

    b->pos.x = b->Spos.x + (b->Tpos.x - b->Spos.x) * t;
    b->pos.y = b->Spos.y + (b->Tpos.y - b->Spos.y) * t;
    b->pos.z = b->Spos.z + (b->Tpos.z - b->Spos.z) * t;

    if (b->angle >= 90.0f){
        b->pos = b->Tpos;
        b->angle = 0.0f;
        b->mov = false;
        b->Spos = b->pos;
    }
}

moveres checkbox(Block *b, Map *map){
    int x1, z1, x2, z2;
    gettile(b, &x1, &z1, &x2, &z2);

    if (!solid(&map->tiles[z1][x1]) || !solid(&map->tiles[z2][x2]))
        return MOVE_FALL;

      if (b->orient == STANDING){
            if (map->tiles[z1][x1].type == O_TILE)
                return MOVE_FALL;
        }

    if (b->orient == STANDING && map->tiles[z1][x1].type == T_TILE)
        return MOVE_WIN;

    Tile *tiles[2] = {&map->tiles[z1][x1], &map->tiles[z2][x2]};
    for (int i = 0; i < 2; i++){
        Tile *t = tiles[i];
        if (t->type == SF_TILE){
            if (!t->pressed) {
                toggleBridge(map, tolower(t->id));
                t->pressed = true;
            }
        }

        if (t->type == HF_TILE){
            if (b->orient == STANDING && !t->pressed){
                toggleBridge(map, tolower(t->id));
                t->pressed = true;
            }
        }
    }

    for (int z = 0; z < map->height; z++){
        for (int x = 0; x < map->width; x++){
            Tile *t = &map->tiles[z][x];
            if (t->pressed){
                bool stillOn = (x == x1 && z == z1) || (x == x2 && z == z2);
                if (!stillOn)
                    t->pressed = false;
            }
        }
    }
    return MOVE_OK;
}



moveres movebox(Block *b, Map *map, int dx, int dz){
    if (b->mov)
        return MOVE_OK;
    // printf("\nattemp dx=%d, dz=%d ===\n", dx, dz);
    int x1, z1, x2, z2;
    gettile(b, &x1, &z1, &x2, &z2);

    int nx1 = x1, nz1 = z1;
    int nx2 = x2, nz2 = z2;
    orient newOrient = b->orient;

    if (b->orient == STANDING){
        if(dx > 0){
            nx1 += 1; nx2 += 2;
            newOrient = LYING_X;
        }
            else if(dx < 0){
                nx1 -= 2; nx2 -= 1;
                newOrient = LYING_X;
            }
        else if(dz > 0){
            nz1 += 1; nz2 += 2;
            newOrient = LYING_Z;
        }
        else if(dz < 0){
            nz1 -= 2; nz2 -= 1;
            newOrient = LYING_Z;
        }
    } else if(b->orient == LYING_X){
        if (dx > 0){
            nx1 += 2; nx2 += 1;
            newOrient = STANDING;
        }
        else if(dx < 0) {
            nx1 -= 1; nx2 -= 2;
            newOrient = STANDING;
        }
        else if(dz != 0){ 
            nz1 += dz;
             nz2 += dz;
            }
    } else if(b->orient == LYING_Z){
        if (dz > 0){
            nz1 += 2;
            nz2 += 1;
            newOrient = STANDING;
        }
        else if(dz < 0){
            nz1 -= 1; nz2 -= 2;
            newOrient = STANDING;
        }
        else if(dx != 0){
            nx1 += dx;
            nx2 += dx;
        }
    }

    // printf("curr tile: [%d][%d],[%d][%d] oritn=%d\n", z1, x1, z2, x2, b->orient);
    // printf("tar til :  [%d][%d],[%d][%d] oreint=%d\n", nz1, nx1, nz2, nx2, newOrient);

    bool isinB1 = (nx1 >= 0 && nz1 >= 0 && nx1 < map->width && nz1 < map->height);
    bool isinB2 = (nx2 >= 0 && nz2 >= 0 && nx2 < map->width && nz2 < map->height);
    
    // printf("bound check=%d, tile2 inBounds=%d\n", isinB1, isinB2);
    // printf("map dem: w=%d, h=%d\n", map->width, map->height);

if (!isinB1 || !isinB2){
    // printf("out off bond\n");
    b->Tpos.z = (nz1 + nz2) * 0.5f * TILE_SIZE;
    b->Tpos.x = (nx1 + nx2) * 0.5f * TILE_SIZE;
    b->Tpos.y = (newOrient == STANDING) ? 1.25f : 0.75f; 
    b->Spos = b->pos;
    b->angle = 0.0f;
    b->orient = newOrient;
    b->mov = true;
    return MOVE_FALL;
}
    // int tile1Val = map->tiles[nz1][nx1];
    // int tile2Val = map->tiles[nz2][nx2];
    // printf("  tile v [%d][%d]=%d, [%d][%d]=%d\n", nz1, nx1, tile1Val, nz2, nx2, tile2Val);

    // bool t1 = (tile1Val > 0);
    // bool t2 = (tile2Val > 0);

    // if (!t1 || !t2){
    //     printf("empty line (t1->%d, t2->%d)\n", t1, t2);
    //     return MOVE_FALL;
    // }

    // if (!solid(&map->tiles[nz1][nx1]) || !solid(&map->tiles[nz2][nx2])){
    //     // printf("fall: non-solid tile\n");
    //     return MOVE_FALL;
    // }

if (!solid(&map->tiles[nz1][nx1]) || !solid(&map->tiles[nz2][nx2])){
    b->Tpos.x = (nx1 + nx2) * 0.5f * TILE_SIZE;
    b->Tpos.z = (nz1 + nz2) * 0.5f * TILE_SIZE;
    b->Tpos.y = (newOrient == STANDING) ? 1.25f : 0.75f;
    b->Spos = b->pos;
    b->angle = 0.0f;
    b->orient = newOrient;
    b->mov = true;
    return MOVE_FALL;
}

    b->Tpos.x = (nx1 + nx2) * 0.5f * TILE_SIZE;
    b->Tpos.z = (nz1 + nz2) * 0.5f * TILE_SIZE;
    b->Tpos.y = (newOrient == STANDING) ? 1.25f : 0.75f;

    // printf("accpet move to pospos = (%.2f, %.2f)\n", b->Tpos.x, b->Tpos.z);

    b->Spos = b->pos;
    b->angle = 0.0f;
    b->orient = newOrient;
    b->mov = true;

    return MOVE_OK;
}
