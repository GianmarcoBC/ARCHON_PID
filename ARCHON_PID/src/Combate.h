#pragma once
#include "Personaje.h"
#include "Disparo.h"
#include "Pj.h"
#include "Obstaculo.h"
#include <vector>
class Combate
{
    Personaje P1;
    Personaje P2;

    Texture2D Fondo = LoadTexture("bin/Resources/Graficos/Sprites/Fondo.png");
    Texture2D GameOver = LoadTexture("bin/Resources/Graficos/Sprites/26-267686_tortugas-ninja-png-imagenes-de-tortugas-ninja-png.png"); 
    Texture2D ObsSolido = LoadTexture("bin/Resources/Graficos/Sprites/Arbusto.png");
    Texture2D ObsDecor = LoadTexture("bin/Resources/Graficos/Sprites/Arbusto.png");

    std::vector<Disparo> Disparos_1{}; // Vector para almacenar los disparos activos del primer personaje
    std::vector<Disparo> Disparos_2{}; // Vector para almacenar los disparos activos del segundo personaje
    std::vector<Obstaculo> Obstaculos{}; // Vector para almacenar los obstáculos en el combate

public:
    Combate(Pj pj1, Pj pj2)
        : P1{ pj1, {KEY_W, KEY_S, KEY_A, KEY_D}, {100.0f, 100.0f} }
        , P2{ pj2, {KEY_UP, KEY_DOWN, KEY_LEFT, KEY_RIGHT}, {800.0f, 100.0f} }
    {
        // Genera 10 obstáculos en posiciones aleatorias
        for (int i = 0; i < 10; i++) {
            bool esSolido = (GetRandomValue(0, 1) == 1); // 50% sólido, 50% decorativo
            Texture2D* tx = esSolido ? &ObsSolido : &ObsDecor;

            float x = (float)GetRandomValue(100, GetScreenWidth() - 100);
            float y = (float)GetRandomValue(100, GetScreenHeight() - 100);

            Obstaculos.push_back({
                tx,
                { x, y, (float)tx->width, (float)tx->height },
                esSolido
                });
        }
    
    }

    void Update();
    void Draw();

    ~Combate() {
        UnloadTexture(Fondo);
        UnloadTexture(GameOver);
    }
};

