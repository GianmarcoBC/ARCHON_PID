#pragma once
#include "raylib.h"
#include "Vec2.h"
#include "Personaje.h"
#include "Disparo.h"
#include "Obstaculo.h"
#include <vector>
#include <cmath>

//  Interacciones
//  Centraliza toda la detección y resolución de colisiones del combate:
//    · Disparo  vs Personaje  
//    · Personaje vs Obstaculo 
//    · Personaje vs Arena     
//    · Personaje vs Personaje 
class Interacciones
{
    Rectangle arena; // Límites del área de combate

    // Helpers geométricos

    // Devuelve la hitbox centrada en la posición del personaje
    static Rectangle Hitbox(const Personaje& p);

    // Empuje mínimo AABB (Axis-Aligned Bounding Box) entre dos rectángulos solapados.
    // Devuelve true si había solapamiento y escribe el vector de corrección en 'push'.
    static bool EmpujeAABB(Rectangle a, Rectangle b, Vec2& push);

public:
    explicit Interacciones(Rectangle arena) : arena(arena) {}

    // Procesa todos los disparos de 'atacante' contra 'objetivo':
    //   · Si un disparo toca al objetivo → aplica daño y lo desactiva.
    //   · Elimina del vector los disparos inactivos (fuera de pantalla o impactados).
    void DisparosContraPersonaje(std::vector<Disparo>& disparos, Personaje& atacante, Personaje& objetivo);

    // Resuelve la colisión de un personaje con un obstáculo sólido (empuje AABB).
    void PersonajeContraObstaculo(Personaje& p, const Obstaculo& obs);

    // Resuelve las colisiones de ambos personajes con toda la lista de obstáculos.
    void PersonajesContraObstaculos(Personaje& p1, Personaje& p2, const std::vector<Obstaculo>& obstaculos);

    // Mantiene al personaje dentro de los límites de la arena.
    void ClampArena(Personaje& p) const;

    // Resuelve la colisión cuerpo a cuerpo entre dos personajes (se empujan mutuamente).
    void PersonajeContraPersonaje(Personaje& p1, Personaje& p2);
};