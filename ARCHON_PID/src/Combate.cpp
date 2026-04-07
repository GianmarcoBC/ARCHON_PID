#include "Combate.h"

void Combate::Update()
{
    float dt = GetFrameTime();

    //Player 1
    P1.Update(dt);
    if (IsKeyPressed(KEY_SPACE)) {
       Disparos_1.push_back(P1.Shoot()); // Agrega un nuevo disparo al vector
    }

    //Gestión de los disparos del Player 1
    for (auto& b : Disparos_1) {
        b.Update();
        if (abs(b.GetPos().x - P2.GetPos().x) <= 32 && abs(b.GetPos().y - P2.GetPos().y) <= 32) {
            P2.pain(P1.GetFuerza()); // Aplica daño al Player 2
            b.setStatus(false); // Desactiva el disparo al colisionar
        }
    }

    //Player 2
    P2.Update(dt);
    if (IsKeyPressed(KEY_RIGHT_CONTROL)) {
        Disparos_2.push_back(P2.Shoot()); // Agrega un nuevo disparo al vector
    }

    //Gestión de los disparos del Player 2
    for (auto& b : Disparos_2) {
        b.Update();
        if (abs(b.GetPos().x - P1.GetPos().x) <= 32 && abs(b.GetPos().y - P1.GetPos().y) <= 32) {
            P1.pain(P2.GetFuerza()); // Aplica daño al Player 1
            b.setStatus(false); // Desactiva el disparo al colisionar
        }

    }
}

void Combate::Draw()
{
    //Gana jugador 2
    if (P1.GetVida() == 0) {
        DrawTexturePro(
            GameOver,
            { 0, 0, (float)GameOver.width, (float)GameOver.height }, // fuente: sprite completo
            { 0, 0, (float)GetScreenWidth(), (float)GetScreenHeight() }, // destino: pantalla completa
            { 0, 0 },
            0.0f,
            WHITE
        );       
        DrawText("Player 2 Wins!", 350, 300, 40, RED);
    
    }

    //Gana jugador 1
    else if (P2.GetVida() == 0) {
        DrawTexturePro(
            GameOver,
            { 0, 0, (float)GameOver.width, (float)GameOver.height }, // fuente: sprite completo
            { 0, 0, (float)GetScreenWidth(), (float)GetScreenHeight() }, // destino: pantalla completa
            { 0, 0 },
            0.0f,
            WHITE
        );          
        DrawText("Player 1 Wins!", 350, 300, 40, RED);
    }

    //Juego en curso
    else {
        DrawTexturePro(
            Fondo,
            { 0, 0, (float)Fondo.width, (float)Fondo.height }, // fuente: sprite completo
            { 0, 0, (float)GetScreenWidth(), (float)GetScreenHeight()}, // destino: pantalla completa
            { 0, 0 },
            0.0f,
            WHITE
        );     
        
        P1.Draw();
        P2.Draw();


        for (auto& b : Disparos_1) {
            b.Draw();
        }

        for (auto& b : Disparos_2) {
            b.Draw();
        }
    }
    
}
