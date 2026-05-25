#include "CombatAI.h"

//  Constructor base — Inicializa estado compartido

CombatAI::CombatAI(Personaje& IA, Personaje& Jugador, Rectangle arena)
    : IA(IA), Jugador(Jugador), arena(arena),
    estado(Acercar), sentido_giro(1.0f), cont_giro(0.0f),
    ultima_accion{ {0, 0}, false }, cooldown_IA(0.0f), delay_IA(0.0f)
{
    float lado_corto = fminf(arena.width, arena.height);
    distancia_combate_optima = lado_corto * 0.38f;
    rango_seguro = distancia_combate_optima * 0.25f;
    rango_esquiva = 2.0f;
    aimbot = 0.93f;
    cambio_giro = (1.8f + (rand() % 100) / 72.0f);
}

//  Pared_cerc — Distancia a la pared mas cercana

float CombatAI::Pared_cerc(Vec2 pos) const {
    return fminf(fminf(pos.x - arena.x, arena.x + arena.width - pos.x),
        fminf(pos.y - arena.y, arena.y + arena.height - pos.y));
}

//  evitar_pared — Corrige direccion para no chocar con los bordes

void CombatAI::evitar_pared(Vec2& v, Vec2 pos, Vec2 lim_x, Vec2 lim_y, float margin) {
    if (pos.x - lim_x.x < margin && v.x < 0) v.x = fabsf(v.x);   // cerca borde izq y va izq → empuja der
    if (lim_x.y - pos.x < margin && v.x > 0) v.x = -fabsf(v.x);  // cerca borde der y va der → empuja izq
    if (pos.y - lim_y.x < margin && v.y < 0) v.y = fabsf(v.y);   // cerca borde arr y va arr → empuja aba
    if (lim_y.y - pos.y < margin && v.y > 0) v.y = -fabsf(v.y);  // cerca borde aba y va aba → empuja arr

    if (v.modulo() > 0) v = v.unitario();
}

//  debe_esquivar — Detecta proyectiles peligrosos y calcula direccion de escape

bool CombatAI::debe_esquivar(const std::vector<Disparo>& disparos, Vec2& outDir) {
    for (const Disparo& d : disparos) {
        if (!d.isFromPlayer()) continue;

        Vec2 Vector_IA_Disparo = IA.GetPos() - d.GetPos();
        Vec2 pn = d.GetVel().unitario();

        if (Vector_IA_Disparo * pn < 0) continue;
        if (fabsf(Vector_IA_Disparo.cross(pn)) > rango_esquiva) continue;

        Vec2  d1 = { -pn.y,  pn.x };
        Vec2  d2 = { pn.y, -pn.x };
        float s1 = Pared_cerc(IA.GetPos() + d1 * IA.GetVelocidad() * 4);
        float s2 = Pared_cerc(IA.GetPos() + d2 * IA.GetVelocidad() * 4);
        outDir = (s1 >= s2) ? d1 : d2;
        return true;
    }
    return false;
}

//  Update — Ciclo principal compartido por todas las derivadas

bool CombatAI::Update(float dt, const std::vector<Disparo>& disparos) {
    if (cooldown_IA > 0) {
        cooldown_IA -= dt;
    }
    else {
        // Apuntar al jugador ANTES de decidir para que la punteria sea correcta

        Vec2 toPlayer = (Jugador.GetPos() - IA.GetPos()).unitario();
        IA.SetDir(toPlayer);

        ultima_accion = decide(disparos, dt);
        cooldown_IA = delay_IA;
    }

    if (ultima_accion.d.x != 0 || ultima_accion.d.y != 0) {

        IA.Move(ultima_accion.d, dt);
    }

    return ultima_accion.disparar;
}