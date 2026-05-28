#pragma once
#include "raylib.h"
#include "Personaje.h"
#include "Arena.h"
#include "AI_Facil.h"
#include "AI_Defensiva.h"
#include "AI_Agresiva.h"
#include "Interacciones.h"
#include "Disparo.h"
#include <vector>

class ControladorCombate
{
    Camera3D camera = { { 0.0f, 25.0f, 35.0f },
                        { 0.0f, 0.0f,  0.0f },
                        { 0.0f, 1.0f,  0.0f },
                        45.0f, CAMERA_PERSPECTIVE };

    Arena arena;
    Personaje P1, P2;

    std::vector<obstaculo*> obstaculos{};
    CombatAI*      ia = nullptr;
    std::vector<Disparo> Disparos_1{};
    std::vector<Disparo> Disparos_2{};
    Shader alphaDiscard{};

    void Draw3D();
    void DrawVictory();

    friend class Interacciones;

public:
    ControladorCombate(Pj_info pj1, Pj_info pj2, bool vsIA, int dificultad);

    void Update();
    void Draw();

    bool IsGameOver() const { return P1.GetVida() == 0 || P2.GetVida() == 0; }

    int  GetWinner()  const {
        if (P2.GetVida() == 0) return 1;
        if (P1.GetVida() == 0) return 2;
        return 0;
    }

    std::string_view GetP1Name() const { return P1.GetNombre(); }
    std::string_view GetP2Name() const { return P2.GetNombre(); }

    ~ControladorCombate();
};
