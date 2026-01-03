#include "./inc/Define.h"

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
    Music bgm = LoadMusicStream("./assets/theme.mp3");
    bgm.looping = true;
    SetMusicVolume(bgm, 0.2f);
    PlayMusicStream(bgm);


    // Sound move = LoadSound("/Users/amezioun/Desktop/bloxorz/assets/move.wav");
    // SetSoundVolume(move, 0.3f);

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
    // bool showFinalStats = false;

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
                
                if(IsKeyPressed(KEY_N)){
                    // PlaySound(move);
                    won = true;
                }
                if (IsKeyPressed(KEY_UP)){
                    // PlaySound(move);
                    res = movebox(&block, &map, 0, -1);
                    moved = true;
                }
                else if (IsKeyPressed(KEY_DOWN)){
                    // PlaySound(move);
                    res = movebox(&block, &map, 0, 1);
                    moved = true;
                }
                else if (IsKeyPressed(KEY_LEFT)) {
                    res = movebox(&block, &map, -1, 0);
                    // PlaySound(move);
                    moved = true;
                }
                else if (IsKeyPressed(KEY_RIGHT)) {
                    // PlaySound(move);
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
                // if (currlev >= LEVEL_COUNT) {
                    // showFinalStats = true;
                    // state = STATE_FINAL;
                    // currlev = 0;
                // } else {
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
                // }
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
                // showFinalStats = false;
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
    // UnloadSound(move);
    CloseAudioDevice();
    CloseWindow();
    return 0;
}