
#include "/Users/amezioun/.brew/include/raylib.h"
#include <stdio.h>
#include <stdlib.h>
#define T_MAX_WIDTH 50
#define T_MAX_HEIGHT 50
#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600

typedef struct{
    int width;
    int height;
    int tiles[T_MAX_HEIGHT][T_MAX_WIDTH];
    Vector3 startPos;
} Map;

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
            else map.tiles[row][col] = 0;

            if (line[col] == 'S') {
                map.startPos = (Vector3){ (float)col, 0.5f, (float)row };
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
    for (int z = 0; z < map.height; z++){
        for (int x = 0; x < map.width; x++){
            if (map.tiles[z][x] == 1){
                Vector3 position = { x * tileSize, 0.0f, z * tileSize };
                DrawCube(position, tileSize, 0.5f, tileSize, LIGHTGRAY);
                DrawCubeWires(position, tileSize, 0.5f, tileSize, GRAY);
            }
        }
    }
}

int main(void){
    Map map = loadmap("level1.txt");

    Camera3D cam = { 0 };
    cam.position = (Vector3){10.0f, 10.0f, 10.0f };
    cam.target = (Vector3){ map.width / 2.0f, 0.0f, map.height / 2.0f };
    cam.up       = (Vector3){ 0.0f, 1.0f, 0.0f };
    cam.fovy     = 45.0f;

    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "testing");
    SetTargetFPS(60);



    while(!WindowShouldClose()){
        UpdateCamera(&cam, CAMERA_FIRST_PERSON);
        BeginDrawing();
        ClearBackground(BLACK);
        BeginMode3D(cam);



        drawMap(map, 1.0f);
        

        EndMode3D();
        DrawText("zaba w chta saba dlsfjklsafjd", 10, 10, 20, BLACK);
        EndDrawing();
    }
}