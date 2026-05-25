#include "AI_Facil.h"

//  AI_Facil — Reaccion lenta (~250ms), estrategia neutra
//
//  Comportamiento: zigzag al acercarse, orbita al
//  llegar a distancia optima, esquiva proyectiles si los detecta.

AI_Facil::AI_Facil(Personaje& IA, Personaje& Jugador, Rectangle arena)
    : CombatAI(IA, Jugador, arena)
{
    delay_IA = 60.0f * 0.25f;  // Intervalo de decision: ~250ms (el mas lento de los tres niveles)
}

//  decide — Estrategia neutra: acercarse en zigzag, orbitar, esquivar si toca
//
//  Maquina de estados con tres transiciones:
//    Esquivar — Prioridad maxima: si hay un proyectil en trayectoria de impacto
//    Acercar  — Si la distancia al jugador supera la distancia optima + margen
//    Rodear   — Por defecto: orbitar al jugador a distancia optima

CombatAI::Accion AI_Facil::decide(const std::vector<Disparo>& disparos, float dt) {

    // Vector normalizado hacia el jugador y distancia actual
    Vec2  n = (Jugador.GetPos() - IA.GetPos()).unitario();
    float dist = (Jugador.GetPos() - IA.GetPos()).modulo();

    // Determinar estado: esquivar tiene prioridad sobre los demas
    Vec2 dir_esq{};
    if (debe_esquivar(disparos, dir_esq))
        estado = Esquivar;
    else if (dist > distancia_combate_optima + rango_seguro)
        estado = Acercar;
    else
        estado = Rodear;

    Vec2 dir_mov{};

    if (estado == Esquivar) {
        // Moverse en la direccion perpendicular calculada por debe_esquivar()
        dir_mov = dir_esq;
    }
    else if (estado == Acercar) {
        // Acercarse con zigzag sinusoidal para ser mas dificil de acertar
        // La amplitud (0.45) y frecuencia (2.5) dan un movimiento suave pero impredecible
        float zigzag = sinf((float)GetTime() * 2.5f) * 0.45f;
        dir_mov = (n + n.perp() * zigzag).unitario();
    }
    else {
        // Orbitar al jugador mezclando movimiento tangencial y correccion radial:
        //   - Tangencial (85%): mantiene la orbita circular
        //   - Radial (45%):     corrige si se aleja o acerca demasiado
        float err = (dist - distancia_combate_optima) / fmaxf(distancia_combate_optima * 0.3f, 1.0f);
        err = fmaxf(-1.0f, fminf(1.0f, err));  // Clamp para evitar correcciones bruscas
        dir_mov = (n * sentido_giro).perp() * 0.85f + n * err * 0.45f;

        // Cambiar sentido de giro periodicamente con intervalo aleatorio
        // para que la orbita no sea completamente predecible
        cont_giro += dt;
        if (cont_giro >= cambio_giro) {
            sentido_giro = -sentido_giro;
            cont_giro = 0;
            cambio_giro = 60.0f * (1.5f + (rand() % 100) / 49.0f);
        }
    }

    // Corregir direccion para no acercarse a menos de 2 unidades de los bordes
    evitar_pared(dir_mov, IA.GetPos(),
        { arena.x, arena.x + arena.width },
        { arena.y, arena.y + arena.height }, 2.0f);

    // Disparar si la punteria supera el umbral:
    // umbral oscila ligeramente con seno para que no sea robotico
    float punteria = IA.GetDir().unitario() * n;
    float umbral = aimbot + sinf((float)GetTime() * 7.3f) * 0.03f;

    return { dir_mov, punteria >= umbral };
}