#include "Disparo.h"

//  Disparo::Update — Mueve el proyectil y verifica limites
//
//  El disparo se mueve en el plano XZ usando su velocidad Vec2.
//  Si la posicion sale de los limites de la arena (±arenaHalfW en X,
//  ±arenaHalfL en Z), se desactiva para que sea eliminado.

void Disparo::Update(float dt, float arenaHalfW, float arenaHalfL)
{
    if (!status_) return;

    // Mover en el plano XZ (vel.y corresponde al eje Z del mundo 3D)
    pos3d.x += vel.x * dt;
    pos3d.z += vel.y * dt;   // Vec2.y  -->  mundo Z

    // Desactivar si sale de los limites de la arena
    if (pos3d.x < -arenaHalfW || pos3d.x > arenaHalfW ||
        pos3d.z < -arenaHalfL || pos3d.z > arenaHalfL)
    {
        status_ = false;
    }
}

void Disparo::Draw(Camera camera) const
{
    if (!status_ || !Disp) return;

    float angulo = 0.0f;

    float w = (float)Disp->width, w1 = w;
    float h = (float)Disp->height;

    // Calcula el ángulo según la dirección
    if (vel.x > 0) { angulo = 0.0f; w1 = w; } // →
    else if (vel.x < 0) { angulo = 0.0f; w1 = -w; } // ←
    else if (vel.y < 0) { angulo = 270.0f; w1 = w; } // ↑
    else if (vel.y > 0) { angulo = 270.0f; w1 = -w; } // ↓

	DrawBillboardPro(
		camera,
		*Disp,
		{ 0, 0, w1, h },
		{ pos3d.x, pos3d.y, pos3d.z },
		{ 0,1,0 },
		{ size3D, size3D },
		{ size3D / 2, size3D / 2 },
		angulo,
		WHITE
	);
}
