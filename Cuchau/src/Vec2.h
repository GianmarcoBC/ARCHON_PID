#pragma once
#include <cmath>

// ============================================================================
//  Vec2.h — Vector 2D para logica en el plano XZ
//
//  Se usa para toda la logica de juego (movimiento, colisiones, IA, direcciones).
//  En el mundo 3D, Vec2.x corresponde al eje X y Vec2.y al eje Z.
// ============================================================================

struct Vec2 {
    float x = 0, y = 0;

    // Suma de dos vectores
    Vec2 operator+(Vec2 o)  const { return { x + o.x, y + o.y }; }

    // Resta de dos vectores
    Vec2 operator-(Vec2 o)  const { return { x - o.x, y - o.y }; }

    // Multiplicacion por escalar (escala el vector)
    Vec2 operator*(float s) const { return { x * s,   y * s }; }

    // Producto punto (escalar) — usado para comparar direcciones y calcular profundidad
    float operator*(Vec2 o) const { return x * o.x + y * o.y; }

    // Producto cruzado 2D — devuelve un escalar, util para determinar
    // de que lado de una linea esta un punto (usado en esquiva de la IA)
    float cross(Vec2 o) const { return x * o.y - y * o.x; }

    // Longitud (modulo) del vector
    float modulo() const { return sqrtf(x * x + y * y); }

    // Devuelve el vector normalizado (longitud 1). Si es cero, devuelve {1,0}
    Vec2  unitario() const { float d = modulo(); return d > 0 ? Vec2{ x / d,y / d } : Vec2{ 1,0 }; }

    // Devuelve el vector perpendicular (rotacion 90 grados en sentido horario)
    Vec2  perp() const { return { -y, x }; }
};
