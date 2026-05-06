#include "raylib.h"
#include "arena.h"

int main() 
{
    InitWindow(0, 0, "Tablero");
    SetTargetFPS(60);
 
    Pj pj("SpriteMH_0001.png", "SpriteMH_shadow.png");
    arena miarena("ciudadnormal2.png", "paredcalida.png", "suelo.png", pj, pj);

    // Frames de warm-up: deja estabilizarse la ventana antes de empezar
    for (int i = 0; i < 10; i++)
    {
        BeginDrawing();
        ClearBackground(BLACK);
        DrawText("Cargando...", GetScreenWidth() / 2 - 60, GetScreenHeight() / 2, 20, WHITE);
        EndDrawing();
    }

    //Codigo de dibujo
    while (!WindowShouldClose())
    {
        BeginDrawing();
        ClearBackground(BLACK);

        miarena.UpdateArena(pj);
        miarena.DibujaArena();

        EndMode3D();
        EndDrawing();
    }

    miarena.UnloadArena();
       
    CloseWindow();
    return 0;
}
