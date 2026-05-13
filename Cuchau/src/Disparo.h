#pragma once
#include "raylib.h"
#include "Vec2.h"

// ============================================================================
//  Disparo.h — Proyectil 3D tipo billboard
//
//  Cada disparo tiene una posicion 3D (para renderizado) y una velocidad Vec2
//  en el plano XZ (para la logica de juego). Se desactiva al salir de la arena.
// ============================================================================

class Disparo
{
    Texture2D* Disp;           // Puntero a la textura del proyectil (compartida con Personaje)
    Vector3 pos3d{};           // Posicion 3D actual del disparo
    Vec2 vel{};                // Velocidad en el plano XZ (Vec2.y = eje Z del mundo)
    bool status_{ false };     // true = activo, false = ya impacto o salio de la arena
    bool fromPlayer{ true };   // true = disparado por jugador humano, false = por IA

public:
    // Tamano del billboard del disparo en unidades 3D
    static constexpr float size = 1.5f;

    // Constructor: posicion inicial, velocidad XZ, textura, y quien lo disparo
    Disparo(Vector3 p, Vec2 v, Texture2D* tx, bool fp = true)
        : Disp(tx), pos3d(p), vel(v), status_(true), fromPlayer(fp) {}

    // Mueve el disparo y lo desactiva si sale de los limites de la arena
    void Update(float dt, float arenaHalfW, float arenaHalfL);

    // Posicion 3D completa (para renderizado con DrawBillboard)
    Vector3 GetPos3D() const { return pos3d; }

    // Posicion en plano XZ (para colisiones y logica de juego)
    Vec2    GetPos()   const { return { pos3d.x, pos3d.z }; }

    // Velocidad en plano XZ (usada por la IA para esquivar)
    Vec2    GetVel()   const { return vel; }

    // Estado del disparo (activo/inactivo)
    bool    getStatus()    const { return status_; }
    void    setStatus(bool s) { status_ = s; }

    // Indica si fue disparado por un jugador humano
    bool    isFromPlayer() const { return fromPlayer; }

    // Puntero a la textura (para renderizar como billboard)
    Texture2D* getTexture() const { return Disp; }
};
