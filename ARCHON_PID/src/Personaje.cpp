#include "Personaje.h"

void Personaje::Update(float dt)
{
    
    if (IsKeyDown(Controles.right)) { pos.x += Player.vel * dt; l_dir = { 1.0f, 0.0f }; } // Mover a la derecha y actualizar dirección
    if (IsKeyDown(Controles.left)) { pos.x -= Player.vel * dt; l_dir = { -1.0f, 0.0f }; } // Mover a la izquierda y actualizar dirección
    if (IsKeyDown(Controles.up)) { pos.y -= Player.vel * dt; l_dir = { 0.0f, -1.0f }; } // Mover hacia arriba y actualizar dirección
    if (IsKeyDown(Controles.down)) { pos.y += Player.vel * dt; l_dir = { 0.0f, 1.0f }; } // Mover hacia abajo y actualizar dirección
}

void Personaje::Draw()
{
    float angulo = 0.0f;

    float w = Sprite.width, w1=w;
    float h = Sprite.height;

    // Calcula el ángulo según la dirección
    if (l_dir.x > 0)      { angulo = 0.0f; w1 = w; } // →
    else if (l_dir.x < 0) { angulo = 0.0f; w1 = -w; } // ←
    else if (l_dir.y < 0) { angulo = 270.0f; w1 = w; } // ↑
    else if (l_dir.y > 0) { angulo = 270.0f; w1 = -w; } // ↓

    DrawTexturePro(
        Sprite,
        { 0, 0, w1, h },              // Región fuente (sprite completo)
        { pos.x, pos.y, w, h },      // Región destino
        { w / 2, h / 2 },            // Origen en el centro (pivote de rotación)
        angulo,
        WHITE
    );
}

Disparo Personaje::Shoot()
{
    float v = 10.0f; // Velocidad de la flecha
    Vector2 vel = { v * l_dir.x, v * l_dir.y }; // Velocidad de la flecha hacia arriba
    return Disparo ({ pos.x, pos.y }, vel, &Ataque);
}