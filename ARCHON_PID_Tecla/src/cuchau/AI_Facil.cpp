#include "AI_Facil.h"

//  AI_Facil — Reaccion lenta (~250ms), estrategia neutra
//
//  Comportamiento: zigzag al acercarse, orbita al
//  llegar a distancia optima, esquiva proyectiles si los detecta.

AI_Facil::AI_Facil(Personaje& IA, Personaje& Jugador, Rectangle arena)
    : CombatAI(IA, Jugador, arena)
{
    delay_IA = 0.25f;  // Intervalo de decision: ~250ms (el mas lento de los tres niveles)
}

//  decide — Estrategia neutra: acercarse en zigzag, orbitar, esquivar si toca
//
//  Maquina de estados con tres transiciones:
//  Esquivar — Prioridad maxima: si hay un proyectil en trayectoria de impacto
//  Acercar  — Si la distancia al jugador supera la distancia optima + margen
//  Rodear   — Por defecto: orbitar al jugador a distancia optima

CombatAI::Accion AI_Facil::decide(const std::vector<Disparo>& disparos, float dt) {

    // Vector normalizado hacia el jugador y distancia actual
    Vec2  n = (Jugador.GetPos() - IA.GetPos()).unitario();
    float dist = (Jugador.GetPos() - IA.GetPos()).modulo();
    float dmax{ 10.0f };

    if (Jugador.GetRangoMax() == Rango_bajo)
        dmax *= 0.1f; // Si el jugador es de corto alcance, la IA se acerca mas
    else if (Jugador.GetRangoMax() == Rango_medio)
        dmax *= 0.6f; // Para los demas personajes, mantiene la distancia optima con margen

    // Determinar estado: esquivar tiene prioridad sobre los demas
    Vec2 dir_esq{};
    if (debe_esquivar(disparos, dir_esq))
        estado = Esquivar;
    else if (dist > dmax)
        estado = Acercar;
    else
        estado = Huir;

    Vec2 dir_mov{};

    if (estado == Esquivar) {
        // Moverse en la direccion perpendicular calculada por debe_esquivar()
        dir_mov = dir_esq;
    }
    else if (estado == Acercar) {

        dir_mov = n.unitario();
    }
    else {
        dir_mov = n.unitario() * -1.0f;
    }

    Vec2  toPlayer = (Jugador.GetPos() - IA.GetPos()).unitario();
    Vec2  dir_norm = dir_mov.modulo() > 0 ? dir_mov.unitario() : Vec2{ 0,0 };
    float punteria = dir_norm * toPlayer;  // cuanto se alinea el movimiento con el jugador
    float umbral = aimbot + sinf((float)GetTime() * 7.3f) * 0.03f;

    return { dir_mov, punteria >= umbral };
}