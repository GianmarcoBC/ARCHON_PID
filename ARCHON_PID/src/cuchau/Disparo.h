#pragma once
#include "raylib.h"
#include "Vec2.h"

class Disparo
{
    Texture2D* Disp;
    Vector3 pos3d{};
    Vec2 vel{};
    bool status_{ false };
    bool fromPlayer{ true };
    float distanciaRecorrida{ 0.0f };
    float maxDistancia{ 0.0f };

    friend class Interacciones;

public:
    static constexpr float size3D = 1.5f;

    Disparo(Vector3 p, Vec2 v, Texture2D* tx, bool fp = true, float maxDist = 0.0f)
        : Disp(tx), pos3d(p), vel(v), status_(true), fromPlayer(fp), maxDistancia(maxDist) {}

    void Update(float dt, float arenaHalfW, float arenaHalfL);
    void Draw(Camera camera) const;

    Vector3 GetPos3D() const { return pos3d; }
    Vec2    GetPos()   const { return { pos3d.x, pos3d.z }; }
    Vec2    GetVel()   const { return vel; }
    bool    getStatus()    const { return status_; }
    void    setStatus(bool s) { status_ = s; }
    bool    isFromPlayer() const { return fromPlayer; }
    Texture2D* getTexture() const { return Disp; }

    ~Disparo() = default;
};
