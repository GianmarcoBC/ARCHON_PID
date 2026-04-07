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
    float w = (float)Sprite.width, w1=w;
    float h = (float)Sprite.height;

    // Calcula el ángulo según la dirección
    if (l_dir.x > 0)      { w1 = w; } // →
    else if (l_dir.x < 0) { w1 = -w; } // ←
 

    DrawTexturePro(
        Sprite,
        { 0, 0, w1, h },              // Región fuente (sprite completo)
        { pos.x, pos.y, w, h },      // Región destino
        { w / 2, h / 2 },            // Origen en el centro (pivote de rotación)
        0.0f,
        WHITE
    );

    DrawLine((int)pos.x - 32, (int)pos.y - 40, (int)pos.x - 32 + 64*((int)Player.vida)/ ((int)max_vida), (int)pos.y - 40, RED);
    DrawText(TextFormat("%.0f", Player.vida), (int)pos.x - 16, (int)pos.y - 60, 20, RED);
}

void Personaje::pain(float damage)
{
    if (Player.vida > damage)    
        Player.vida -= damage;
    else                       
        Player.vida = 0;
}

Disparo Personaje::Shoot()
{
    float v = 10.0f; // Velocidad de la flecha
    Vector2 vel = { v * l_dir.x, v * l_dir.y }; // Velocidad de la flecha hacia arriba
    return Disparo ({ pos.x, pos.y }, vel, &Ataque);
}