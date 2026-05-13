#include "CombatAI.h"

// ============================================================================
//  Pared_cerc — Calcula la distancia a la pared mas cercana
//
//  Compara la distancia del punto 'pos' a cada uno de los 4 bordes de la arena
//  y devuelve la menor. Se usa para decidir hacia donde esquivar sin chocar.
// ============================================================================

float CombatAI::Pared_cerc(Vec2 pos) const {
    return fminf(fminf(pos.x - arena.x, arena.x + arena.width  - pos.x),
                 fminf(pos.y - arena.y, arena.y + arena.height - pos.y));
}

// ============================================================================
//  evitar_pared — Corrige el vector de movimiento para no chocar con las paredes
//
//  Si la IA esta a menos de 'margin' unidades de un borde, invierte la
//  componente correspondiente del vector de movimiento para alejarse.
// ============================================================================

void CombatAI::evitar_pared(Vec2& v, Vec2 pos, Vec2 lim_x, Vec2 lim_y, float margin) {
    if (pos.x - lim_x.x < margin) v.x =  fabsf(v.x);  // Muy cerca del borde izquierdo → ir a la derecha
    if (lim_x.y - pos.x < margin) v.x = -fabsf(v.x);  // Muy cerca del borde derecho → ir a la izquierda
    if (pos.y - lim_y.x < margin) v.y =  fabsf(v.y);  // Muy cerca del borde inferior → ir hacia arriba
    if (lim_y.y - pos.y < margin) v.y = -fabsf(v.y);  // Muy cerca del borde superior → ir hacia abajo
}

// ============================================================================
//  debe_esquivar — Detecta proyectiles peligrosos y calcula direccion de escape
//
//  Para cada disparo del jugador que este activo:
//  1. Calcula el vector desde el disparo hacia la IA
//  2. Si el disparo se aleja (producto punto < 0), lo ignora
//  3. Si la distancia perpendicular al disparo es mayor que rango_esquiva, lo ignora
//  4. Si es peligroso, elige esquivar perpendicularmente al disparo,
//     eligiendo el lado que deje mas distancia a las paredes
// ============================================================================

bool CombatAI::debe_esquivar(const std::vector<Disparo>& disparos, Vec2& outDir) {
    for (const Disparo& d : disparos) {
        if (!d.isFromPlayer()) continue;  // Solo esquivar disparos del jugador

        Vec2  Vector_IA_Disparo = IA.GetPos() - d.GetPos();
        Vec2  pn = d.GetVel().unitario();  // Direccion normalizada del proyectil

        // Si el disparo se aleja de la IA, ignorar
        if (Vector_IA_Disparo * pn < 0) continue;

        // Si la distancia perpendicular es mayor que el rango de esquiva, ignorar
        if (fabsf(Vector_IA_Disparo.cross(pn)) > rango_esquiva) continue;

        // Calcular las dos direcciones perpendiculares posibles
        Vec2  d1 = { -pn.y,  pn.x };   // Perpendicular izquierda
        Vec2  d2 = {  pn.y, -pn.x };   // Perpendicular derecha

        // Elegir la direccion que deje mas espacio respecto a las paredes
        float speed = IA.GetVelocidad();
        float s1 = Pared_cerc(IA.GetPos() + d1 * speed * 4);  // Simular 4 segundos en esa direccion
        float s2 = Pared_cerc(IA.GetPos() + d2 * speed * 4);
        outDir = (s1 >= s2) ? d1 : d2;
        return true;  // Hay que esquivar
    }
    return false;  // No hay disparos peligrosos
}

// ============================================================================
//  decide — Toma la decision de movimiento y disparo
//
//  Maquina de estados:
//    Esquivar — Si hay un proyectil peligroso, moverse perpendicularmente
//    Acercar  — Si esta lejos del jugador, acercarse con movimiento en zigzag
//    Rodear   — Si esta a distancia optima, orbitar al jugador cambiando
//               sentido de giro aleatoriamente
//
//  Despues de calcular la direccion, aplica correccion para evitar paredes.
//  Finalmente, evalua si la punteria es suficiente para disparar (producto punto
//  entre la direccion de apuntado y la direccion al jugador >= umbral).
// ============================================================================

