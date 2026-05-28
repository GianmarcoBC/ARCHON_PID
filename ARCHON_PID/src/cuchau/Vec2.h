#pragma once
#include <cmath>
#include "raylib.h"

struct Vec2 {
    float x = 0, y = 0;

    Vec2 operator+(Vec2 o)  const { return { x + o.x, y + o.y }; }
    Vec2 operator-(Vec2 o)  const { return { x - o.x, y - o.y }; }
    Vec2 operator*(float s) const { return { x * s,   y * s }; }
    float operator*(Vec2 o) const { return x * o.x + y * o.y; }

    float cross(Vec2 o) const { return x * o.y - y * o.x; }
    float modulo() const { return sqrtf(x * x + y * y); }
    Vec2  unitario() const { float d = modulo(); return d > 0 ? Vec2{ x / d,y / d } : Vec2{ 1,0 }; }
    Vec2  perp() const { return { -y, x }; }
    Vector2 toVector2() const { return { x, y }; }
};
