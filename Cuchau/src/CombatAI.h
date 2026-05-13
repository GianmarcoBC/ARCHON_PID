#pragma once
#include "Personaje.h"
#include "Disparo.h"
#include "Vec2.h"
#include "raylib.h"
#include <cmath>
#include <cstdlib>
#include <vector>

// ============================================================================
//  CombatAI.h — Inteligencia artificial para el combate
//
//  Maquina de estados con 3 comportamientos:
//    Acercar  — Se acerca al jugador con movimiento en zigzag
//    Rodear   — Orbita al jugador a distancia optima de combate
//    Esquivar — Esquiva perpendicular al detectar un proyectil cercano
//
//  Opera completamente en el plano XZ usando Vec2.
//  Tiene un delay de reaccion configurable (dificultad).
// ============================================================================

class CombatAI {

    // Estados de la maquina de estados
    enum Estado { Acercar, Rodear, Esquivar };

    // Resultado de una decision: direccion de movimiento + si debe disparar
    struct Accion {
        Vec2 d;          // Direccion de movimiento (no normalizada)
        bool disparar;   // true = la IA quiere disparar este frame
    };

    // --- Referencias a los personajes ---
    Personaje& Jugador;   // Referencia al jugador humano (objetivo)
    Personaje& IA;        // Referencia al personaje controlado por la IA
    Rectangle  arena;     // Limites XZ de la arena: {minX, minZ, anchoX, largoZ}

    // --- Umbrales de distancia ---
    float distancia_combate_optima;  // Distancia ideal para orbitar (~38% del lado corto)
    float rango_seguro;              // Margen alrededor de la distancia optima (25%)
    float rango_esquiva;             // Distancia minima a un proyectil para activar esquiva (2.0 unidades)
    float aimbot;                    // Umbral de precision para disparar (0.93 = cos ~21°)

    // --- Estado actual ---
    Estado estado;           // Estado actual de la maquina de estados
    float  sentido_giro;     // +1 o -1, sentido en que la IA orbita al jugador
    float  cont_giro;        // Contador de tiempo para cambiar sentido de giro
    float  cambio_giro;      // Tiempo (frames) hasta el proximo cambio de giro

    // --- Buffer de reaccion (simula tiempo de reaccion humano) ---
    Accion ultima_accion;    // Ultima decision tomada (se mantiene durante el delay)
    float  cooldown_IA;      // Tiempo restante hasta la proxima decision
    float  delay_IA;         // Intervalo entre decisiones (menor = mas dificil)

    // --- Metodos auxiliares ---

    // Devuelve la distancia a la pared mas cercana desde una posicion
    float Pared_cerc(Vec2 pos) const;

    // Modifica un vector de movimiento para evitar acercarse demasiado a las paredes
    void  evitar_pared(Vec2& v, Vec2 pos, Vec2 lim_x, Vec2 lim_y, float margin);

    // Revisa todos los disparos del jugador y determina si hay que esquivar alguno
    // Si hay que esquivar, outDir recibe la direccion perpendicular de escape
    bool  debe_esquivar(const std::vector<Disparo>& disparos, Vec2& outDir);

    // Toma la decision de movimiento y disparo segun el estado actual
    Accion decide(const std::vector<Disparo>& disparos, float dt);

public:
    // Constructor: recibe al personaje IA, al jugador objetivo, y los limites de la arena
    CombatAI(Personaje& IA, Personaje& Jugador, Rectangle arena)
        : IA(IA), Jugador(Jugador), arena(arena),
          estado(Acercar), sentido_giro(1.0f), cont_giro(0.0f),
          ultima_accion{ {0, 0}, false }, cooldown_IA(0.0f), delay_IA(0.0f)
    {
        // Calcular distancia optima de combate (38% del lado corto de la arena)
        float lado_corto = fminf(arena.width, arena.height);
        distancia_combate_optima = lado_corto * 0.38f;
        rango_seguro   = distancia_combate_optima * 0.25f;
        rango_esquiva  = 2.0f;    // 2 unidades (proporcional al charSize de 4)
        aimbot         = 0.93f;   // cos(~21°) — dispara si apunta con ±21° de error
        cambio_giro    = 60.0f * (1.8f + (rand() % 100) / 72.0f);  // Tiempo aleatorio para cambiar orbita
        delay_IA       = 60.0f * 0.12f;  // Delay por defecto (~120ms a 60fps)
    }

    // Actualiza la IA: mueve al personaje y devuelve true si debe disparar
    bool Update(float dt, const std::vector<Disparo>& disparos);

    // Cambia el delay de reaccion (para ajustar dificultad)
    void SetReactionDelay(float frames) { delay_IA = frames; }
};