CombatAI::Accion CombatAI::decide(const std::vector<Disparo>& disparos, float dt) {

    // Calcular vector y distancia al jugador
    Vec2  Vector_IA_Jugador = Jugador.GetPos() - IA.GetPos();
    float dist = Vector_IA_Jugador.modulo();
    Vec2  n    = Vector_IA_Jugador.unitario();  // Direccion normalizada hacia el jugador

    // Verificar si hay que esquivar algun disparo
    Vec2  dir_esq{};
    bool  ESQUIVA = debe_esquivar(disparos, dir_esq);

    // Transicion de estados
    if (ESQUIVA)
        estado = Esquivar;
    else if (dist > distancia_combate_optima + rango_seguro)
        estado = Acercar;   // Muy lejos → acercarse
    else
        estado = Rodear;    // A distancia optima → orbitar

    Vec2 dir_mov{};

    if (estado == Esquivar) {
        // Moverse perpendicular al disparo peligroso
        dir_mov = dir_esq;
    }
    else if (estado == Acercar) {
        // Acercarse al jugador con zigzag sinusoidal para ser dificil de acertar
        float zigzag = sinf((float)GetTime() * 2.5f) * 0.45f;
        dir_mov = (n + n.perp() * zigzag).unitario();
    }
    else {
        // Rodear: orbitar al jugador manteniendo la distancia optima
        Vec2  perp = (n * sentido_giro).perp();

        // Error de distancia: positivo = demasiado lejos, negativo = demasiado cerca
        float err  = (dist - distancia_combate_optima) / fmaxf(distancia_combate_optima * 0.3f, 1.0f);
        err = fmaxf(-1.0f, fminf(1.0f, err));  // Clamp entre -1 y 1

        // Mezclar movimiento orbital (85%) con correccion radial (45%)
        dir_mov = (n * sentido_giro).perp() * 0.85f + n * err * 0.45f;

        // Cambiar sentido de giro periodicamente (aleatorio)
        cont_giro += dt;
        if (cont_giro >= cambio_giro) {
            sentido_giro = -sentido_giro;
            cont_giro = 0;
            cambio_giro = 60.0f * (1.5f + (rand() % 100) / 49.0f);
        }
    }

    // Aplicar correccion para no acercarse demasiado a las paredes
    float margin = 2.0f;  // Margen de 2 unidades respecto a los bordes
    evitar_pared(dir_mov, IA.GetPos(),
                 { arena.x, arena.x + arena.width },
                 { arena.y, arena.y + arena.height }, margin);

    // Evaluar si disparar: comparar direccion de apuntado con direccion al jugador
    float t = (float)GetTime();
    Vec2  apuntar  = IA.GetDir();
    float punteria = apuntar.unitario() * n;  // Producto punto (1.0 = apuntando perfecto)
    float umbral   = aimbot + sinf(t * 7.3f) * 0.03f;  // Pequena oscilacion para variabilidad
    bool  fire     = (punteria >= umbral);

    return { dir_mov, fire };
}

// ============================================================================
//  Update — Ciclo principal de la IA
//
//  1. Reduce el cooldown de decision. Cuando llega a 0, toma una nueva decision.
//  2. Si hay movimiento, actualiza la direccion de apuntado hacia el jugador
//     y mueve al personaje IA.
//  3. Devuelve true si la IA quiere disparar (arena se encarga de crear el Disparo).
// ============================================================================

bool CombatAI::Update(float dt, const std::vector<Disparo>& disparos) {

    // Reducir cooldown de decision
    if (cooldown_IA > 0) { cooldown_IA -= dt; }
    else {
        // Tomar nueva decision y reiniciar cooldown
        ultima_accion = decide(disparos, dt);
        cooldown_IA   = delay_IA;
    }

    // Ejecutar la ultima decision tomada
    if (ultima_accion.d.x != 0 || ultima_accion.d.y != 0) {
        // Actualizar direccion de apuntado hacia el jugador (corrige bug original de ARCHON)
        Vec2 toPlayer = (Jugador.GetPos() - IA.GetPos()).unitario();
        IA.SetDir(toPlayer);

        // Mover al personaje IA
        IA.SetPos(IA.GetPos() + ultima_accion.d * IA.GetVelocidad() * dt);
    }

    return ultima_accion.disparar;
}
