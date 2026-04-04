#include "Disparo.h"

void Disparo::Update()
{
    if (!status_) return; // Solo actualiza si el disparo está activo

    pos.x += vel.x;
    pos.y += vel.y;

    if (pos.y < 0 || pos.y > 580 || pos.x < 0 || pos.x > 970) {
        status_ = false; // Desactiva el disparo si sale de la pantalla
    }
}

void Disparo::Draw()
{
    if (!status_ || !Disp) return;

    float angulo = 0.0f;

    float w = Disp->width, w1=w;
    float h = Disp->height;

    // Calcula el ángulo según la dirección
    if (vel.x > 0) { angulo = 0.0f; w1 = w; } // →
    else if (vel.x < 0) { angulo = 0.0f; w1 = -w; } // ←
    else if (vel.y < 0) { angulo = 270.0f; w1 = w; } // ↑
    else if (vel.y > 0) { angulo = 270.0f; w1 = -w; } // ↓

    

    DrawTexturePro(
        *Disp,
        { 0, 0, w1, h },              // Región fuente (sprite completo)
        { pos.x, pos.y, w, h },      // Región destino
        {w/2, h/2 },            // Origen en el centro (pivote de rotación)
        angulo,
        WHITE
    );
}


