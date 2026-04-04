#include "Game.h"

void Game::Update()
{
    float dt = GetFrameTime();

    A1.Update(dt);
    if (IsKeyPressed(KEY_SPACE)) {
       Disparos.push_back(A1.Shoot()); // Agrega un nuevo disparo al vector
    }

    for (auto& b : Disparos) {
        b.Update();
    }

    // Elimina los disparos inactivos
    
}

void Game::Draw()
{
    DrawTexture(Fondo, 0, 0, WHITE);
    A1.Draw();
    

    for (auto& b : Disparos) {
        b.Draw();
    }
}
