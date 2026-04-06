#pragma once
#include "Personaje.h"
#include "Disparo.h"
#include "Pj.h"
#include <vector>
class Combate
{
    
    Personaje P1{ Golem, {KEY_W, KEY_S, KEY_A, KEY_D}, {100.0, 100.0}};
    Personaje P2{ Platero, {KEY_UP, KEY_DOWN, KEY_LEFT, KEY_RIGHT}, {800.0, 100.0} };
    Texture2D Fondo = LoadTexture("bin/Resources/Graficos/Sprites/Fondo.png");
    std::vector<Disparo> Disparos_1{}; // Vector para almacenar los disparos activos del primer personaje
    std::vector<Disparo> Disparos_2{}; // Vector para almacenar los disparos activos del segundo personaje
public:
    Combate() = default;
    void Update();
    void Draw();
    ~Combate() {
        UnloadTexture(Fondo);
    }
};

