#include "AI_Defensiva.h"

// ============================================================================
//  AI_Defensiva — Reaccion media (~120ms), prioriza distancia y esquiva
//
//  Mantiene una distancia optima mayor que la base.
//  Si el jugador se acerca demasiado, retrocede activamente.
//  Solo dispara con punteria mas exigente.
// ============================================================================

AI_Defensiva::AI_Defensiva(Personaje& IA, Personaje& Jugador, Rectangle arena)
    : CombatAI(IA, Jugador, arena)
{
    delay_IA = 0.12f;        // Intervalo de decision: ~120ms (nivel medio entre Facil y Agresiva)
    distancia_combate_optima *= 1.3f; // Aumenta la distancia optima: prefiere combatir desde lejos
    aimbot = 0.97f;                   // Umbral de punteria alto: solo dispara apuntando casi perfecto
}

// ============================================================================
//  decide — Estrategia defensiva: mantener distancia, esquivar y retroceder
//
//  Maquina de estados con cuatro transiciones (Esquivar tiene doble causa):
//    Esquivar (proyectil) — Hay un disparo en trayectoria de impacto
//    Esquivar (invasion)  — El jugador se ha acercado por debajo del 60% de la distancia optima
//    Acercar             — La distancia supera la distancia optima + margen
//    Rodear              — Por defecto: orbitar manteniendo distancia optima mayor
//
//  La combinacion de distancia optima alta y retroceso activo hace que esta IA
//  sea dificil de alcanzar en cuerpo a cuerpo, compensando su baja cadencia de fuego.
// ============================================================================

CombatAI::Accion AI_Defensiva::decide(const std::vector<Disparo>& disparos, float dt) {

    // Vector normalizado hacia el jugador y distancia actual
    Vec2  n = (Jugador.GetPos() - IA.GetPos()).unitario();
    float dist = (Jugador.GetPos() - IA.GetPos()).modulo();

    // Evaluar ambas condiciones de esquiva por separado para elegir la direccion correcta
    Vec2 dir_esq{};
    bool esquiva = debe_esquivar(disparos, dir_esq);
    bool demasiado_cerca = dist < distancia_combate_optima * 0.6f;  // Jugador demasiado cerca
    float dmax{ 15.0f };

    if (Jugador.GetRangoMax() == Rango_bajo)
        dmax *= 0.2f; // Si el jugador es de corto alcance, la IA se acerca mas
    else if (Jugador.GetRangoMax() == Rango_medio)
        dmax *= 0.4f; // Para los demas personajes, mantiene la distancia optima con margen

    if (esquiva || demasiado_cerca) {
        estado = Esquivar;
        if (demasiado_cerca && !esquiva)
            dir_esq = (n * -1.0);   // Sin proyectil peligroso: retroceder directamente alejandose del jugador
        // Si hay proyectil, dir_esq ya viene calculada por debe_esquivar()
    }
    else if (dist > dmax)
        estado = Acercar;
    else
        estado = Huir;

    Vec2 dir_mov{};

    if (estado == Esquivar) {
        // Usar la direccion de escape calculada (perpendicular al proyectil o alejarse del jugador)
        dir_mov = dir_esq;
    }
    else if (estado == Acercar) {
        // Al tener que acercarse desde mas lejos, necesita ser mas impredecible durante el acercamiento
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