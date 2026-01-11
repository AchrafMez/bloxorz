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

void DrawDiffMenu(GameMode *mode){
    float swidth = (float)GetScreenWidth();
    float sheight = (float)GetScreenHeight();
    float centerX = swidth * 0.5f;

    int Tsize = (int)(sheight * 0.04f);
    int mdsize  = (int)(sheight * 0.035f);
    int descSize  = (int)(sheight * 0.025f);

    float ttdesc = sheight * 0.048f;
    float descgap = sheight * 0.035f;
    float blockgap= sheight * 0.065f;

    const char *title = "SELECT DIFFICULTY";
    DrawText(title, centerX - MeasureText(title, Tsize) / 2, sheight * 0.08f, Tsize, YELLOW);

    float textStartX = swidth * 0.25f; 
    float currentY   = sheight * 0.25f; 

    Color normalColor = (mode->Diff == NORMAL) ? YELLOW : WHITE;
    DrawText("1. NORMAL MODE", (int)textStartX, (int)currentY, mdsize, normalColor);

    currentY += ttdesc;
    DrawText("- No restrictions", (int)textStartX, (int)currentY, descSize, LIGHTGRAY);
    currentY += descgap;
    DrawText("- Full camera control", (int)textStartX, (int)currentY, descSize, LIGHTGRAY);
    currentY += descgap;
    DrawText("- Take your time", (int)textStartX, (int)currentY, descSize, LIGHTGRAY);

    currentY += blockgap;

    Color mediumColor = (mode->Diff == MEDIUM) ? YELLOW : WHITE;
    DrawText("2. MEDIUM MODE", (int)textStartX, (int)currentY, mdsize, mediumColor);

    currentY += ttdesc;
    DrawText("- Time and move limits", (int)textStartX, (int)currentY, descSize, ORANGE);
    currentY += descgap;
    DrawText("- Faster block movement", (int)textStartX, (int)currentY, descSize, ORANGE);
    currentY += descgap;
    DrawText("- Full camera control", (int)textStartX, (int)currentY, descSize, LIGHTGRAY);

    currentY += blockgap;

    Color hardColor = (mode->Diff == HARD) ? YELLOW : WHITE;
    DrawText("3. HARD MODE", (int)textStartX, (int)currentY, mdsize, hardColor);

    currentY += ttdesc;
    DrawText("- Tight time and move limits", (int)textStartX, (int)currentY, descSize, RED);
    currentY += descgap;
    DrawText("- Faster block movement", (int)textStartX, (int)currentY, descSize, RED);
    currentY += descgap;
    DrawText("- NO camera control", (int)textStartX, (int)currentY, descSize, RED);
    currentY += descgap;
    DrawText("- Random camera angle on each move!", (int)textStartX, (int)currentY, descSize, RED);

    const char *use = "Use 1, 2, 3 to select | Press ENTER to start";
    DrawText(use, centerX - MeasureText(use, descSize) / 2, sheight * 0.88f, descSize, GREEN);
}



