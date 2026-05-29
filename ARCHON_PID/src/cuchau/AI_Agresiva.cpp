#include "AI_Agresiva.h"

AI_Agresiva::AI_Agresiva(Personaje& IA, Personaje& Jugador, Rectangle arena)
    : CombatAI(IA, Jugador, arena)
{
    delay_IA = 0.04f;
    distancia_combate_optima *= 0.6f;
    aimbot = 0.85f;
}

CombatAI::Accion AI_Agresiva::decide(const std::vector<Disparo>& disparos, float dt) {
    Vec2  n = (Jugador.GetPos() - IA.GetPos()).unitario();
    float dist = (Jugador.GetPos() - IA.GetPos()).modulo();
    float dmax{ 5.0f };

    if (Jugador.GetRangoMax() == Rango_bajo)
        dmax *= 0.8f;
    else if (Jugador.GetRangoMax() == Rango_medio)
        dmax *= 0.9f;

    if (dist > dmax) estado = Acercar;
    else estado = Huir;

    Vec2 dir_mov{};
    if (estado == Acercar) dir_mov = n;
    else dir_mov = n.unitario() * -1.0f;

    Vec2  toPlayer = (Jugador.GetPos() - IA.GetPos()).unitario();
    Vec2  dir_norm = dir_mov.modulo() > 0 ? dir_mov.unitario() : Vec2{ 0,0 };
    float punteria = dir_norm * toPlayer;
    float umbral = aimbot + sinf((float)GetTime() * 7.3f) * 0.03f;

    return { dir_mov, punteria >= umbral };
}
