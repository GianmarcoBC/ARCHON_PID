#pragma once
#include "raylib.h"

struct Obstaculo {
    Texture2D* Sprite;
    Rectangle  hitbox;
    bool       solido;
};
