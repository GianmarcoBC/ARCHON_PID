#include <raylib.h>
#include "Pereza.h"

int main()
{
    
    InitWindow(1600, 800, "Test Raylib");
    SetTargetFPS(60);
    InitAudioDevice();

    goblin A1;
    Music musica = LoadMusicStream("bin/Resources/Audio/Musica/MusicaBatalla1V1.mp3");  // archivo en bin/
    PlayMusicStream(musica);


    while (!WindowShouldClose()) {
        UpdateMusicStream(musica);  // necesario llamarlo cada frame
        BeginDrawing();
        ClearBackground(RAYWHITE);
        DrawText("Raylib funciona!", 300, 280, 20, DARKGRAY);

        if (musica.stream.buffer == NULL)
            DrawText("Error cargando musica", 300, 260, 20, RED);
        else
            DrawText("Musica cargada OK", 300, 260, 20, GREEN);

        if (A1.Sprite.id != 0)
            DrawTexture(A1.Sprite, 200, 100, WHITE);
        else
            DrawText("Error cargando imagen", 300, 240, 20, RED);


        EndDrawing();
    }

    UnloadMusicStream(musica);
	UnloadTexture(A1.Sprite);
    CloseAudioDevice();
    CloseWindow();
    return 0;

    //prueba commit 3
}
