#include "Interacciones.h"

//  Helpers geométricos (privados/estáticos)

Rectangle Interacciones::Hitbox(const Personaje& p)
{
    float w = p.GetAncho();
    float h = p.GetAlto();
    return { p.GetPos().x - w / 2.0f,
             p.GetPos().y - h / 2.0f,
             w, h };
}

bool Interacciones::EmpujeAABB(Rectangle a, Rectangle b, Vec2& push)
{
    if (!CheckCollisionRecs(a, b)) return false;

    float overlapX = (a.x + a.width / 2.0f) - (b.x + b.width / 2.0f);
    float overlapY = (a.y + a.height / 2.0f) - (b.y + b.height / 2.0f);

    float halfW = (a.width + b.width) / 2.0f;
    float halfH = (a.height + b.height) / 2.0f;

    float pushX = halfW - fabsf(overlapX);
    float pushY = halfH - fabsf(overlapY);

    // Empujar por el eje de menor solapamiento
    if (pushX < pushY)
        push = { (overlapX > 0) ? pushX : -pushX, 0.0f };
    else
        push = { 0.0f, (overlapY > 0) ? pushY : -pushY };

    return true;
}

//  DisparosContraPersonaje

void Interacciones::DisparosContraPersonaje(std::vector<Disparo>& disparos, Personaje& atacante, Personaje& objetivo, float dt)
{
    Rectangle hitboxObj = Hitbox(objetivo);
    const float radio = hitboxObj.width / 2.0f; // Radio de impacto basado en la hitbox

    for (Disparo& d : disparos) {
        if (!d.getStatus()) continue;

        d.Update(dt);

        Vec2  dPos = d.GetPos();
        Vec2  oPos = objetivo.GetPos();

        // Detección circular 
        float dx = dPos.x - oPos.x;
        float dy = dPos.y - oPos.y;
        if (dx * dx + dy * dy <= radio * radio) {
            objetivo.pain(atacante.GetFuerza());
            d.setStatus(false);
        }
    }

    // Limpia los disparos inactivos para no acumular basura en el vector
    disparos.erase(
        std::remove_if(disparos.begin(), disparos.end(),
            [](const Disparo& d) { return !d.getStatus(); }),
        disparos.end()
    );
}

//  PersonajeContraObstaculo

void Interacciones::PersonajeContraObstaculo(Personaje& p, const Obstaculo& obs)
{
    if (!obs.solido) return;

    Vec2 push{};
    if (EmpujeAABB(Hitbox(p), obs.hitbox, push))
        p.SetPos(p.GetPos() + push);
}

//  PersonajesContraObstaculos

void Interacciones::PersonajesContraObstaculos(Personaje& p1, Personaje& p2, const std::vector<Obstaculo>& obstaculos)
{
    for (const Obstaculo& obs : obstaculos) {
        PersonajeContraObstaculo(p1, obs);
        PersonajeContraObstaculo(p2, obs);
    }
}

//  ClampArena

void Interacciones::ClampArena(Personaje& p) const
{
    float hw = p.GetAncho() / 2.0f;
    float hh = p.GetAlto() / 2.0f;
    Vec2  pos = p.GetPos();

    if (pos.x - hw < arena.x)                     pos.x = arena.x + hw;
    if (pos.x + hw > arena.x + arena.width)       pos.x = arena.x + arena.width - hw;
    if (pos.y - hh < arena.y)                     pos.y = arena.y + hh;
    if (pos.y + hh > arena.y + arena.height)      pos.y = arena.y + arena.height - hh;

    p.SetPos(pos);
}

//  PersonajeContraPersonaje

void Interacciones::PersonajeContraPersonaje(Personaje& p1, Personaje& p2)
{
    Vec2 push{};
    if (!EmpujeAABB(Hitbox(p1), Hitbox(p2), push)) return;

    // Cada uno recibe la mitad del empuje en sentidos opuestos
    p1.SetPos(p1.GetPos() + push * 0.5f);
    p2.SetPos(p2.GetPos() - push * 0.5f);
}