void drawPauseMen(void){
    float screenWidth = (float)GetScreenWidth();
    float screenHeight = (float)GetScreenHeight();
    float centerX = screenWidth / 2.0f;
    
    int tSzie = (int)(screenHeight * 0.04f);
    int secSize = (int)(screenHeight * 0.035f);
    int textSize = (int)(screenHeight * 0.025f);
    int startSize = (int)(screenHeight * 0.025f);
    
    const char *title = "WELCOME TO BLOXORZ";
    DrawText(title, centerX - MeasureText(title, tSzie) / 2, screenHeight * 0.08f, tSzie, YELLOW);
    
    float textst = centerX - (screenWidth * 0.25f);
    float currentY = screenHeight * 0.25f;
    
    const char *ruleSize = "How to Play:";
    DrawText(ruleSize, textst, currentY, secSize, RAYWHITE);
    
    currentY += screenHeight * 0.07f;
    DrawText("Arrows - Move the block", textst, currentY, textSize, LIGHTGRAY);
    
    currentY += screenHeight * 0.045f;
    DrawText("W A S D + Mouse - Move camera", textst, currentY, textSize, LIGHTGRAY);
    
    currentY += screenHeight * 0.045f;
    DrawText("R - Reset level    |    P - Pause/Menu", textst, currentY, textSize, LIGHTGRAY);
    
    currentY += screenHeight * 0.045f;
    DrawText("M - Pause or Play Song", textst, currentY, textSize, LIGHTGRAY);

    currentY += screenHeight * 0.08f;
    const char *tilesTitle = "Tile Types:";
    DrawText(tilesTitle, textst, currentY, secSize, RAYWHITE);
    
    currentY += screenHeight * 0.07f;
    float squareSize = screenHeight * 0.035f;
    float textOffset = squareSize + (screenWidth * 0.015f);
    
    DrawRectangle(textst, currentY, squareSize, squareSize, GREEN);
    DrawText("Target - Stand on this vertically to win", textst + textOffset, currentY + (squareSize * 0.15f), textSize, LIGHTGRAY);
    
    currentY += screenHeight * 0.055f;
    DrawRectangle(textst, currentY, squareSize, squareSize, ORANGE);
    DrawText("Weak Tile - Cannot stand vertically here", textst + textOffset, currentY + (squareSize * 0.15f), textSize, LIGHTGRAY);
    
    currentY += screenHeight * 0.055f;
    DrawRectangle(textst, currentY, squareSize, squareSize, SKYBLUE);
    DrawText("Soft Switch - Activates bridges | Can stand any way", textst + textOffset, currentY + (squareSize * 0.15f), textSize, LIGHTGRAY);

    currentY += screenHeight * 0.055f;
    DrawRectangle(textst, currentY, squareSize, squareSize, DARKBLUE);
    DrawText("Hard Switch - Activates bridges | Must stand vertically", textst + textOffset, currentY + (squareSize * 0.15f), textSize, LIGHTGRAY);
    
    const char *startText = "Press P to resume";
    DrawText(startText, centerX - MeasureText(startText, startSize) / 2, screenHeight * 0.88f, startSize, GREEN);
}

void drawMen(void){
    float screenWidth = (float)GetScreenWidth();
    float screenHeight = (float)GetScreenHeight();
    float centerX = screenWidth / 2.0f;
    
    int tSzie = (int)(screenHeight * 0.04f);
    int secSize = (int)(screenHeight * 0.035f);
    int textSize = (int)(screenHeight * 0.025f);
    int startSize = (int)(screenHeight * 0.025f);
    
    const char *title = "WELCOME TO BLOXORZ";
    DrawText(title, centerX - MeasureText(title, tSzie) / 2, screenHeight * 0.08f, tSzie, YELLOW);
    
    float textst = centerX - (screenWidth * 0.25f);
    float currentY = screenHeight * 0.25f;
    
    const char *ruleSize = "How to Play:";
    DrawText(ruleSize, textst, currentY, secSize, RAYWHITE);
    
    currentY += screenHeight * 0.07f;
    DrawText("Arrows - Move the block", textst, currentY, textSize, LIGHTGRAY);
    
    currentY += screenHeight * 0.045f;
    DrawText("W A S D + Mouse - Move camera", textst, currentY, textSize, LIGHTGRAY);
    
    currentY += screenHeight * 0.045f;
    DrawText("R - Reset level    |    P - Pause/Menu", textst, currentY, textSize, LIGHTGRAY);
    
    currentY += screenHeight * 0.045f;
    DrawText("M - Pause or Play Song", textst, currentY, textSize, LIGHTGRAY);

    currentY += screenHeight * 0.08f;
    const char *tilesTitle = "Tile Types:";
    DrawText(tilesTitle, textst, currentY, secSize, RAYWHITE);
    
    currentY += screenHeight * 0.07f;
    float squareSize = screenHeight * 0.035f;
    float textOffset = squareSize + (screenWidth * 0.015f);
    
    DrawRectangle(textst, currentY, squareSize, squareSize, GREEN);
    DrawText("Target - Stand on this vertically to win", textst + textOffset, currentY + (squareSize * 0.15f), textSize, LIGHTGRAY);
    
    currentY += screenHeight * 0.055f;
    DrawRectangle(textst, currentY, squareSize, squareSize, ORANGE);
    DrawText("Weak Tile - Cannot stand vertically here", textst + textOffset, currentY + (squareSize * 0.15f), textSize, LIGHTGRAY);
    
    currentY += screenHeight * 0.055f;
    DrawRectangle(textst, currentY, squareSize, squareSize, SKYBLUE);
    DrawText("Soft Switch - Activates bridges | Can stand any way", textst + textOffset, currentY + (squareSize * 0.15f), textSize, LIGHTGRAY);

    currentY += screenHeight * 0.055f;
    DrawRectangle(textst, currentY, squareSize, squareSize, DARKBLUE);
    DrawText("Hard Switch - Activates bridges | Must stand vertically", textst + textOffset, currentY + (squareSize * 0.15f), textSize, LIGHTGRAY);
    
    const char *startText = "Press ENTER to Select Game Mode";
    DrawText(startText, centerX - MeasureText(startText, startSize) / 2, screenHeight * 0.88f, startSize, GREEN);
}

