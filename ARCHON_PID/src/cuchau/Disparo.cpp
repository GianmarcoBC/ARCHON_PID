#include "Disparo.h"

void Disparo::Update(float dt, float arenaHalfW, float arenaHalfL)
{
    if (!status_) return;

    pos3d.x += vel.x * dt;
    pos3d.z += vel.y * dt;

    if (pos3d.x < -arenaHalfW || pos3d.x > arenaHalfW || pos3d.z < -arenaHalfL || pos3d.z > arenaHalfL) status_ = false;
    if (maxDistancia > 0.0f) {
        distanciaRecorrida += sqrtf(vel.x * dt * vel.x * dt + vel.y * dt * vel.y * dt);
        if (distanciaRecorrida >= maxDistancia)
            status_ = false;
    }
}

void Disparo::Draw(Camera camera) const
{
    if (!status_ || !Disp) return;

    float angulo = 0.0f;
    float w = (float)Disp->width, w1 = w;
    float h = (float)Disp->height;

    if (vel.x > 0) { angulo = 0.0f; w1 = w; }
    else if (vel.x < 0) { angulo = 0.0f; w1 = -w; }
    else if (vel.y < 0) { angulo = 270.0f; w1 = w; }
    else if (vel.y > 0) { angulo = 270.0f; w1 = -w; }

    DrawBillboardPro(camera, *Disp, { 0, 0, w1, h }, { pos3d.x, pos3d.y, pos3d.z },
        { 0,1,0 }, { size3D, size3D }, { size3D / 2, size3D / 2 }, angulo, WHITE);
}
