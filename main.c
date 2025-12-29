#include "raylib.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define T_MAX_WIDTH 50
#define T_MAX_HEIGHT 50
#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600
#define TILE_SIZE 1.0f
#define LEVEL_COUNT (sizeof(levels) / sizeof(levels[0]))


typedef struct {
    int width;
    int height;
    int tiles[T_MAX_HEIGHT][T_MAX_WIDTH];
    Vector3 Spos;
} Map;

typedef enum {
    MOVE_OK,
    MOVE_FALL,
    MOVE_WIN
} moveres;

typedef enum {
    STANDING,
    LYING_X,
    LYING_Z 
} orient;

typedef struct {
    Vector3 pos;
    Vector3 Spos;
    orient orient;
    Vector3 Tpos;
    bool mov;
    float angle;
} Block;

Map loadmap(const char *filename)
{
    Map map = {0};
    FILE *file = fopen(filename, "r");
    if (!file) {
        fprintf(stderr, "fopen failed %s\n", filename);
        return map;
    }

    char line[1024];
    int row = 0;

    while (fgets(line, sizeof(line), file) && row < T_MAX_HEIGHT)
    {
        int col = 0;
        while (line[col] != '\0' && line[col] != '\n' && col < T_MAX_WIDTH)
        {
            if (line[col] == 'F')
                map.tiles[row][col] = 1;
            else if (line[col] == 'T')
                map.tiles[row][col] = 2;
            else
                map.tiles[row][col] = 0;

            if (line[col] == 'S') {
                map.Spos = (Vector3){ 
                    col * TILE_SIZE,
                    1.25f,
                    row * TILE_SIZE
                };
                map.tiles[row][col] = 1;
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

void drawMap(Map map, float tileSize)
{
    for (int z = 0; z < map.height; z++) {
        for (int x = 0; x < map.width; x++) {
            Vector3 position = { x * TILE_SIZE, 0.0f, z * TILE_SIZE };
            
            if (map.tiles[z][x] == 1) {
                DrawCube(position, TILE_SIZE, 0.5f, TILE_SIZE, LIGHTGRAY);
                DrawCubeWires(position, TILE_SIZE, 0.5f, TILE_SIZE, GRAY);
            }
            else if(map.tiles[z][x] == 2){
                DrawCube(position, TILE_SIZE, 0.5f, TILE_SIZE, GREEN);
                DrawCubeWires(position, TILE_SIZE, 0.5f, TILE_SIZE, DARKGREEN);
            }
        }
    }
}

void updateblox(Block *b, float dt)
{
    if (!b->mov) return;

    float speed = 180.0f;
    b->angle += speed * dt;
    if (b->angle > 90.0f) b->angle = 90.0f;

    float t = sinf(DEG2RAD * b->angle);

    b->pos.x = b->Spos.x + (b->Tpos.x - b->Spos.x) * t;
    b->pos.y = b->Spos.y + (b->Tpos.y - b->Spos.y) * t;
    b->pos.z = b->Spos.z + (b->Tpos.z - b->Spos.z) * t;

    if (b->angle >= 90.0f) {
        b->pos = b->Tpos;
        b->angle = 0.0f;
        b->mov = false;
        b->Spos = b->pos;
    }
}

void gettile(Block *b, int *x1, int *z1, int *x2, int *z2)
{
    float fx = b->pos.x / TILE_SIZE;
    float fz = b->pos.z / TILE_SIZE;
    
    if (b->orient == STANDING) {
        int cx = (int)roundf(fx);
        int cz = (int)roundf(fz);
        *x1 = cx; *z1 = cz;
        *x2 = cx; *z2 = cz;
    }
    else if (b->orient == LYING_X) {
        *x1 = (int)floorf(fx - 0.5f);
        *x2 = (int)floorf(fx + 0.5f);
        *z1 = (int)roundf(fz);
        *z2 = *z1;
    }
    else {
        *x1 = (int)roundf(fx);
        *x2 = *x1;
        *z1 = (int)floorf(fz - 0.5f);
        *z2 = (int)floorf(fz + 0.5f);
    }
    
    printf("gettile: pos=(%.2f, %.2f), fx=%.2f, fz=%.2f, orient=%d -> tiles[%d][%d] and [%d][%d]\n",
           b->pos.x, b->pos.z, fx, fz, b->orient, *z1, *x1, *z2, *x2);
}

moveres movebox(Block *b, Map *map, int dx, int dz)
{
    if (b->mov) return MOVE_OK;

    printf("\nattemp dx=%d, dz=%d ===\n", dx, dz);

    int x1, z1, x2, z2;
    gettile(b, &x1, &z1, &x2, &z2);

    int nx1 = x1, nz1 = z1;
    int nx2 = x2, nz2 = z2;
    orient newOrient = b->orient;

    if (b->orient == STANDING) {
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
    } else if(b->orient == LYING_X) {
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

    printf("curr tile: [%d][%d],[%d][%d] oritn=%d\n", z1, x1, z2, x2, b->orient);
    printf("tar til :  [%d][%d],[%d][%d] oreint=%d\n", nz1, nx1, nz2, nx2, newOrient);

    bool isinB1 = (nx1 >= 0 && nz1 >= 0 && nx1 < map->width && nz1 < map->height);
    bool isinB2 = (nx2 >= 0 && nz2 >= 0 && nx2 < map->width && nz2 < map->height);
    
    printf("bound check=%d, tile2 inBounds=%d\n", isinB1, isinB2);
    printf("map dem: w=%d, h=%d\n", map->width, map->height);

    if (!isinB1 || !isinB2) {
        printf("ou bond\n");
        return MOVE_FALL;
    }

    int tile1Val = map->tiles[nz1][nx1];
    int tile2Val = map->tiles[nz2][nx2];
    printf("  tile v [%d][%d]=%d, [%d][%d]=%d\n", nz1, nx1, tile1Val, nz2, nx2, tile2Val);

    bool t1 = (tile1Val > 0);
    bool t2 = (tile2Val > 0);

    if (!t1 || !t2) {
        printf("empty line (t1->%d, t2->%d)\n", t1, t2);
        return MOVE_FALL;
    }

    b->Tpos.x = (nx1 + nx2) * 0.5f * TILE_SIZE;
    b->Tpos.z = (nz1 + nz2) * 0.5f * TILE_SIZE;
    b->Tpos.y = (newOrient == STANDING) ? 1.25f : 0.75f;

    printf("accpet move to pospos = (%.2f, %.2f)\n", b->Tpos.x, b->Tpos.z);

    b->Spos = b->pos;
    b->angle = 0.0f;
    b->orient = newOrient;
    b->mov = true;

    return MOVE_OK;
}

void drawbox(Block *b) {
    Vector3 size;
    if (b->orient == STANDING)
        size = (Vector3){1.0f, 2.0f, 1.0f};
    else if (b->orient == LYING_X)
        size = (Vector3){2.0f, 1.0f, 1.0f};
    else
        size = (Vector3){1.0f, 1.0f, 2.0f};

    DrawCube(b->pos, size.x, size.y, size.z, RED);
    DrawCubeWires(b->pos, size.x, size.y, size.z, BLACK);
}

moveres checkbox(Block *b, Map *map)
{
    int x1, z1, x2, z2;
    gettile(b, &x1, &z1, &x2, &z2);

    if (x1 < 0 || z1 < 0 || x1 >= map->width || z1 >= map->height ||
        x2 < 0 || z2 < 0 || x2 >= map->width || z2 >= map->height) {
        printf("checkbox: fail out\n");
        return MOVE_FALL;
    }

    if (map->tiles[z1][x1] == 0 || map->tiles[z2][x2] == 0) {
        printf("checkbox: fail. empty square: [%d][%d]=%d or [%d][%d]=%d\n",
               z1, x1, map->tiles[z1][x1], z2, x2, map->tiles[z2][x2]);
        return MOVE_FALL;
    }

    if (b->orient == STANDING && map->tiles[z1][x1] == 2) {
        printf("Checkbox: win\n");
        return MOVE_WIN;
    }

    return MOVE_OK;
}

#include <unistd.h>


typedef enum {
    STATE_MENU,
    STATE_PLAYING
} GameState;

void DrawMenuUI(void)
{
    DrawText("WELCOME TO BLOXORZ", SCREEN_WIDTH/2 - 220, 120, 40, YELLOW);

    DrawText("Rules:", SCREEN_WIDTH/2 - 200, 200, 25, RAYWHITE);
    DrawText("- Move the block to the GREEN tile", SCREEN_WIDTH/2 - 200, 240, 20, LIGHTGRAY);
    DrawText("- Falling off the map resets the level", SCREEN_WIDTH/2 - 200, 270, 20, LIGHTGRAY);

    DrawText("Controls:", SCREEN_WIDTH/2 - 200, 320, 25, RAYWHITE);
    DrawText("- I J K L : Move the block", SCREEN_WIDTH/2 - 200, 360, 20, LIGHTGRAY);
    DrawText("- W A S D + Mouse : Move camera", SCREEN_WIDTH/2 - 200, 390, 20, LIGHTGRAY);
    DrawText("- R : Reset level", SCREEN_WIDTH/2 - 200, 420, 20, LIGHTGRAY);
    DrawText("- P : Pause / Menu", SCREEN_WIDTH/2 - 200, 450, 20, LIGHTGRAY);

    DrawText("Press ENTER to Start", SCREEN_WIDTH/2 - 160, 520, 25, GREEN);
}




int main(void)
{
    const char *levels[] = {
        "./levels/level1.txt",
        "./levels/level2.txt",
        "./levels/level3.txt"
    };

    int currlev = 0;

    Map map = loadmap(levels[currlev]);

    Block block = {0};
    block.pos = map.Spos;
    block.Spos = map.Spos;
    block.orient = STANDING;
    block.Tpos = block.pos;
    block.mov = false;

    Camera3D cam = {0};
    cam.position = (Vector3){10.0f, 10.0f, 10.0f};
    cam.target   = (Vector3){map.width/2.0f, 0.0f, map.height/2.0f};
    cam.up       = (Vector3){0, 1, 0};
    cam.fovy     = 45.0f;

    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Bloxorz");
    DisableCursor();
    SetTargetFPS(60);

    bool won = false;
    bool fail = false;

    GameState state = STATE_MENU;

    while (!WindowShouldClose())
    {
        float dt = GetFrameTime();
        if (state == STATE_MENU)
        {
            if (IsKeyPressed(KEY_ENTER))
                state = STATE_PLAYING;
        }
        else if (state == STATE_PLAYING)
        {
            if (IsKeyPressed(KEY_P))
                state = STATE_MENU;

            UpdateCamera(&cam, CAMERA_FIRST_PERSON);

            if (!block.mov && !won && !fail)
            {
                moveres res = MOVE_OK;

                if (IsKeyPressed(KEY_I)) res = movebox(&block, &map, 0, -1);
                else if (IsKeyPressed(KEY_K)) res = movebox(&block, &map, 0, 1);
                else if (IsKeyPressed(KEY_J)) res = movebox(&block, &map, -1, 0);
                else if (IsKeyPressed(KEY_L)) res = movebox(&block, &map, 1, 0);
                else if (IsKeyPressed(KEY_R))
                {
                    fail = false;
                    won = false;
                    block.pos = map.Spos;
                    block.Spos = map.Spos;
                    block.orient = STANDING;
                    block.mov = false;
                }

                if (res == MOVE_FALL)
                    fail = true;
            }
            if(!fail)
                updateblox(&block, dt);

            if (!block.mov && !won && !fail)
            {
                moveres chk = checkbox(&block, &map);

                if (chk == MOVE_FALL)
                    fail = true;
                else if (chk == MOVE_WIN)
                    won = true;
            }

            if (won && IsKeyPressed(KEY_ENTER))
            {
                currlev++;
                if (currlev >= LEVEL_COUNT)
                    currlev = 0;

                fail = false;
                won = false;
                map = loadmap(levels[currlev]);
                block.pos = map.Spos;
                block.Spos = map.Spos;
                block.orient = STANDING;
                block.mov = false;
            }
        }

        BeginDrawing();
        ClearBackground(BLACK);

        if (state == STATE_MENU)
        {
            DrawMenuUI();
        }
        else
        {
            BeginMode3D(cam);
            drawMap(map, TILE_SIZE);
            drawbox(&block);
            EndMode3D();

            DrawText("IJKL Move | P Menu | R Reset", 10, 10, 20, WHITE);
                if(fail){
                    if(IsKeyPressed(KEY_R))
                    {
                        block.pos = map.Spos;
                            block.Spos = map.Spos;
                            block.orient = STANDING;
                            block.mov = false;
                            fail = false;
                    }
                const char *failText = "YOU FAIL!";
                const char *failHint = "Press R to retry";

                int failS = 40;
                int hintS = 20;

                int centerY = SCREEN_HEIGHT / 2;

                DrawText(failText, (SCREEN_WIDTH - MeasureText(failText, failS))/2, centerY - 40, failS, RED);
                DrawText(failHint, (SCREEN_WIDTH - MeasureText(failHint, hintS))/2, centerY + 10, hintS, RAYWHITE);
        }

            if (won)
            {
                const char *winText = "YOU WIN!";
                const char *winHint = "Press ENTER for next level";

                int winS = 40;
                int hintS = 20;

                int centerY = SCREEN_HEIGHT / 2;

                DrawText(winText, (SCREEN_WIDTH - MeasureText(winText, winS))/2, centerY - 40, winS, YELLOW);
                DrawText(winHint, (SCREEN_WIDTH - MeasureText(winHint, hintS))/2, centerY + 10, hintS, RAYWHITE);
            }

        }
    
        EndDrawing();

    }
    CloseWindow();
    return 0;
}