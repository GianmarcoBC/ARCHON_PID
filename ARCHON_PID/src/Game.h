#pragma once
#include "Archer.h"
#include "Disparo.h"
#include <vector>
class Game
{
    
    Archer A1{ {100.0f, 400.0f} }; // Posición inicial del arquero
    Texture2D Fondo = LoadTexture("bin/Resources/Graficos/Sprites/Fondo.png");
    std::vector<Disparo> Disparos{}; // Vector para almacenar los disparos activos
public:
    Game() = default;
    void Update();
    void Draw();
    ~Game() {
        UnloadTexture(Fondo);
    }
};

