#include <raylib.h>

int main()
{
    InitWindow(800, 600, "Test Raylib");
    SetTargetFPS(60);
    InitAudioDevice();
    
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
        EndDrawing();
    }

    UnloadMusicStream(musica);
    CloseAudioDevice();
    CloseWindow();
    return 0;

    //prueba commit
}
