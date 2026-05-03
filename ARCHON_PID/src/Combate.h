#pragma once
#include "Personaje.h"
#include "CombatAI.h"
#include "Disparo.h"
#include "Pj.h"
#include "Obstaculo.h"
#include "Interacciones.h"
#include <vector>

class Combate
{
    Personaje P1;
    Personaje P2;

    // Solo existe en modo IA
    CombatAI* ia = nullptr;

    Texture2D Fondo = LoadTexture("bin/Resources/Graficos/Sprites/Fondo.png");
    Texture2D GameOver = LoadTexture("bin/Resources/Graficos/Sprites/26-267686_tortugas-ninja-png-imagenes-de-tortugas-ninja-png.png");
    Texture2D ObsSolido = LoadTexture("bin/Resources/Graficos/Sprites/Arbusto.png");
    Texture2D ObsDecor = LoadTexture("bin/Resources/Graficos/Sprites/Arbusto.png");

    std::vector<Disparo>   Disparos_1{};
    std::vector<Disparo>   Disparos_2{};
    std::vector<Obstaculo> Obstaculos{};

    Interacciones interaccion{ { 0, 0, (float)GetScreenWidth(), (float)GetScreenHeight() } };

    void GenerarObstaculos() {
        for (int i = 0; i < 10; i++) {
            bool esSolido = (GetRandomValue(0, 1) == 1);
            Texture2D* tx = esSolido ? &ObsSolido : &ObsDecor;
            float x = (float)GetRandomValue(100, GetScreenWidth() - 100);
            float y = (float)GetRandomValue(100, GetScreenHeight() - 100);
            Obstaculos.push_back({ tx, { x, y, (float)tx->width, (float)tx->height }, esSolido });
        }
    }

public:

    // ── Modo 2 jugadores ──────────────────────────────────────────────────────
    Combate(Pj pj1, Pj pj2)
        : P1{ pj1, {KEY_W, KEY_S, KEY_A, KEY_D},         {100.0f, 100.0f}, true }
        , P2{ pj2, {KEY_UP, KEY_DOWN, KEY_LEFT, KEY_RIGHT},{800.0f, 100.0f}, true }
    {
        GenerarObstaculos();
    }

    // ── Modo vs IA ────────────────────────────────────────────────────────────
    Combate(Pj pj1, Pj pj2, int dificultad)
        : P1{ pj1, {KEY_W, KEY_S, KEY_A, KEY_D}, {100.0f, 100.0f}, true }
        , P2{ pj2, {}, {800.0f, 100.0f}, false }   // isPlayer = false
    {
        GenerarObstaculos();

        Rectangle arena = { 0, 0, (float)GetScreenWidth(), (float)GetScreenHeight() };
        ia = new CombatAI(P2, P1, arena);

        // Ajustar reacción según dificultad: 0=fácil 1=normal 2=difícil
        float delays[3] = { 60.0f * 0.25f,   // fácil  (~250 ms)
                            60.0f * 0.12f,   // normal (~120 ms)
                            60.0f * 0.04f }; // difícil (~40 ms)
        ia->SetReactionDelay(delays[dificultad]);
    }

    void Update();
    void Draw();
    bool IsGameOver() const { return P1.GetVida() == 0 || P2.GetVida() == 0; }

    ~Combate() {
        delete ia;
        UnloadTexture(Fondo);
        UnloadTexture(GameOver);
        UnloadTexture(ObsSolido);
        UnloadTexture(ObsDecor);
    }
};