#include "Combate.h"

void Combate::Update()
{
    float dt = GetFrameTime();

    //Player 1
    P1.Update(dt);
    if (IsKeyPressed(KEY_SPACE)) {
       Disparos_1.push_back(P1.Shoot()); // Agrega un nuevo disparo al vector
    }

    for (auto& b : Disparos_1) {
        b.Update();

    }

    //Player 2
    P2.Update(dt);
    if (IsKeyPressed(KEY_RIGHT_CONTROL)) {
        Disparos_2.push_back(P2.Shoot()); // Agrega un nuevo disparo al vector
    }

    for (auto& b : Disparos_2) {
        b.Update();

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
