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
    DrawTexture(Fondo, 0, 0, WHITE);
    P1.Draw();
    P2.Draw();
    

    for (auto& b : Disparos_1) {
        b.Draw();
    }

    for (auto& b : Disparos_2) {
        b.Draw();
    }
}
