#pragma once
#include "Personaje.h"
#include "Disparo.h"
#include "Vec2.h"
#include "raylib.h"
#include <cmath>
#include <cstdlib>
#include <vector>
#include "Arena.h"

//  CombatAI.h — Inteligencia artificial para el combate
//
//  "Maquina de estados" con 3 comportamientos:
//    Acercar  — Se acerca al jugador con movimiento en zigzag
//    Rodear   — Orbita al jugador a distancia optima de combate
//    Esquivar — Esquiva perpendicular al detectar un proyectil cercano
//
//  Opera completamente en el plano XZ usando Vec2.
//  Tiene un delay de reaccion configurable (dificultad).
//  Tiene distintos comportamientos según la difiultad.

namespace Archon_PID {

    class CombatAI {
    protected:
        // Estados de la maquina de estados
        enum Estado { Acercar, Huir, Esquivar };

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

        // Revisa todos los disparos del jugador y determina si hay que esquivar alguno
        // Si hay que esquivar, outDir recibe la direccion perpendicular de escape
        bool  debe_esquivar(const std::vector<Disparo>& disparos, Vec2& outDir);

        // Cada derivada implementa su propia estrategia
        virtual Accion decide(const std::vector<Disparo>& disparos, float dt) = 0;

        // Constructor protegido: solo lo llaman las derivadas
        CombatAI(Personaje& IA, Personaje& Jugador, Rectangle arena);

    public:
        virtual ~CombatAI() = default;

        // Actualiza la IA: mueve al personaje y devuelve true si debe disparar
        bool Update(float dt, const std::vector<Disparo>& disparos);
    };

} // namespace Archon_PID