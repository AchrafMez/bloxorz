#include "../inc/Define.h"

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
    
    const char *controls = "Arrows - Move the block";
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
    const char *blue = "Soft Switch - Activates bridges | Can stand any way";
    DrawText(blue, squareX + squareSize + 20, start + 5, 20, LIGHTGRAY);

    start+=45;
    DrawRectangle(squareX, start, squareSize, squareSize, DARKBLUE);
    const char *darkblue = "Hard Switch - Activates bridges | Must stand vertically";
    DrawText(darkblue, squareX + squareSize + 20, start + 5, 20, LIGHTGRAY);
    
    start += 180;
    const char *startText = "Press ENTER to Start";
    int startSize = 32;
    DrawText(startText, centerX - MeasureText(startText, startSize) / 2, start, startSize, GREEN);
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
