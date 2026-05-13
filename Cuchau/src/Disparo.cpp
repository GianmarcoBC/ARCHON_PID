#include "Disparo.h"

// ============================================================================
//  Disparo::Update — Mueve el proyectil y verifica limites
//
//  El disparo se mueve en el plano XZ usando su velocidad Vec2.
//  Si la posicion sale de los limites de la arena (±arenaHalfW en X,
//  ±arenaHalfL en Z), se desactiva para que sea eliminado.
// ============================================================================

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
