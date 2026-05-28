#include "Interacciones.h"

Rectangle Interacciones::Hitbox(const Personaje& p)
{
    float s = p.Size3D;
    float sz = s * 0.5f;
    return { p.pos3d.x - s / 2.0f, p.pos3d.z - sz / 2.0f, s, sz };
}

bool Interacciones::EmpujeAABB(Rectangle a, Rectangle b, Vec2& push)
{
    if (!CheckCollisionRecs(a, b)) return false;

    float overlapX = (a.x + a.width  / 2.0f) - (b.x + b.width  / 2.0f);
    float overlapY = (a.y + a.height / 2.0f) - (b.y + b.height / 2.0f);
    float halfW = (a.width  + b.width)  / 2.0f;
    float halfH = (a.height + b.height) / 2.0f;
    float pushX = halfW - fabsf(overlapX);
    float pushY = halfH - fabsf(overlapY);

    if (pushX < pushY)
        push = { (overlapX > 0) ? pushX : -pushX, 0.0f };
    else
        push = { 0.0f, (overlapY > 0) ? pushY : -pushY };

    return true;
}

void Interacciones::DisparosContraPersonaje(
    std::vector<Disparo>& disparos,
    Personaje& atacante, Personaje& objetivo)
{
    float radio = objetivo.Size3D / 2.0f;

    for (Disparo& d : disparos) {
        if (!d.status_) continue;
        float dx = d.pos3d.x - objetivo.pos3d.x;
        float dy = d.pos3d.z - objetivo.pos3d.z;
        if (dx * dx + dy * dy <= radio * radio) {
            objetivo.pain(atacante.Player.fuerza);
            d.status_ = false;
        }
    }

    disparos.erase(
        std::remove_if(disparos.begin(), disparos.end(),
            [](const Disparo& d) { return !d.status_; }),
        disparos.end()
    );
}

void Interacciones::PersonajeContraObstaculo(Personaje& p, const obstaculo& obs)
{
    if (!obs.solido) return;
    Rectangle obsRect = {
        obs.colCX - obs.colHW, obs.colCZ - obs.colHL,
        obs.colHW * 2.0f, obs.colHL * 2.0f
    };
    Vec2 push{};
    if (EmpujeAABB(Hitbox(p), obsRect, push)) {
        p.pos3d.x = p.pos3d.x + push.x;
        p.pos3d.z = p.pos3d.z + push.y;
    }
}

void Interacciones::PersonajesContraObstaculos(
    Personaje& p1, Personaje& p2,
    std::vector<obstaculo*>& obs)
{
    for (auto* o : obs) {
        PersonajeContraObstaculo(p1, *o);
        PersonajeContraObstaculo(p2, *o);
    }
}

void Interacciones::DisparosContraObstaculos(std::vector<Disparo>& disparos, std::vector<obstaculo*>& obs)
{
    for (const auto* o : obs) {
        for (auto& d : disparos) {
            if (!d.getStatus()) continue;
            Rectangle obsRect = {
                o->colCX - o->colHW, o->colCZ - o->colHL,
                o->colHW * 2.0f, o->colHL * 2.0f
            };
            if (CheckCollisionCircleRec(d.GetPos().toVector2(), Disparo::size3D / 2.0f, obsRect)) {
                d.setStatus(false);
            }
        }
    }
}

void Interacciones::ClampArena(Personaje& p, Arena& arena)
{
    float hs = p.Size3D / 2.0f;
    if (p.pos3d.x - hs < -arena.sueloWidth / 2)  p.pos3d.x = -arena.sueloWidth / 2 + hs;
    if (p.pos3d.x + hs > arena.sueloWidth / 2)   p.pos3d.x = arena.sueloWidth / 2 - hs;
    if (p.pos3d.z - hs < -arena.sueloLength / 2)  p.pos3d.z = -arena.sueloLength / 2 + hs;
    if (p.pos3d.z + hs > arena.sueloLength / 2)   p.pos3d.z = arena.sueloLength / 2 - hs;
}

void Interacciones::PersonajeContraPersonaje(Personaje& p1, Personaje& p2)
{
    Vec2 push{};
    if (!EmpujeAABB(Hitbox(p1), Hitbox(p2), push)) return;
    p1.SetPos(p1.GetPos() + push * 0.5f);
    p2.SetPos(p2.GetPos() - push * 0.5f);
}
