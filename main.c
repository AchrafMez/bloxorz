#include "raylib.h"
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <ctype.h>

#define T_MAX_WIDTH 50
#define T_MAX_HEIGHT 50
#define SCREEN_WIDTH 1200
#define SCREEN_HEIGHT 800
#define TILE_SIZE 1.0f
#define LEVEL_COUNT 13

typedef enum{
    STATE_MENU,
    STATE_PLAYING,
    STATE_FINAL
} GameState;

typedef enum{
    E_TILE,
    F_TILE,
    T_TILE,
    SF_TILE,
    HF_TILE,
    B_TILE,
    O_TILE,
    // TELE_TILE
} TileType;

typedef struct{
    char id;
    bool active;
    bool pressed;
    TileType type;
} Tile;


typedef struct{
    int width;
    int height;
    // int tiles[T_MAX_HEIGHT][T_MAX_WIDTH];
    Tile tiles[T_MAX_HEIGHT][T_MAX_WIDTH];
    Vector3 Spos;
} Map;

typedef enum{
    MOVE_OK,
    MOVE_FALL,
    MOVE_WIN
} moveres;

typedef enum{
    STANDING,
    LYING_X,
    LYING_Z 
} orient;

typedef struct{
    Vector3 Spos;
    Vector3 pos;
    orient orient;
    Vector3 Tpos;
    bool mov;
    float angle;
} Block;



typedef struct {
    char switchId;
    bool hard;
    char bridgeId;
} SwitchDef;

SwitchDef switches[32];
int switchCount = 0;

typedef struct {
    int moves;
    float time;
} LevelStats;

typedef struct {
    LevelStats levels[13];
    int currentMoves;
    float currentTime;
    float totalTime;
    int totalMoves;
    bool allLevelsCompleted;
} GameStats;


#define COLOR_EMPTY (Color){ 0, 0, 0, 255}
#define COLOR_WIRE (Color){ 65, 65, 75, 255}


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

void toggleBridge(Map *map, char bridgeId){
    for (int z = 0; z < map->height; z++)
        for (int x = 0; x < map->width; x++)
            if (map->tiles[z][x].type == B_TILE &&
                map->tiles[z][x].id == bridgeId)
                map->tiles[z][x].active = !map->tiles[z][x].active;
}


void drawMap(Map map, float tileSize){
    for (int z = 0; z < map.height; z++){
        for (int x = 0; x < map.width; x++){
            Tile *t = &map.tiles[z][x];
            Vector3 position = {x* TILE_SIZE, 0.0f, z * TILE_SIZE};
            if (t->type == F_TILE){
                DrawCube(position, TILE_SIZE, 0.5f, TILE_SIZE, LIGHTGRAY);
                DrawCubeWires(position, TILE_SIZE, 0.5f, TILE_SIZE, COLOR_WIRE);
            }
            else if(t->type == T_TILE){
                DrawCube(position, TILE_SIZE, 0.5f, TILE_SIZE, GREEN);
                DrawCubeWires(position, TILE_SIZE, 0.5f, TILE_SIZE, COLOR_WIRE);
            }
            else if (t->type == B_TILE && t->active){
                DrawCube(position, TILE_SIZE, 0.5f, TILE_SIZE, LIGHTGRAY);
                DrawCubeWires(position, TILE_SIZE, 0.5f, TILE_SIZE, COLOR_WIRE);
            }
            else if (t->type == O_TILE){
                DrawCube(position, TILE_SIZE, 0.5f, TILE_SIZE, ORANGE);
                DrawCubeWires(position, TILE_SIZE, 0.5f, TILE_SIZE, COLOR_WIRE);
            }
            else if (t->type == SF_TILE){
                DrawCube(position, TILE_SIZE, 0.5f, TILE_SIZE, SKYBLUE);
                DrawCubeWires(position, TILE_SIZE, 0.5f, TILE_SIZE, COLOR_WIRE);
            }
            else if (t->type == HF_TILE){
                DrawCube(position, TILE_SIZE, 0.5f, TILE_SIZE, DARKBLUE);
                DrawCubeWires(position, TILE_SIZE, 0.5f, TILE_SIZE, COLOR_WIRE);
            }
        }
    }
}

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

