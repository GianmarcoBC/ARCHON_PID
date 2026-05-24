#pragma once
#include <cmath>
struct Vec2 {
    float x = 0, y = 0;

    //Suma
    Vec2 operator+(Vec2 o)  const { return { x + o.x, y + o.y }; }
    //Resta
    Vec2 operator-(Vec2 o)  const { return { x - o.x, y - o.y }; }
    //Multiplicación por escalar
    Vec2 operator*(float s) const { return { x * s,   y * s }; }
    //Producto escalar
    float operator*(Vec2 o) const { return x * o.x + y * o.y; }
    //Producto cruzado
    float cross(Vec2 o) const { return x * o.y - y * o.x; }
    //Longitud del vector
    float modulo() const { return sqrtf(x * x + y * y); }
    //Normalización del vector
    Vec2  unitario() const { float d = modulo(); return d > 0 ? Vec2{ x / d,y / d } : Vec2{ 1,0 }; }
    //Perpendicular horaria
    Vec2  perp() const { return { -y, x }; }
};