void DrawStatsHUD(GameStats stats, GameMode mode, int currlev) {
    char timeText[64];
    char moveText[64];
    const char *diffText = "";
    Color diffColor;
    char levT[32];
    snprintf(levT, sizeof(levT), "Level %d", currlev);
    DrawText(levT, 25, 10, 22, WHITE);

    int screenWidth = GetScreenWidth();

    switch (mode.Diff){
        case NORMAL:
            diffText = "NORMAL MODE";
            diffColor = YELLOW;
            break;
        case MEDIUM:
            diffText = "MEDIUM MODE";
            diffColor = ORANGE;
            break;
        case HARD:
            diffText = "HARD MODE";
            diffColor = RED;
            break;
    }

    DrawText(diffText, screenWidth - 250, 15, 22, diffColor);

    if (mode.Diff == NORMAL){
        snprintf(moveText, sizeof(moveText), "Moves: %d", stats.currentMoves);
        snprintf(timeText, sizeof(timeText), "Time: %.1fs", stats.currentTime);

        DrawText(moveText, screenWidth - 250, 50, 20, YELLOW);
        DrawText(timeText, screenWidth - 250, 75, 20, YELLOW);
        return;
    }

    const LevLimits *constraints = (mode.Diff == MEDIUM) ? &mediumMode[currlev] : &hardMode[currlev];

    int movesLeft = constraints->moveLimit - stats.currentMoves;
    Color moveColor = (movesLeft <= 5) ? RED : (movesLeft <= 10) ? ORANGE :WHITE;
    snprintf(moveText, sizeof(moveText), "Moves: %d / %d", stats.currentMoves, constraints->moveLimit);
    DrawText(moveText, screenWidth - 250, 50, 20, moveColor);

    float timeLeft = constraints->timeLimit - stats.currentTime;
    Color timeColor = (timeLeft <= 10.0f) ? RED : (timeLeft <= 20.0f) ?ORANGE : WHITE;
    snprintf(timeText, sizeof(timeText), "Time: %.1f / %.1f", stats.currentTime, constraints->timeLimit);
    DrawText(timeText, screenWidth - 250, 80, 20, timeColor);
}


void DrawFinalStatsScreen(GameStats stats) {
    int screenWidth = GetScreenWidth();
    int screenHeight = GetScreenHeight();
    
    DrawRectangle(0, 0, screenWidth, screenHeight, (Color){0, 0, 0, 200});
    
    const char *title = "CONGRATULATIONS! ALL LEVELS COMPLETED!";
    int tSzie = 30;
    DrawText(title, (screenWidth - MeasureText(title, tSzie)) / 2, 50, tSzie, GOLD);
    
    const char *statsHeader = "YOUR PERFORMANCE:";
    int headerSize = 25;
    DrawText(statsHeader, (screenWidth - MeasureText(statsHeader, headerSize)) / 2, 100, headerSize, WHITE);
    
    int bestTotalMoves = 0;
    float bestTotalTime = 0.0f;
    
    for (int i = 0; i < LEVEL_COUNT; i++){
        if (stats.levels[i].moves != -1){
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
        } else{
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
