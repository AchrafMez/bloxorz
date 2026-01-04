#pragma once
#include <raylib.h>
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
// extern SwitchDef switches[32];
// extern    int switchCount;


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


Map loadmap(const char *filename);
void initState(GameStats *stats);
void UpdateTime(GameStats *stats, float dt);
moveres movebox(Block *b, Map *map, int dx, int dz);
void resetCurrLev(GameStats *stats);
void incMoves(GameStats *stats);
void saveLevState(GameStats *stats, int levelIndex);
void exportState(GameStats stats);
moveres checkbox(Block *b, Map *map);
bool solid(Tile *t);
void resetSB(Map *map);
void initState(GameStats *stats);
void updateblox(Block *b, float dt);
void gettile(Block *b, int *x1, int *z1, int *x2, int *z2);
void toggleBridge(Map *map, char bridgeId);
void drawMap(Map map, float tileSize);
void drawbox(Block *b);
void drawMen(void);
void DrawStatsHUD(GameStats stats);
void DrawFinalStatsScreen(GameStats stats);