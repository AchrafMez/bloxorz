#include "../inc/Define.h"
void applymode(GameMode *mode){
    switch(mode->Diff)
    {
        case NORMAL:
            mode->boxspeedMult = 1.0f;
            mode->allowCamControl = true;
            mode->randomCam = false;
            break;

        case MEDIUM:
            mode->boxspeedMult = 1.3f;
            mode->allowCamControl = true;
            mode->randomCam = false;
            break;

        case HARD:
            mode->boxspeedMult = 1.5f;
            mode->allowCamControl = false;
            mode->randomCam = true;
            break;
    }
}

void toggleBridge(Map *map, char bridgeId){
    for (int z = 0; z < map->height; z++)
        for (int x = 0; x < map->width; x++)
            if (map->tiles[z][x].type == B_TILE &&
                map->tiles[z][x].id == bridgeId)
                map->tiles[z][x].active = !map->tiles[z][x].active;
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

void exportState(GameStats stats) {
    char filename[256];
    
    #ifdef _WIN32
        char *username = getenv("USERNAME");
    #else
        char *username = getenv("USER");
    #endif
    
    if (username) {
        sprintf(filename, "Stats/%s_stat.txt", username);
    } else {
        sprintf(filename, "Stats/player_stat.txt");
    }
    
    FILE *file = fopen(filename, "w");
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
    // fprintf(file, "\nYou can contribute with you stats to:\n");
    // fprintf(file, "https://github.com/AchrafMez/bloxorz\n");
    
    fclose(file);
}



Vector3 vector3cus(Vector3 a, Vector3 b, float t){
    return (Vector3){a.x + (b.x - a.x) * t,a.y + (b.y - a.y) * t,a.z + (b.z - a.z) * t};
}


void updateCamT(Camera3D *cam, CamTrans *ct, float mapCenterX,float mapCenterZ){
    if (!ct->active)
        return;

    ct->t += GetFrameTime() * 2.0f;

    if (ct->t >= 1.0f) {
        ct->t = 1.0f;
        ct->active = false;
    }

    cam->position = vector3cus(ct->startPos, ct->targetPos, ct->t);
    cam->target   = (Vector3){ mapCenterX, 0.0f, mapCenterZ };
}

void randomCam(Camera3D *cam, CamTrans *ct, float mapCenterX, float mapCenterZ, float maxDim){
    float radius = maxDim * 1.4f;

    float angle = GetRandomValue(0, 360) * DEG2RAD;
    float height = GetRandomValue(60, 140) / 5.0f;

    ct->startPos = cam->position;
    ct->targetPos = (Vector3){mapCenterX + cosf(angle) * radius, height, mapCenterZ + sinf(angle) * radius};

    ct->t = 0.0f;
    ct->duration = 100.9f; 
    ct->active = true;
}

