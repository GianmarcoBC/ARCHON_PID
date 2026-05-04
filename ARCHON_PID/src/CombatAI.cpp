#pragma once
#include "CombatAI.h"

float CombatAI::Pared_cerc(Vec2 pos) const {
    return fminf(fminf(pos.x - arena.x, arena.x + arena.width - pos.x),
        fminf(pos.y - arena.y, arena.y + arena.height - pos.y));
}

void CombatAI::evitar_pared(Vec2 &v, Vec2 pos, Vec2 lim_x, Vec2 lim_y, float margin) {
    if (pos.x - lim_x.x < margin) v.x =  fabsf(v.x);
    if (lim_x.y - pos.x < margin) v.x = -fabsf(v.x);
    if (pos.y - lim_y.x < margin) v.y =  fabsf(v.y);
    if (lim_y.y - pos.y < margin) v.y = -fabsf(v.y);
}


bool CombatAI::debe_esquivar(const std::vector<Disparo>& disparos, Vec2 &outDir){

    for (const Disparo& d : disparos) {
        if (!d.isFromPlayer()) continue;   // Solo esquivar disparos del jugador.

        Vec2  Vector_IA_Disparo = IA.GetPos() - d.GetPos(); // Vector desde el disparo hacia la IA.
        Vec2  pn = d.GetVel().unitario(); // Vector unitario en la dirección del disparo.
        if (Vector_IA_Disparo * pn < 0) continue; // Ya nos pasó de largo.

        if (fabsf(Vector_IA_Disparo.cross(pn)) > rango_esquiva) continue; // El disparo no va directo hacia nosotros.

        // Elegir la perpendicular que aleja de las paredes.
        // Se evalúan las dos perpendiculares.
        Vec2  d1 = { -pn.y,  pn.x };
        Vec2  d2 = { pn.y, -pn.x };
        float speed = IA.GetVelocidad();
        float s1 = Pared_cerc(IA.GetPos() + d1 * speed * 4); //Evaluar qué tan cerca de la pared quedamos si esquivamos en esa dirección.
        float s2 = Pared_cerc(IA.GetPos() + d2 * speed * 4); //Evaluar qué tan cerca de la pared quedamos si esquivamos en esa dirección.
        Vec2 best = (s1 >= s2) ? d1 : d2;
        outDir = best;
        return true;
    }
    return false;
}

CombatAI::Accion CombatAI::decide(const std::vector<Disparo>& disparos, float dt) {

    Vec2  Vector_IA_Jugador = Jugador.GetPos() - IA.GetPos(); // Vector desde la IA hacia el jugador.
    float dist = Vector_IA_Jugador.modulo(); // Distancia entre la IA y el jugador.
    Vec2  n = Vector_IA_Jugador.unitario(); // Vector unitario en la dirección del jugador.

    // 1. ¿Hay que esquivar?
    Vec2 dir_esq{};
    bool  ESQUIVA = debe_esquivar(disparos, dir_esq); //Calcula si debe esquiavar y en qué dirección. 

    // 2. Transición de estado
    if (ESQUIVA) //Se prioriza esquivar sobre cualquier otra acción.
        estado = Esquivar;
    else if (dist > distancia_combate_optima + rango_seguro) // Si estamos muy lejos, acercarse.
        estado = Acercar;
    else // Si estamos en el rango óptimo, rodear.
        estado = Rodear;

    // 3. Dirección de movimiento
    Vec2 dir_mov{};

    if (estado == Esquivar) {
        dir_mov = dir_esq; // Si se detectó un disparo peligroso, moverse en la dirección de esquiva calculada.
    }
    else if (estado == Acercar) {
        float zigzag = sinf((float)GetTime() * 2.5f) * 0.45f; // Oscilación para hacer zigzag, ajusta frecuencia y amplitud.
        Vec2  dir = (n + n.perp() * zigzag).unitario(); // Combina la dirección hacia el jugador con un componente perpendicular para zigzaguear.
        dir_mov = dir;
    }
    else { //Rodear.
        Vec2  perp = (n * sentido_giro).perp(); // Perpendicular a la dirección del jugador, multiplicado por el sentido de giro actual.

        float err = (dist - distancia_combate_optima) / fmaxf(distancia_combate_optima * 0.3f, 1.0f); // Error normalizado de la distancia al rango óptimo. Si es positivo, estamos fuera del rango ideal y queremos acercarnos; si es negativo, queremos alejarnos.
        // El 0.3f es la banda de tolerancia alrededor del rango óptimo donde no se prioriza acercarse o alejarse, solo rodear. Se evita dividir por cero con fmaxf.

        err = fmaxf(-1.0f, fminf(1.0f, err)); // Limitar el error para evitar movimientos erráticos cuando estamos muy lejos o muy cerca.

        dir_mov = (n * sentido_giro).perp() * 0.85f + n * err * 0.45f; // Combina el movimiento de rodeo con un componente hacia/alejándose del jugador según si estamos fuera/dentro del rango óptimo.
        //0.85f y 0.45f son pesos para ajustar la influencia del rodeo vs acercarse/alejarse.

        // Cambiar sentido de giro periódicamente para ser impredecible.
        cont_giro+=dt;
        if (cont_giro >= cambio_giro) {
            sentido_giro = -sentido_giro;
            cont_giro = 0;
            cambio_giro = 60.0f * (1.5f + (rand() % 100) / 49.0f);
        }
    }

    // 4. Separarse de las paredes
    float margin = 20.0f; // Distancia mínima a las paredes para empezar a evitar.  
    evitar_pared(dir_mov, IA.GetPos(), {arena.x, arena.x + arena.width}, {arena.y, arena.y + arena.height}, margin);

    // 6. Disparar si la puntería es suficiente
    float t = (float)GetTime();
    Vec2  apuntar = IA.GetDir();
    float punteria = apuntar.unitario() * n; // Producto escalar entre la dirección en la que la IA está apuntando y la dirección hacia el jugador. Esto da un valor entre -1 y 1, donde 1 significa que estamos apuntando exactamente al jugador.
    float umbral_disparo = aimbot + sinf(t * 7.3f) * 0.03f; // Umbral dinámico para decidir disparar, con una pequeña oscilación para que también falle.
    bool  fire = (punteria >= umbral_disparo); // Disparar si el producto escalar es mayor o igual al umbral. Esto significa que la IA solo disparará cuando esté apuntando suficientemente cerca del jugador, con un margen de error controlado por el umbral.

    return { dir_mov, fire };
}

bool CombatAI::Update(float dt, const std::vector<Disparo>& disparos) {

    // Recalcular la acción de la IA solo si el cooldown ha terminado. Esto simula un delay de reacción y evita que la IA cambie de dirección o decida disparar cada frame, lo que sería poco realista y demasiado difícil de enfrentar.
    if (cooldown_IA > 0) { cooldown_IA -= dt; }
    else {
        ultima_accion = decide(disparos, dt);
        cooldown_IA = delay_IA;
    }

    // Mover al personaje en la dirección calculada
    if (ultima_accion.d.x != 0 || ultima_accion.d.y != 0) {
        IA.SetPos(IA.GetPos() + ultima_accion.d * IA.GetVelocidad() * dt);
    }

    return ultima_accion.disparar;
}