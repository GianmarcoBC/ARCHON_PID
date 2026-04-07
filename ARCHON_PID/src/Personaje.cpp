#include "Personaje.h"

void Personaje::Update(float dt)
{ 
    moviendose = false;

    if (IsKeyDown(Controles.right)) { pos.x += Player.vel * dt; l_dir = { 1.0f, 0.0f }; moviendose = true;  } // Mover a la derecha y actualizar dirección
    if (IsKeyDown(Controles.left)) { pos.x -= Player.vel * dt; l_dir = { -1.0f, 0.0f }; moviendose = true;} // Mover a la izquierda y actualizar dirección
    if (IsKeyDown(Controles.up)) { pos.y -= Player.vel * dt; l_dir = { 0.0f, -1.0f }; moviendose = true;} // Mover hacia arriba y actualizar dirección
    if (IsKeyDown(Controles.down)) { pos.y += Player.vel * dt; l_dir = { 0.0f, 1.0f }; moviendose = true;} // Mover hacia abajo y actualizar dirección

    if (moviendose) {
        frameTimer += dt;
        if (frameTimer >= Player.frameSpeed) {
            frameTimer = 0.0f;
            frameActual = (frameActual + 1) % Player.frameCount;
        }
    }
    else {
        frameActual = 0; // Vuelve al frame de reposo
        frameTimer = 0.0f;
    }

}

void Personaje::Draw()
{

    Texture2D& texActual = Frames[frameActual]; // Usa el frame actual
    float w = (float)texActual.width;
    float h = (float)texActual.height;
    float srcW = (l_dir.x < 0) ? -w : w;       // Espejo si va a la izquierda

    DrawTexturePro(
        texActual,
        { 0, 0, srcW, h },
        { pos.x, pos.y, w, h },
        { w / 2, h / 2 },
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