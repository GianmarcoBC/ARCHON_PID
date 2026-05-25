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

    // Nunca esquiva — solo Acercar o Rodear
    if (dist > distancia_combate_optima + rango_seguro)
        estado = Acercar;
    else
        estado = Rodear;

    Vec2 dir_mov{};

    if (estado == Acercar) {
        // Linea recta hacia el jugador sin zigzag: maxima velocidad de aproximacion
        // A diferencia de AI_Facil, no sacrifica velocidad por imprevisibilidad
        dir_mov = n;
    }
    else {
        // Orbita agresiva: predomina el componente tangencial (95%) sobre el radial (20%)
        // El resultado es una orbita rapida que apenas se corrige, manteniendose
        // muy cerca del jugador aunque se salga ligeramente de la distancia optima
        float err = (dist - distancia_combate_optima) / fmaxf(distancia_combate_optima * 0.3f, 1.0f);
        err = fmaxf(-1.0f, fminf(1.0f, err));  // Clamp para evitar correcciones bruscas
        dir_mov = (n * sentido_giro).perp() * 0.95f + n * err * 0.2f;

        // Cambiar sentido de giro periodicamente con intervalo aleatorio
        cont_giro += dt;
        if (cont_giro >= cambio_giro) {
            sentido_giro = -sentido_giro;
            cont_giro = 0;
            cambio_giro =(1.5f + (rand() % 100) / 49.0f);
        }
    }

    // Corregir direccion para no acercarse a menos de 2 unidades de los bordes
    evitar_pared(dir_mov, IA.GetPos(),
        { arena.x, arena.x + arena.width },
        { arena.y, arena.y + arena.height }, 2.0f);

    // Disparar si la punteria supera el umbral (0.85 = cos ~32°):
    // margen de disparo mucho mas amplio que AI_Facil o AI_Defensiva,
    // generando mayor cadencia de fuego a costa de menos precision
    float punteria = IA.GetDir().unitario() * n;
    float umbral = aimbot + sinf((float)GetTime() * 7.3f) * 0.03f;

    return { dir_mov, punteria >= umbral };
}