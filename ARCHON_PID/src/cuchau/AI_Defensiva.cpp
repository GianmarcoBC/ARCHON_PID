#include "AI_Defensiva.h"

AI_Defensiva::AI_Defensiva(Personaje& IA, Personaje& Jugador, Rectangle arena)
    : CombatAI(IA, Jugador, arena)
{
    delay_IA = 0.12f;
    distancia_combate_optima *= 1.3f;
    aimbot = 0.97f;
}

CombatAI::Accion AI_Defensiva::decide(const std::vector<Disparo>& disparos, float dt) {
    Vec2  n = (Jugador.GetPos() - IA.GetPos()).unitario();
    float dist = (Jugador.GetPos() - IA.GetPos()).modulo();

    Vec2 dir_esq{};
    bool esquiva = debe_esquivar(disparos, dir_esq);
    bool demasiado_cerca = dist < distancia_combate_optima * 0.6f;
    float dmax{ 15.0 };

    if (Jugador.GetRangoMax() == Rango_bajo)
        dmax *= 0.1f;
    else if (Jugador.GetRangoMax() == Rango_medio)
        dmax *= 0.6f;

    if (esquiva || demasiado_cerca) {
        estado = Esquivar;
        if (demasiado_cerca && !esquiva)
            dir_esq = (n * -1.0);
    }
    else if (dist > dmax)
        estado = Acercar;
    else
        estado = Huir;

    Vec2 dir_mov{};
    if (estado == Esquivar) dir_mov = dir_esq;
    else if (estado == Acercar) dir_mov = n.unitario();
    else dir_mov = n.unitario() * -1.0f;

    Vec2  toPlayer = (Jugador.GetPos() - IA.GetPos()).unitario();
    Vec2  dir_norm = dir_mov.modulo() > 0 ? dir_mov.unitario() : Vec2{ 0,0 };
    float punteria = dir_norm * toPlayer;
    float umbral = aimbot + sinf((float)GetTime() * 7.3f) * 0.03f;

    return { dir_mov, punteria >= umbral };
}
