#pragma once
#include "Personaje.h"
#include "Disparo.h"
#include "Pj.h"
#include <vector>
class Combate
{
    Personaje P1;
    Personaje P2;

    Texture2D Fondo = LoadTexture("bin/Resources/Graficos/Sprites/Fondo.png");
    Texture2D GameOver = LoadTexture("bin/Resources/Graficos/Sprites/26-267686_tortugas-ninja-png-imagenes-de-tortugas-ninja-png.png"); 

    std::vector<Disparo> Disparos_1{}; // Vector para almacenar los disparos activos del primer personaje
    std::vector<Disparo> Disparos_2{}; // Vector para almacenar los disparos activos del segundo personaje

public:
    Combate(Pj pj1, Pj pj2)
        : P1{ pj1, {KEY_W, KEY_S, KEY_A, KEY_D}, {100.0f, 100.0f} }
        , P2{ pj2, {KEY_UP, KEY_DOWN, KEY_LEFT, KEY_RIGHT}, {800.0f, 100.0f} }
    {}

    void Update();
    void Draw();

    ~Combate() {
        UnloadTexture(Fondo);
        UnloadTexture(GameOver);
    }
};

