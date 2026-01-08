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
    STATE_FINAL,
    STATE_MODE
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

typedef enum {
    NORMAL,
    MEDIUM,
    HARD
} Diff;

typedef struct {
    Diff Diff;
    float boxspeedMult;  
    bool allowCamControl;     
    bool randomCam;     
} GameMode;

typedef struct {
    int moveLimit;
    float timeLimit;
} LevLimits;

static const LevLimits mediumMode[13] = {
    {10, 10.0f},    
    {29, 15.0f},    
    {32, 22.0f},    
    {42, 18.0f},    
    {35, 15.0f},    
    {65, 27.0f},    
    {99, 30.0f},  
    {60, 22.0f},    
    {61, 25.0f},    
    {84, 35.0f},    
    {81, 30.0f},    
    {58, 28.0f},    
    {175, 50.0f}    
};

static const LevLimits hardMode[13] = {
    {17, 15.0f},   
    {35, 35.0f},    
    {57, 50.0f},    
    {50, 45.0f},    
    {44, 45.0f},    
    {66, 70.0f},    
    {102, 110.0f},   
    {68, 60.0f},    
    {64, 65.0f},    
    {98, 95.0f},  
    {82, 90.0f},   
    {59, 60.0f},  
    {175, 200.0f}  
};

typedef struct {
    Vector3 startPos;
    Vector3 targetPos;
    float t;
    float duration;
    bool active;
} CamTrans;


#define COLOR_EMPTY (Color){ 0, 0, 0, 255}
#define COLOR_WIRE (Color){ 65, 65, 75, 255}

//---------MAP--------
Map loadmap(const char *filename);
void drawMap(Map map, float tileSize);

//---------BOX--------
moveres movebox(Block *b, Map *map, int dx, int dz);
void drawbox(Block *b);
moveres checkbox(Block *b, Map *map, GameMode *mode, GameStats *stats, int *currentLevel);
void updateblox(Block *b, float dt);
void resetSB(Map *map);
void incMoves(GameStats *stats);

//---------CAM--------
void updateCamT(Camera3D *cam, CamTrans *ct, float mapCenterX,float mapCenterZ);
void randomCam(Camera3D *cam, CamTrans *ct, float mapCenterX, float mapCenterZ, float maxDim);

//---------UI--------
void drawMen(void);
void DrawDiffMenu(GameMode *mode);
void DrawStatsHUD(GameStats stats, GameMode mode, int currentLevel);
void DrawFinalStatsScreen(GameStats stats);

//---------STATS--------
void initState(GameStats *stats);
void UpdateTime(GameStats *stats, float dt);
void resetCurrLev(GameStats *stats);
void saveLevState(GameStats *stats, int levelIndex);
void exportState(GameStats stats);
void applymode(GameMode *mode);

//---------UTILS--------
bool solid(Tile *t);
void initState(GameStats *stats);
void gettile(Block *b, int *x1, int *z1, int *x2, int *z2);
void toggleBridge(Map *map, char bridgeId);
Vector3 vector3cus(Vector3 a, Vector3 b, float t);
