#include "Pereza.h"

int main()
{

    InitWindow(970, 580, "Shooter Test");
    SetTargetFPS(60);
    InitAudioDevice();

    Combate game;
    Music musica = LoadMusicStream("bin/Resources/Audio/Musica/MusicaBatalla1V1.mp3");  // archivo en bin/
    PlayMusicStream(musica);


    while (!WindowShouldClose()) {
        game.Update();
        UpdateMusicStream(musica);  // necesario llamarlo cada frame

        BeginDrawing();
        ClearBackground(RAYWHITE);

        game.Draw();

        EndDrawing();
    }

    UnloadMusicStream(musica);
    CloseAudioDevice();
    CloseWindow();
    return 0;

}
