#include "AI_Agresiva.h"

//  AI_Agresiva — Reaccion rapida (~40ms), presion constante
//
//  Ignora proyectiles entrantes: nunca esquiva.
//  Mantiene distancia optima menor para estar siempre encima del jugador.
//  Dispara con punteria mas laxa — prefiere cadencia a precision.

AI_Agresiva::AI_Agresiva(Personaje& IA, Personaje& Jugador, Rectangle arena)
    : CombatAI(IA, Jugador, arena)
{
    delay_IA = 0.04f;       // Intervalo de decision: ~40ms (el mas rapido de los tres niveles)
    distancia_combate_optima *= 0.6f; // Reduce la distancia optima para mantenerse pegado al jugador
    aimbot = 0.85f;                  // Umbral de punteria bajo: dispara aunque no apunte perfecto
}

//  decide — Estrategia agresiva: presion maxima, sin esquiva
//
//  Maquina de estados reducida a dos transiciones (nunca entra en Esquivar):
//    Acercar — Si la distancia supera la distancia optima + margen
//    Rodear  — Por defecto: orbitar muy cerca del jugador
//
//  Al ignorar los proyectiles entrantes, la IA acepta recibir impactos
//  a cambio de mantener presion constante sobre el jugador.

CombatAI::Accion AI_Agresiva::decide(const std::vector<Disparo>& disparos, float dt) {

    // Vector normalizado hacia el jugador y distancia actual
    // 'disparos' se recibe por firma comun pero no se usa: esta IA no esquiva
    Vec2  n = (Jugador.GetPos() - IA.GetPos()).unitario();
    float dist = (Jugador.GetPos() - IA.GetPos()).modulo();
    float dmax{5.0f};

    if (Jugador.GetRangoMax() == Rango_bajo)
        dmax *= 0.8f; // Si el jugador es de corto alcance, la IA se acerca mas
    else if (Jugador.GetRangoMax() == Rango_medio)
        dmax *= 0.9f; // Para los demas personajes, mantiene la distancia optima con margen

    // Nunca esquiva — solo Acercar o Rodear
    if (dist > dmax)
        estado = Acercar;
    else
        estado = Huir;

    Vec2 dir_mov{};

    if (estado == Acercar) {
        // Linea recta hacia el jugador sin zigzag: maxima velocidad de aproximacion
        // A diferencia de AI_Facil, no sacrifica velocidad por imprevisibilidad
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