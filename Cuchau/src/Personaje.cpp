#include "Personaje.h"
#include <cmath>

// ============================================================================
//  Constructor — Inicializa un personaje con sus recursos graficos y de audio
//
//  Carga los 3 frames de animacion, los 3 frames de sombra, la textura del
//  proyectil y el sonido de ataque. Genera 3 modelos planos para las sombras.
// ============================================================================

Personaje::Personaje(Pj_info p, cntrl c, Vector3 po, bool ip)
{
    Player    = p;
    max_vida  = p.vida;       // Guardar vida maxima antes de que se reduzca
    Controles = c;
    pos3d     = po;
    isPlayer  = ip;

    // Cargar los 3 frames de animacion del personaje
    Frames[0] = LoadTexture(Player.Sprite_1);
    Frames[1] = LoadTexture(Player.Sprite_2);
    Frames[2] = LoadTexture(Player.Sprite_3);

    // Cargar los 3 frames de sombra correspondientes
    Frames_shadow[0] = LoadTexture(Player.Sprite_1_shadow);
    Frames_shadow[1] = LoadTexture(Player.Sprite_2_shadow);
    Frames_shadow[2] = LoadTexture(Player.Sprite_3_shadow);

    // Cargar textura del proyectil y sonido de ataque
    Ataque = LoadTexture(Player.Ataque);
    efecto_ataque = LoadSound(Player.Efecto_ataque);

    // Generar modelos planos para las sombras (uno por frame de animacion)
    // Cada sombra es un plano de charSize x charSize con la textura de sombra
    for (int i = 0; i < 3; i++) {
        shadowMesh[i] = GenMeshPlane(charSize, charSize, 1, 1);
        shadow[i]     = LoadModelFromMesh(shadowMesh[i]);
        shadow[i].materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = Frames_shadow[i];
    }
}

// ============================================================================
//  Update — Procesa input de teclado y actualiza animacion
//
//  Solo se ejecuta si isPlayer==true (la IA controla el movimiento externamente).
//  Lee las teclas asignadas, mueve en el plano XZ, actualiza la direccion de
//  apuntado y avanza la animacion de sprites.
// ============================================================================

void Personaje::Update(float dt)
{
    if (!isPlayer) return;   // IA controla externamente

    moviendose = false;
    float spd = GetVelocidad();  // Velocidad escalada a unidades 3D

    // Leer teclas y mover en el plano XZ
    // Cada tecla actualiza la direccion de apuntado (l_dir)
    if (IsKeyDown(Controles.left))  { pos3d.x -= spd * dt; l_dir = {-1,  0}; moviendose = true; }
    if (IsKeyDown(Controles.right)) { pos3d.x += spd * dt; l_dir = { 1,  0}; moviendose = true; }
    if (IsKeyDown(Controles.up))    { pos3d.z -= spd * dt; l_dir = { 0, -1}; moviendose = true; }
    if (IsKeyDown(Controles.down))  { pos3d.z += spd * dt; l_dir = { 0,  1}; moviendose = true; }

    // Avanzar animacion si se esta moviendo
    if (moviendose) {
        frameTimer += dt;
        if (frameTimer >= Player.frameSpeed) {
            frameTimer = 0.0f;
            frameActual = (frameActual + 1) % Player.frameCount;
        }
    }
    // Si esta quieto, volver al frame 0
    else {
        frameActual = 0;
        frameTimer  = 0.0f;
    }
}

// ============================================================================
//  drawshadow — Dibuja la sombra del personaje en el suelo
//
//  Usa BLEND_MULTIPLIED para que la sombra se mezcle con el suelo
//  oscureciendolo sin taparlo completamente.
// ============================================================================

void Personaje::drawshadow(Vector3 shadowpos)
{
    BeginBlendMode(BLEND_MULTIPLIED);
    DrawModel(shadow[frameActual], shadowpos, 1.0f, WHITE);
    EndBlendMode();
}

// ============================================================================
//  pain — Aplica dano al personaje
//
//  Reduce la vida por la cantidad de dano recibido. La vida no baja de 0.
// ============================================================================

void Personaje::pain(float damage)
{
    if (Player.vida > damage)
        Player.vida -= damage;
    else
        Player.vida = 0;
}

// ============================================================================
//  Shoot — Crea un disparo desde la posicion del personaje
//
//  El proyectil aparece desplazado medio charSize en la direccion de apuntado
//  (para que salga del borde del billboard, no del centro).
//  La velocidad del proyectil es la direccion de apuntado * velocidad de ataque.
// ============================================================================

Disparo Personaje::Shoot()
{
    float offset = charSize / 2.0f;
    Vec2  pos2d  = GetPos() + l_dir * offset;                  // Posicion de origen en XZ
    Vector3 origin = { pos2d.x, charSize / 2.0f, pos2d.y };   // Convertir a 3D (y = altura del billboard)
    Vec2    vel    = l_dir * GetAttackSpeed();                  // Velocidad del proyectil en XZ
    return Disparo(origin, vel, &Ataque, isPlayer);
}

// ============================================================================
//  PlayAttackSound — Reproduce el efecto de sonido de ataque
// ============================================================================

void Personaje::PlayAttackSound()
{
    SetSoundVolume(efecto_ataque, 3.5f);
    PlaySound(efecto_ataque);
}

// ============================================================================
//  UnloadPersonaje — Libera todos los recursos de GPU y audio
//
//  Descarga modelos de sombra, texturas de animacion, textura de ataque
//  y el sonido de ataque. Se llama al destruir la arena.
// ============================================================================

void Personaje::UnloadPersonaje()
{
    for (int i = 0; i < 3; i++) {
        UnloadModel(shadow[i]);
        UnloadTexture(Frames_shadow[i]);
        UnloadTexture(Frames[i]);
    }
    UnloadTexture(Ataque);
    UnloadSound(efecto_ataque);
}
