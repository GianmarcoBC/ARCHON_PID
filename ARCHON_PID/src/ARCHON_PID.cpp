#include "Pereza.h"

int main()
{

    InitWindow(0, 0, "Shooter Test");
    SetTargetFPS(60);
    InitAudioDevice();

    Menu_Combate menu;
    Combate* combate = nullptr; //Se crea solo cuando el menú termina

    Music musica = LoadMusicStream("bin/Resources/Audio/Musica/MusicaBatalla1V1.mp3");  // archivo en bin/
    PlayMusicStream(musica);


    while (!WindowShouldClose()) {

        UpdateMusicStream(musica);  // necesario llamarlo cada frame

        BeginDrawing();
        ClearBackground(RAYWHITE);


        if (combate == nullptr) {
            // Estamos en el menú
            if (menu.Update()) {
                // Los dos jugadores eligieron, creamos el combate
                combate = new Combate(menu.GetSelP1(), menu.GetSelP2());
            }
            menu.Draw();
        }
        else {
            // Estamos en el combate
            combate->Update();
            combate->Draw();
        }

        EndDrawing();
    }

    UnloadMusicStream(musica);
    CloseAudioDevice();
    CloseWindow();
    return 0;

}
