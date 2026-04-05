#include "Combate.h"

void Combate::Update()
{
    float dt = GetFrameTime();

    P1.Update(dt);
    if (IsKeyPressed(KEY_SPACE)) {
       Disparos.push_back(P1.Shoot()); // Agrega un nuevo disparo al vector
    }

    for (auto& b : Disparos) {
        b.Update();
    }

    // Elimina los disparos inactivos
    
}

void Combate::Draw()
{
    DrawTexture(Fondo, 0, 0, WHITE);
    P1.Draw();
    

    for (auto& b : Disparos) {
        b.Draw();
    }
}
