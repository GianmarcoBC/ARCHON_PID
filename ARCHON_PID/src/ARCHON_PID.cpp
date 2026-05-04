#include "Pereza.h"
#include "Rolitas.h"

int main()
{

    InitWindow(0, 0, "Shooter Test");
    SetTargetFPS(60);
    InitAudioDevice();

    Menu_Combate menu;
    Combate* combate = nullptr; //Se crea solo cuando el menú termina
    Rolitas rolitas;

    while (!WindowShouldClose()) {

          // necesario llamarlo cada frame

        BeginDrawing();
        ClearBackground(RAYWHITE);

        rolitas.Update();

        

        if (combate == nullptr) {
            // Estamos en el menú
            if (menu.Update()) {
                if (menu.EsModoIA())
                    combate = new Combate(menu.GetSelP1(), menu.GetSelP2(), menu.GetDificultad());
                else
                    combate = new Combate(menu.GetSelP1(), menu.GetSelP2());
            }
            menu.Draw();
        }
        else {
            // Estamos en el combate      
            combate->Update();
            combate->Draw();
            
            rolitas.SetMusic(menu.GetSelP1().nombre, menu.GetSelP2().nombre, combate->IsGameOver());
        }
        
        EndDrawing();
    }
    
    delete combate;
    CloseAudioDevice();
    CloseWindow();
    return 0;

}
