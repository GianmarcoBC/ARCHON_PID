#pragma once
#include "Personaje.h"
#include "CombatAI.h"
#include "Disparo.h"
#include "Pj.h"
#include "Obstaculo.h"
#include "Interacciones.h"
#include "SaveManager.h"
#include <vector>

class Combate
{
    Personaje P1;
    Personaje P2;

    // Solo existe en modo IA
    CombatAI* ia = nullptr;

    Texture2D Fondo{};
    Texture2D GameOver{};
    Texture2D ObsSolido{};
    Texture2D ObsDecor{};

    std::vector<Disparo>   Disparos_1{};
    std::vector<Disparo>   Disparos_2{};
    std::vector<Obstaculo> Obstaculos{};

    float cooldown1 = 0.0f, cooldown2 = 0.0f; // Tiempo de recarga para evitar disparos continuos

    Interacciones interaccion{ { 0, 0, (float)GetScreenWidth(), (float)GetScreenHeight() } };

    void GenerarObstaculos();

public:

    // ── Modo 2 jugadores ──────────────────────────────────────────────────────
    Combate(Pj pj1, Pj pj2)
        : P1{ pj1, {KEY_W, KEY_S, KEY_A, KEY_D},         {100.0f, 100.0f}, true }
        , P2{ pj2, {KEY_UP, KEY_DOWN, KEY_LEFT, KEY_RIGHT},{800.0f, 100.0f}, true }
    {
        Fondo = LoadTexture("bin/Resources/Graficos/Sprites/Fondo.png");
        GameOver = LoadTexture("bin/Resources/Graficos/Sprites/26-267686_tortugas-ninja-png-imagenes-de-tortugas-ninja-png.png");
        ObsSolido = LoadTexture("bin/Resources/Graficos/Sprites/Arbusto.png");
        ObsDecor = LoadTexture("bin/Resources/Graficos/Sprites/Arbusto.png");
        GenerarObstaculos();
    }

    // ── Modo vs IA ────────────────────────────────────────────────────────────
    Combate(Pj pj1, Pj pj2, int dificultad)
        : P1{ pj1, {KEY_W, KEY_S, KEY_A, KEY_D}, {100.0f, 100.0f}, true }
        , P2{ pj2, {}, {800.0f, 100.0f}, false }   // isPlayer = false
    {
        Fondo = LoadTexture("bin/Resources/Graficos/Sprites/Fondo.png");
        GameOver = LoadTexture("bin/Resources/Graficos/Sprites/26-267686_tortugas-ninja-png-imagenes-de-tortugas-ninja-png.png");
        ObsSolido = LoadTexture("bin/Resources/Graficos/Sprites/Arbusto.png");
        ObsDecor = LoadTexture("bin/Resources/Graficos/Sprites/Arbusto.png");
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

    // ── Guardado / Carga ──────────────────────────────────────────────────
    // Rellena un SaveData con el estado actual del combate y lo escribe al disco.
    // Devuelve true si la escritura tuvo éxito.
    bool GuardarEstado(bool modoIA, int dificultad) const
    {
        SaveData d;
        d.nombreP1   = P1.GetNombre();
        d.vidaP1     = P1.GetVida();
        d.posP1x     = P1.GetPos().x;
        d.posP1y     = P1.GetPos().y;
        d.nombreP2   = P2.GetNombre();
        d.vidaP2     = P2.GetVida();
        d.posP2x     = P2.GetPos().x;
        d.posP2y     = P2.GetPos().y;
        d.modoIA     = modoIA;
        d.dificultad = dificultad;
        return SaveManager::Guardar(d);
    }

    // Restaura vida y posición de ambos personajes desde un SaveData cargado.
    void CargarEstado(const SaveData& d)
    {
        P1.SetVida(d.vidaP1);
        P1.SetPos({ d.posP1x, d.posP1y });
        P2.SetVida(d.vidaP2);
        P2.SetPos({ d.posP2x, d.posP2y });
    }

    ~Combate() {
        delete ia;
        UnloadTexture(Fondo);
        UnloadTexture(GameOver);
        UnloadTexture(ObsSolido);
        UnloadTexture(ObsDecor);
    }
};