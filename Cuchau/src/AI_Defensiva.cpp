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
    delay_IA = 60.0f * 0.12f;        // Intervalo de decision: ~120ms (nivel medio entre Facil y Agresiva)
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

    if (esquiva || demasiado_cerca) {
        estado = Esquivar;
        if (demasiado_cerca && !esquiva)
            dir_esq = (n * -1.0);   // Sin proyectil peligroso: retroceder directamente alejandose del jugador
        // Si hay proyectil, dir_esq ya viene calculada por debe_esquivar()
    }
    else if (dist > distancia_combate_optima + rango_seguro)
        estado = Acercar;
    else
        estado = Rodear;

    Vec2 dir_mov{};

    if (estado == Esquivar) {
        // Usar la direccion de escape calculada (perpendicular al proyectil o alejarse del jugador)
        dir_mov = dir_esq;
    }
    else if (estado == Acercar) {
        // Zigzag mas pronunciado que AI_Facil (amplitud 0.7 vs 0.45, frecuencia 3.0 vs 2.5)
        // Al tener que acercarse desde mas lejos, necesita ser mas impredecible durante el acercamiento
        float zigzag = sinf((float)GetTime() * 3.0f) * 0.7f;
        dir_mov = (n + n.perp() * zigzag).unitario();
    }
    else {
        // Orbita con mayor peso radial (80%) que tangencial (60%):
        // prioriza mantener la distancia optima sobre la velocidad de orbita,
        // al contrario que AI_Agresiva que hace lo opuesto
        float err = (dist - distancia_combate_optima) / fmaxf(distancia_combate_optima * 0.3f, 1.0f);
        err = fmaxf(-1.0f, fminf(1.0f, err));  // Clamp para evitar correcciones bruscas
        dir_mov = (n * sentido_giro).perp() * 0.6f + n * err * 0.8f;

        // Cambiar sentido de giro periodicamente con intervalo aleatorio
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

    // Disparar solo si la punteria supera el umbral (0.97 = cos ~14°):
    // el margen de disparo mas estrecho de los tres niveles genera
    // menos disparos pero mas precisos
    float punteria = IA.GetDir().unitario() * n;
    float umbral = aimbot + sinf((float)GetTime() * 7.3f) * 0.03f;

    return { dir_mov, punteria >= umbral };
}