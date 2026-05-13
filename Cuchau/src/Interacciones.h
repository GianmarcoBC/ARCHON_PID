#pragma once
#include "raylib.h"
#include "Vec2.h"
#include "Personaje.h"
#include "Disparo.h"
#include "obstaculo.h"
#include <vector>
#include <cmath>
#include <algorithm>

// ============================================================================
//  Interacciones.h — Sistema de colisiones del combate
//
//  Maneja todas las colisiones en el plano XZ:
//    - Disparos contra personajes (colision circular)
//    - Personajes contra obstaculos (empuje AABB)
//    - Personaje contra personaje (empuje mutuo AABB)
//    - Limitar personajes dentro de la arena (clamp)
//
//  Todas las posiciones y hitboxes son en el plano XZ (Vec2).
// ============================================================================

class Interacciones
{
    Rectangle arena;   // Limites de la arena en XZ: {minX, minZ, anchoX, largoZ}

    // Genera un rectangulo de colision centrado en la posicion XZ del personaje
    static Rectangle Hitbox(const Personaje& p);

    // Calcula el vector de empuje minimo para separar dos rectangulos que se solapan
    // Devuelve true si hay colision, false si no
    static bool EmpujeAABB(Rectangle a, Rectangle b, Vec2& push);

public:
    // Constructor: recibe los limites de la arena como Rectangle
    explicit Interacciones(Rectangle arena) : arena(arena) {}

    // Verifica colision circular de cada disparo contra el objetivo.
    // Si impacta: aplica dano y desactiva el disparo. Elimina disparos inactivos del vector.
    void DisparosContraPersonaje(std::vector<Disparo>& disparos,
                                 Personaje& atacante, Personaje& objetivo);

    // Empuja un personaje fuera de un obstaculo solido si hay colision AABB
    void PersonajeContraObstaculo(Personaje& p, const obstaculo& obs);

    // Verifica colision de ambos personajes contra todos los obstaculos
    void PersonajesContraObstaculos(Personaje& p1, Personaje& p2,
                                    obstaculo* obs[], int count);

    // Limita la posicion del personaje dentro de los bordes de la arena
    void ClampArena(Personaje& p) const;

    // Separa dos personajes que se solapan con empuje mutuo (50% cada uno)
    void PersonajeContraPersonaje(Personaje& p1, Personaje& p2);
};
