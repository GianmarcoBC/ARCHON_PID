#pragma once
#include "Personaje.h"
#include "Disparo.h"
#include "Pj.h"
#include <vector>
class Combate
{
    
    Personaje P1{ Golem }; // Posición inicial del arquero
    Texture2D Fondo = LoadTexture("bin/Resources/Graficos/Sprites/Fondo.png");
    std::vector<Disparo> Disparos{}; // Vector para almacenar los disparos activos
public:
    Combate() = default;
    void Update();
    void Draw();
    ~Combate() {
        UnloadTexture(Fondo);
    }
};