void gettile(Block *b, int *x1, int *z1, int *x2, int *z2){
    float fx = b->pos.x / TILE_SIZE;
    float fz = b->pos.z / TILE_SIZE;
    
    if (b->orient == STANDING){
        int cx = (int)roundf(fx);
        int cz = (int)roundf(fz);
        *x1 = cx; *z1 = cz;
        *x2 = cx; *z2 = cz;
    }
    else if (b->orient == LYING_X){
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
    
    // printf("gettile: pos=(%.2f, %.2f), fx=%.2f, fz=%.2f, orient=%d -> tiles[%d][%d] and [%d][%d]\n",
        //    b->pos.x, b->pos.z, fx, fz, b->orient, *z1, *x1, *z2, *x2);
}

bool solid(Tile *t){
    if (t->type == E_TILE) return false;
    if (t->type == B_TILE && !t->active) return false;
    return true;
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

void drawMen(void){
    int screenWidth = GetScreenWidth();
    int screenHeight = GetScreenHeight();
    int centerX = screenWidth / 2;
    
    const char *title = "WELCOME TO BLOXORZ";
    int titleSize = 50;
    DrawText(title, centerX - MeasureText(title, titleSize) / 2, screenHeight / 6, titleSize, YELLOW);
    
    int start = screenHeight / 3;
    const char *rulesTitle = "How to Play:";
    int ruleSize = 30;
    DrawText(rulesTitle, centerX - MeasureText(rulesTitle, ruleSize) / 2, start, ruleSize, RAYWHITE);
    
    start += 60;
    int controlsX = centerX - 200; 
    
    const char *controls = "I J K L - Move the block";
    DrawText(controls, controlsX, start, 22, LIGHTGRAY);
    
    start += 40;
    const char *camera = "W A S D + Mouse - Move camera";
    DrawText(camera, controlsX, start, 22, LIGHTGRAY);
    
    start += 40;
    const char *reset = "R - Reset level    |    P - Pause/Menu";
    DrawText(reset, controlsX, start, 22, LIGHTGRAY);
    
    start += 70;
    const char *tilesTitle = "Tile Types:";
    DrawText(tilesTitle, centerX - MeasureText(tilesTitle, ruleSize) / 2, start, ruleSize, RAYWHITE);
    start += 60;
    int squareSize = 30;
    int squareX = centerX - 300;
    DrawRectangle(squareX, start, squareSize, squareSize, GREEN);
    const char *green = "Target - Stand on this vertically to win";
    DrawText(green, squareX + squareSize + 20, start + 5, 20, LIGHTGRAY);
    
    start += 45;
    DrawRectangle(squareX, start, squareSize, squareSize, ORANGE);
    const char *orange = "Weak Tile - Cannot stand vertically here";
    DrawText(orange, squareX + squareSize + 20, start + 5, 20, LIGHTGRAY);
    
    start += 45;
    DrawRectangle(squareX, start, squareSize, squareSize, SKYBLUE);
    const char *blue = "Switch - Activates bridges and mechanisms";
    DrawText(blue, squareX + squareSize + 20, start + 5, 20, LIGHTGRAY);

    start+=45;
    DrawRectangle(squareX, start, squareSize, squareSize, DARKBLUE);
    const char *darkblue = "Switch - Activates bridges and mechanisms";
    DrawText(darkblue, squareX + squareSize + 20, start + 5, 20, LIGHTGRAY);
    
    start += 180;
    const char *startText = "Press ENTER to Start";
    int startSize = 32;
    DrawText(startText, centerX - MeasureText(startText, startSize) / 2, start, startSize, GREEN);
}

void resetSB(Map *map){
    for (int z = 0; z < map->height; z++){
        for (int x = 0; x < map->width; x++){
            Tile *t = &map->tiles[z][x];
            if (t->type == SF_TILE || t->type == HF_TILE)
                t->pressed = false;
            if (t->type == B_TILE)
                t->active = false;
        }
    }
}

void initState(GameStats *stats) {
    stats->currentMoves = 0;
    stats->currentTime = 0.0f;
    stats->totalTime = 0.0f;
    stats->totalMoves = 0;
    stats->allLevelsCompleted = false;
    
    for (int i = 0; i < 13; i++) {
        stats->levels[i].moves = -1;
        stats->levels[i].time = 0.0f;
    }
}

void resetCurrLev(GameStats *stats) {
    stats->currentMoves = 0;
    stats->currentTime = 0.0f;
}

void incMoves(GameStats *stats) {
    stats->currentMoves++;
}

void UpdateTime(GameStats *stats, float dt) {
    stats->currentTime += dt;
}

void saveLevState(GameStats *stats, int levelIndex) {
    if (stats->levels[levelIndex].moves == -1 || 
        stats->currentMoves < stats->levels[levelIndex].moves) {
        stats->levels[levelIndex].moves = stats->currentMoves;
        stats->levels[levelIndex].time = stats->currentTime;
    }
    
    stats->totalMoves += stats->currentMoves;
    stats->totalTime += stats->currentTime;
    
    stats->allLevelsCompleted = true;
    for (int i = 0; i < LEVEL_COUNT; i++) {
        if (stats->levels[i].moves == -1) {
            stats->allLevelsCompleted = false;
            break;
        }
    }
}

void DrawStatsHUD(GameStats stats) {
    char moveText[32];
    char timeText[32];
    
    snprintf(moveText, sizeof(moveText), "Moves: %d", stats.currentMoves);
    snprintf(timeText, sizeof(timeText), "Time: %.1fs", stats.currentTime);
    
    DrawText(moveText, GetScreenWidth() - 150, 10, 20, YELLOW);
    DrawText(timeText, GetScreenWidth() - 150, 35, 20, YELLOW);
}

void DrawFinalStatsScreen(GameStats stats) {
    int screenWidth = GetScreenWidth();
    int screenHeight = GetScreenHeight();
    
    DrawRectangle(0, 0, screenWidth, screenHeight, (Color){0, 0, 0, 200});
    
    const char *title = "CONGRATULATIONS! ALL LEVELS COMPLETED!";
    int titleSize = 30;
    DrawText(title, (screenWidth - MeasureText(title, titleSize)) / 2, 50, titleSize, GOLD);
    
    const char *statsHeader = "YOUR PERFORMANCE:";
    int headerSize = 25;
    DrawText(statsHeader, (screenWidth - MeasureText(statsHeader, headerSize)) / 2, 100, headerSize, WHITE);
    
    int bestTotalMoves = 0;
    float bestTotalTime = 0.0f;
    
    for (int i = 0; i < LEVEL_COUNT; i++) {
        if (stats.levels[i].moves != -1) {
            bestTotalMoves += stats.levels[i].moves;
            bestTotalTime += stats.levels[i].time;
        }
    }
    
    int startY = 150;
    int lineHeight = 25;
    
    DrawText("Level", 150, startY, 20, RAYWHITE);
    DrawText("Best Moves", 300, startY, 20, RAYWHITE);
    DrawText("Best Time", 500, startY, 20, RAYWHITE);
    
    for (int i = 0; i < LEVEL_COUNT; i++) {
        int y = startY + (i + 1) * lineHeight;
        
        char levelNum[16];
        snprintf(levelNum, sizeof(levelNum), "Level %d", i);
        DrawText(levelNum, 150, y, 18, WHITE);
        
        if (stats.levels[i].moves != -1) {
            char movesText[16];
            char timeText[16];
            snprintf(movesText, sizeof(movesText), "%d", stats.levels[i].moves);
            snprintf(timeText, sizeof(timeText), "%.2fs", stats.levels[i].time);
            
            DrawText(movesText, 330, y, 18, YELLOW);
            DrawText(timeText, 530, y, 18, YELLOW);
        } else {
            DrawText("---", 330, y, 18, GRAY);
            DrawText("---", 530, y, 18, GRAY);
        }
    }
    
    int totalsY = startY + (LEVEL_COUNT + 2) * lineHeight;
    DrawText("TOTAL:", 150, totalsY, 22, GOLD);
    
    char totalMovesText[32];
    char totalTimeText[32];
    snprintf(totalMovesText, sizeof(totalMovesText), "%d moves", bestTotalMoves);
    snprintf(totalTimeText, sizeof(totalTimeText), "%.2fs", bestTotalTime);
    
    DrawText(totalMovesText, 330, totalsY, 22, GOLD);
    DrawText(totalTimeText, 530, totalsY, 22, GOLD);
    
    const char *shareText = "Save your stats and contribute to the repo!";
    const char *continueText = "Press ENTER to continue playing";
    const char *exportText = "Press S to save stats to file";
    
    int bottomY = screenHeight - 120;
    DrawText(shareText, (screenWidth - MeasureText(shareText, 20)) / 2, bottomY, 20, SKYBLUE);
    DrawText(continueText, (screenWidth - MeasureText(continueText, 18)) / 2, bottomY + 30, 18, WHITE);
    DrawText(exportText, (screenWidth - MeasureText(exportText, 18)) / 2, bottomY + 55, 18, WHITE);
}

void exportState(GameStats stats) {
    FILE *file = fopen("bloxorz_stats.txt", "w");
    if (!file) return;
    
    fprintf(file, "===========================================\n");
    fprintf(file, "      BLOXORZ - GAME STATISTICS\n");
    fprintf(file, "===========================================\n\n");
    
    int totalMoves = 0;
    float totalTime = 0.0f;
    
    for (int i = 0; i < LEVEL_COUNT; i++) {
        if (stats.levels[i].moves != -1) {
            fprintf(file, "Level %d: %d moves, %.2f seconds\n", i, stats.levels[i].moves, stats.levels[i].time);
            totalMoves += stats.levels[i].moves;
            totalTime += stats.levels[i].time;
        }
    }
    
    fprintf(file, "\n-------------------------------------------\n");
    fprintf(file, "TOTAL: %d moves, %.2f seconds\n", totalMoves, totalTime);
    fprintf(file, "===========================================\n");
    fprintf(file, "\nYou can contribute with you stats to:\n");
    fprintf(file, "https://github.com/AchrafMez/bloxorz\n");
    
    fclose(file);
}


int main(void){
    const char *levels[] = {"./levels/level0.txt", "./levels/level1.txt",
    "./levels/level2.txt","./levels/level3.txt","./levels/level4.txt",
    "./levels/level5.txt","./levels/level6.txt","./levels/level7.txt",
    "./levels/level8.txt","./levels/level9.txt","./levels/level10.txt",
    "./levels/level11.txt","./levels/level12.txt", "./levels/level12.txt"};
    int mWidth = GetMonitorWidth(0);
    int mHeight = GetMonitorHeight(0);
    int winWidth = mWidth;
    int winHeight = mHeight;
    int currlev = 0;
    InitWindow(winWidth, winHeight, "Bloxorz");
    ToggleFullscreen();
    DisableCursor();



    SetAudioStreamBufferSizeDefault(16384);
    InitAudioDevice();
    Music bgm = LoadMusicStream("/Users/amezioun/Desktop/bloxorz/assets/theme.wav");
    bgm.looping = true;
    SetMusicVolume(bgm, 0.25f);
    PlayMusicStream(bgm);


    Map map = loadmap(levels[currlev]);
    Block block = {0};
    block.pos = map.Spos;
    block.Spos = map.Spos;
    block.orient = STANDING;
    block.Tpos = block.pos;
    block.mov = false;


    SetTargetFPS(60);


    float mapCenterX = map.width / 2.0f;
    float mapCenterZ = map.height / 2.0f;
    float maxDim = (map.width > map.height) ? map.width : map.height;
    if (maxDim < 8.0f) maxDim = 8.0f;
    float camDistance = maxDim * 1.2f;

    Camera3D cam = {0};
    cam.position = (Vector3){mapCenterX + camDistance * 0.7f, camDistance * 0.8f, mapCenterZ + camDistance * 0.7f};
    cam.target = (Vector3){mapCenterX, 0.0f, mapCenterZ};
    cam.up = (Vector3){0, 1, 0};
    cam.fovy = 45.0f;

    bool won = false;
    bool fail = false;
    bool showFinalStats = false;

    GameState state = STATE_MENU;
    GameStats stats = {0};
    initState(&stats);

    bool animCam = false;
    float animT = 0.0f;
    float animD = 2.0f;

    Vector3 initCamPos;
    Vector3 tarCamPos;
    Vector3 baseCamPosition = cam.position;
    Vector3 baseCamTarget = cam.target;

    while (!WindowShouldClose()){
        UpdateMusicStream(bgm);
        if (animCam){
            animT += GetFrameTime();

            if (animT < animD) {
                float t = animT / animD;
                cam.position.x = initCamPos.x + t *(tarCamPos.x - initCamPos.x);
                cam.position.y = initCamPos.y + t * (tarCamPos.y - initCamPos.y);
                cam.position.z =initCamPos.z + t * (tarCamPos.z - initCamPos.z);
            } else {
                animCam = false;
                cam.position = tarCamPos;
            }
        }
        float dt = GetFrameTime();
        if (state == STATE_MENU){
            if (IsKeyPressed(KEY_ENTER))
                state = STATE_PLAYING;
        }
        else if (state == STATE_PLAYING){
            if (IsKeyPressed(KEY_P))
                state = STATE_MENU;
            if (!fail && !won && !block.mov)
                UpdateTime(&stats, dt);

            if (!animCam && !won && !fail){
                Vector3 forward = {cam.target.x - cam.position.x, 0, cam.target.z - cam.position.z};
                Vector3 right = {forward.z, 0, -forward.x};
                
                float moveSpeed = 0.005f;
                
                if (IsKeyDown(KEY_W)) {
                    cam.position.x += forward.x * moveSpeed;
                    cam.position.z += forward.z * moveSpeed;
                    cam.target.x += forward.x * moveSpeed;
                    cam.target.z += forward.z * moveSpeed;
                }
                if (IsKeyDown(KEY_S)) {
                    cam.position.x -= forward.x * moveSpeed;
                    cam.position.z -= forward.z * moveSpeed;
                    cam.target.x -= forward.x * moveSpeed;
                    cam.target.z -= forward.z * moveSpeed;
                }
                if (IsKeyDown(KEY_A)) {
                    cam.position.x += right.x * moveSpeed;
                    cam.position.z += right.z * moveSpeed;
                    cam.target.x += right.x * moveSpeed;
                    cam.target.z += right.z * moveSpeed;
                }
                if (IsKeyDown(KEY_D)) {
                    cam.position.x -= right.x * moveSpeed;
                    cam.position.z -= right.z * moveSpeed;
                    cam.target.x -= right.x * moveSpeed;
                    cam.target.z -= right.z * moveSpeed;
                }
                
                Vector2 mouseDelta = GetMouseDelta();
                
                mouseDelta.x *= 0.003f;
                mouseDelta.y *= 0.003f;
                
                Vector3 camToTarget = {
                    cam.position.x - cam.target.x,
                    cam.position.y - cam.target.y,
                    cam.position.z - cam.target.z
                };
                
                float radius = sqrtf(camToTarget.x * camToTarget.x + camToTarget.y * camToTarget.y + camToTarget.z * camToTarget.z);
                float angleXZ = atan2f(camToTarget.z, camToTarget.x);
                float angleY = asinf(camToTarget.y / radius);
                
                angleXZ -= mouseDelta.x;
                angleY -= mouseDelta.y;
                
                angleY = fmaxf(-1.4f, fminf(1.4f, angleY));
                cam.position.x = cam.target.x + radius * cosf(angleY) * cosf(angleXZ);
                cam.position.y = cam.target.y + radius * sinf(angleY);
                cam.position.z = cam.target.z + radius * cosf(angleY) * sinf(angleXZ);
                
                float minDist = maxDim * 0.8f;
                float maxDist = maxDim * 2.0f;
                if (radius < minDist || radius > maxDist) {
                    radius = fmaxf(minDist, fminf(maxDist, radius));
                    cam.position.x = cam.target.x + radius * cosf(angleY) * cosf(angleXZ);
                    cam.position.y = cam.target.y + radius * sinf(angleY);
                    cam.position.z = cam.target.z + radius * cosf(angleY) * sinf(angleXZ);
                }
                
                float padding = maxDim * 0.3f;
                cam.target.x = fmaxf(-padding, fminf(map.width + padding, cam.target.x));
                cam.target.z = fmaxf(-padding, fminf(map.height + padding, cam.target.z));
            }

            if (!block.mov && !won && !fail)
            {
                moveres res = MOVE_OK;
                bool moved = false;
                
                if(IsKeyPressed(KEY_N))
                    won = true;
                if (IsKeyPressed(KEY_I)) {
                    res = movebox(&block, &map, 0, -1);
                    moved = true;
                }
                else if (IsKeyPressed(KEY_K)) {
                    res = movebox(&block, &map, 0, 1);
                    moved = true;
                }
                else if (IsKeyPressed(KEY_J)) {
                    res = movebox(&block, &map, -1, 0);
                    moved = true;
                }
                else if (IsKeyPressed(KEY_L)) {
                    res = movebox(&block, &map, 1, 0);
                    moved = true;
                }
                else if (IsKeyPressed(KEY_R)){
                    resetSB(&map);
                    fail = false;
                    won = false;
                    block.pos = map.Spos;
                    block.Spos = map.Spos;
                    block.orient = STANDING;
                    block.mov = false;
                    animCam = false;
                    cam.position = baseCamPosition;
                    cam.target = baseCamTarget;
                    resetCurrLev(&stats);
                }

                if (moved && res != MOVE_FALL) {
                    incMoves(&stats);
                }

                if (res == MOVE_FALL)
                    fail = true;
            }
            
            // if(!fail)
                updateblox(&block, dt);

            if (!block.mov && !won && !fail){
                moveres chk = checkbox(&block, &map);

                if (chk == MOVE_FALL)
                    fail = true;
                else if (chk == MOVE_WIN)
                    won = true;
            }

            if (won && IsKeyPressed(KEY_ENTER)){
                saveLevState(&stats, currlev);
                currlev++;
                if (currlev >= LEVEL_COUNT) {
                    showFinalStats = true;
                    state = STATE_FINAL;
                    // currlev = 0;
                } else {
                    fail = false;
                    won = false;
                    map = loadmap(levels[currlev]);
                    block.pos = map.Spos;
                    block.Spos = map.Spos;
                    block.orient = STANDING;
                    block.mov = false;
                    
                    mapCenterX = map.width / 2.0f;
                    mapCenterZ = map.height / 2.0f;
                    maxDim = (map.width > map.height) ? map.width : map.height;
                    if (maxDim < 8.0f) maxDim = 8.0f;
                    camDistance = maxDim * 1.2f;
                    
                    baseCamPosition = (Vector3){mapCenterX + camDistance * 0.7f, camDistance * 0.8f, mapCenterZ + camDistance * 0.7f};
                    baseCamTarget = (Vector3){mapCenterX, 0.0f, mapCenterZ};
                    cam.position = baseCamPosition;
                    cam.target = baseCamTarget;
                    animCam = false;
                    
                    resetCurrLev(&stats);
                }
            }
            if (fail && IsKeyPressed(KEY_R)) {
                resetSB(&map);
                block.pos = map.Spos;
                block.Spos = map.Spos;
                block.orient = STANDING;
                block.mov = false;
                fail = false;
                animCam = false;
                cam.position = baseCamPosition;
                cam.target = baseCamTarget;
                resetCurrLev(&stats);
            }
        }

        BeginDrawing();
        ClearBackground(COLOR_EMPTY);
        
        if (state == STATE_FINAL) {
            DrawFinalStatsScreen(stats);

            
            if (IsKeyPressed(KEY_ENTER)) {
                showFinalStats = false;
                state = STATE_PLAYING;
    
                currlev = 0;
                fail = false;
                won = false;
                map = loadmap(levels[currlev]);
                block.pos = map.Spos;
                block.Spos = map.Spos;
                block.orient = STANDING;
                block.mov = false;
                
                mapCenterX = map.width / 2.0f;
                mapCenterZ = map.height / 2.0f;
                maxDim = (map.width > map.height) ? map.width : map.height;
                if (maxDim < 8.0f) maxDim = 8.0f;
                camDistance = maxDim * 1.2f;
                
                baseCamPosition = (Vector3){mapCenterX + camDistance * 0.7f, camDistance * 0.8f, mapCenterZ + camDistance * 0.7f};
                baseCamTarget = (Vector3){mapCenterX, 0.0f, mapCenterZ};
                cam.position = baseCamPosition;
                cam.target = baseCamTarget;
                animCam = false;
                initState(&stats);
            }
            
            if (IsKeyPressed(KEY_S)){
                exportState(stats);
            }
        }
        else if (state == STATE_MENU)
            drawMen();
        else
        {
            BeginMode3D(cam);
            drawMap(map, TILE_SIZE);
            drawbox(&block);
            EndMode3D();

            char levT[32];
            snprintf(levT, sizeof(levT), "Level %d", currlev);
            DrawText(levT, 25, 10, 22, WHITE);
                        
            DrawStatsHUD(stats);
            
            if (fail) {
                if (!animCam) {
                    animCam = true;
                    animT = 0.0f;
                    initCamPos = cam.position;
                    tarCamPos = (Vector3){cam.position.x, cam.position.y + 10.0f, cam.position.z + 10.0f};
                }

                const char *failText = "YOU FAIL!";
                const char *failHint = "Press R to retry";

                int currWidth = GetScreenWidth();
                int currHeight = GetScreenHeight();
                int winFont = 40;
                int hintFont = 20;

                int centerY = currHeight / 2.5;

                DrawText(failText, (currWidth - MeasureText(failText, winFont)) / 2, centerY - 40, winFont, RED);
                DrawText(failHint, (currWidth - MeasureText(failHint, hintFont)) / 2, centerY + 10, hintFont, RAYWHITE);
            }

            if (won) {  
                if (!animCam){
                    animCam = true;
                    animT = 0.0f;
                    initCamPos = cam.position;
                    tarCamPos = (Vector3){cam.position.x, cam.position.y + 10.0f, cam.position.z - 10.0f};
                }
                
                const char *winText = "YOU WIN!";
                const char *winHint = "Press ENTER for next level";
                
                int currWidth = GetScreenWidth();
                int currHeight = GetScreenHeight();
                
                int winFont = 40;
                int hintFont = 20;
                
                int centerY = currHeight / 2.5;
                
                DrawText(winText, (currWidth - MeasureText(winText, winFont)) / 2, centerY - 40, winFont, YELLOW);
                DrawText(winHint, (currWidth - MeasureText(winHint, hintFont)) / 2, centerY + 10, hintFont, RAYWHITE);
                
                char statsText[64];
                snprintf(statsText, sizeof(statsText), "Completed in %d moves, %.1fs", 
                stats.currentMoves, stats.currentTime);
                DrawText(statsText, (currWidth - MeasureText(statsText, 18)) / 2, centerY + 50, 18, SKYBLUE);
                if(currlev >= LEVEL_COUNT)
                    state = STATE_FINAL;
            }
        }
        EndDrawing();
    }
    UnloadMusicStream(bgm);
    CloseAudioDevice();
    CloseWindow();
    return 0;
}