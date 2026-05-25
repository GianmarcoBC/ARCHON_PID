#include "Interacciones.h"

//  Hitbox — Genera un rectangulo de colision para un personaje
//
//  Crea un rectangulo de charSize x charSize (4x4 unidades) centrado en la
//  posicion XZ del personaje. Se usa para todas las colisiones AABB.

Rectangle Interacciones::Hitbox(const Personaje& p)
{
    float s = p.Size3D;
    return { p.pos3d.x - s / 2.0f, p.pos3d.z - s / 2.0f, s, s };
}

//  EmpujeAABB — Calcula el vector de empuje minimo entre dos rectangulos
//
//  Si los rectangulos se solapan, calcula el eje con menor penetracion
//  y devuelve el vector de empuje necesario para separarlos.
//  Siempre empuja por el eje con menor solapamiento (resolucion minima).

bool Interacciones::EmpujeAABB(Rectangle a, Rectangle b, Vec2& push)
{
    // Verificar si hay colision usando la funcion de raylib
    if (!CheckCollisionRecs(a, b)) return false;

    // Diferencia entre centros de los rectangulos
    float overlapX = (a.x + a.width  / 2.0f) - (b.x + b.width  / 2.0f);
    float overlapY = (a.y + a.height / 2.0f) - (b.y + b.height / 2.0f);

    // Suma de las mitades (distancia maxima antes de solapamiento)
    float halfW = (a.width  + b.width)  / 2.0f;
    float halfH = (a.height + b.height) / 2.0f;

    // Penetracion en cada eje
    float pushX = halfW - fabsf(overlapX);
    float pushY = halfH - fabsf(overlapY);

    // Empujar por el eje con menor penetracion (resolucion mas suave)
    if (pushX < pushY)
        push = { (overlapX > 0) ? pushX : -pushX, 0.0f };
    else
        push = { 0.0f, (overlapY > 0) ? pushY : -pushY };

    return true;
}

//  DisparosContraPersonaje — Colision circular disparo vs personaje
//
//  Para cada disparo activo, verifica si esta dentro del radio de colision
//  del personaje objetivo (charSize/2 = 2.0 unidades). Si impacta:
//    1. Aplica el dano del atacante al objetivo
//    2. Desactiva el disparo
//  Al final, elimina todos los disparos inactivos del vector.

void Interacciones::DisparosContraPersonaje(
    std::vector<Disparo>& disparos,
    Personaje& atacante, Personaje& objetivo)
{
    float radio = objetivo.Size3D / 2.0f;  // Radio de colision = 2.0

    for (Disparo& d : disparos) {
        if (!d.status_) continue;

        // Calcular distancia al cuadrado entre disparo y objetivo (en XZ)
        float dx   = d.pos3d.x - objetivo.pos3d.x;
        float dy   = d.pos3d.z - objetivo.pos3d.z;

        // Colision circular: distancia^2 <= radio^2
        if (dx * dx + dy * dy <= radio * radio) {
            objetivo.pain(atacante.Player.fuerza);  // Aplicar daño
            d.status_ = false;                   // Desactivar disparo
        }
    }

    // Eliminar disparos inactivos del vector (patron erase-remove)
    disparos.erase(
        std::remove_if(disparos.begin(), disparos.end(),
            [](const Disparo& d) { return !d.status_; }),
        disparos.end()
    );
}

//  PersonajeContraObstaculo — Empuja al personaje fuera de un obstaculo solido
//
//  Solo actua si el obstaculo es solido. Construye un rectangulo AABB a partir
//  de los datos de colision del obstaculo y empuja al personaje si hay solapamiento.

void Interacciones::PersonajeContraObstaculo(Personaje& p, const obstaculo& obs)
{
    if (!obs.solido) return;

    // Construir rectangulo de colision del obstaculo desde su centro y mitades
    Rectangle obsRect = {
        obs.colCX - obs.colHW, obs.colCZ - obs.colHL,
        obs.colHW * 2.0f, obs.colHL * 2.0f
    };

    Vec2 push{};
    if (EmpujeAABB(Hitbox(p), obsRect, push)) {
        p.pos3d.x = p.pos3d.x + push.x;  // Empujar al personaje fuera del obstaculo
        p.pos3d.z = p.pos3d.z + push.y;  // Empujar al personaje fuera del obstaculo
    }
}

//  PersonajesContraObstaculos — Verifica colision de ambos personajes
//  contra todos los obstaculos de la arena

void Interacciones::PersonajesContraObstaculos(
    Personaje& p1, Personaje& p2,
    std::vector<obstaculo*>& obs)
{
    for (auto* o : obs) {
        PersonajeContraObstaculo(p1, *o);
        PersonajeContraObstaculo(p2, *o);
    }
}

// ============================================================================
//  ClampArena — Limita al personaje dentro de los bordes de la arena
//
//  Si alguna parte del hitbox (charSize/2 desde el centro) sale de los limites
//  de la arena, lo reposiciona al borde.
// ============================================================================

void Interacciones::ClampArena(Personaje& p, Arena& arena)
{
    float hs  = p.Size3D / 2.0f;  // Mitad del tamano del personaje

    // Clamp en cada borde de la arena
    if (p.pos3d.x - hs < arena.sueloWidth / 2) p.pos3d.x = arena.sueloWidth / 2 + hs;
    if (p.pos3d.x + hs > arena.sueloWidth / 2 + arena.sueloWidth) p.pos3d.x = arena.sueloWidth / 2 + arena.sueloWidth  - hs;
    if (p.pos3d.z - hs < arena.sueloLength / 2) p.pos3d.z = arena.sueloLength / 2 + hs;
    if (p.pos3d.z + hs > arena.sueloLength / 2 + arena.sueloLength) p.pos3d.z = arena.sueloLength / 2 + arena.sueloLength - hs;

}

// ============================================================================
//  PersonajeContraPersonaje — Separa dos personajes que se solapan
//
//  Si los hitboxes de P1 y P2 se solapan, aplica la mitad del empuje a cada
//  uno en direcciones opuestas (separacion simetrica).
// ============================================================================

void Interacciones::PersonajeContraPersonaje(Personaje& p1, Personaje& p2)
{
    Vec2 push{};
    if (!EmpujeAABB(Hitbox(p1), Hitbox(p2), push)) return;

    p1.SetPos(p1.GetPos() + push * 0.5f);   // Empujar P1 en una direccion
    p2.SetPos(p2.GetPos() - push * 0.5f);   // Empujar P2 en la opuesta
}
