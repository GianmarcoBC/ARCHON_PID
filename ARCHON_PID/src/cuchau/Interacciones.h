#pragma once
#include "raylib.h"
#include "Vec2.h"
#include "Personaje.h"
#include "Disparo.h"
#include "obstaculo.h"
#include "Arena.h"
#include <vector>
#include <cmath>
#include <algorithm>

class Interacciones
{
    static Rectangle Hitbox(const Personaje& p);
    static bool EmpujeAABB(Rectangle a, Rectangle b, Vec2& push);
    static void PersonajeContraObstaculo(Personaje& p, const obstaculo& obs);

public:
    static void DisparosContraPersonaje(std::vector<Disparo>& disparos,
                                 Personaje& atacante, Personaje& objetivo);
    static void PersonajesContraObstaculos(Personaje& p1, Personaje& p2,
                                    std::vector<obstaculo*>& obs);
    static void DisparosContraObstaculos(std::vector<Disparo>& disparos, std::vector<obstaculo*>& obs);
    static void ClampArena(Personaje& p, Arena& arena);
    static void PersonajeContraPersonaje(Personaje& p1, Personaje& p2);
};
