#pragma once
#include "raylib.h"
#include "Disparo.h"
#include "Pj_info.h"
#include "Vec2.h"

// ============================================================================
//  Personaje.h — Clase unificada de personaje 3D
//
//  Combina la logica de juego de ARCHON (vida, disparo, controles, animacion)
//  con el renderizado 3D de Cuchau (billboard + sombra en el suelo).
//
//  Posicion interna: Vector3 pos3d (3D para rendering).
//  Logica de juego:  Vec2 en plano XZ (GetPos/SetPos).
//  Conversion:       Vec2.x = pos3d.x, Vec2.y = pos3d.z
// ============================================================================

class Personaje
{
    friend class arena;  // arena accede directamente a miembros para el rendering

    Pj_info Player{};             // Datos del personaje (stats, rutas de sprites, etc.)
    float max_vida{};             // Vida maxima (para calcular porcentaje en barras de vida)
    cntrl Controles{};            // Teclas de movimiento asignadas
    bool  isPlayer{ true };       // true = controlado por teclado, false = controlado por IA

    // --- Posicion y direccion ---
    Vector3 pos3d{};              // Posicion 3D (x = lateral, y = altura billboard, z = profundidad)
    Vec2    l_dir{ 1.0f, 0.0f }; // Ultima direccion de movimiento en plano XZ (para apuntar disparos)

    // --- Constantes de renderizado y escala ---
    static constexpr float charSize    = 4.0f;   // Tamano del billboard en unidades 3D
    static constexpr float SPEED_SCALE = 0.02f;  // Factor de conversion: pixeles/s -> unidades/s
                                                  // (arena 40u / pantalla ~2000px ≈ 0.02)

    // --- Texturas y modelos ---
    Texture2D Frames[3]{};          // 3 frames de animacion del personaje
    Texture2D Frames_shadow[3]{};   // 3 frames de sombra correspondientes
    Texture2D Ataque{};             // Textura del proyectil que dispara este personaje
    Mesh  shadowMesh[3]{};          // Mallas planas para las sombras (una por frame)
    Model shadow[3]{};              // Modelos de sombra (malla + textura)
    Sound efecto_ataque{};          // Sonido que se reproduce al disparar

    // --- Animacion ---
    int   frameActual = 0;          // Indice del frame actual (0, 1 o 2)
    float frameTimer  = 0.0f;       // Acumulador de tiempo para cambiar de frame
    bool  moviendose{ false };      // true si el personaje se esta moviendo (para animar)

public:
    Personaje() = default;

    // Constructor: recibe datos del personaje, controles, posicion inicial, y si es jugador
    Personaje(Pj_info p, cntrl c, Vector3 po, bool ip);

    // Actualiza movimiento (si es jugador) y animacion
    void Update(float dt);

    // Dibuja la sombra del frame actual en la posicion dada
    void drawshadow(Vector3 shadowpos);

    // Devuelve la textura del frame actual de animacion
    Texture2D getCurrentFrame() const { return Frames[frameActual]; }

    // --- Getters y setters ---

    float       GetFuerza()      const { return Player.fuerza; }         // Dano por impacto
    Vec2        GetPos()         const { return { pos3d.x, pos3d.z }; }  // Posicion en plano XZ
    void        SetPos(Vec2 p)         { pos3d.x = p.x; pos3d.z = p.y; }// Asigna posicion XZ
    Vector3     GetPos3D()       const { return pos3d; }                 // Posicion 3D completa
    void        SetPos3D(Vector3 p)    { pos3d = p; }                    // Asigna posicion 3D
    float       GetVida()        const { return Player.vida; }           // Vida actual
    void        SetVida(float v)       { Player.vida = v; }              // Asigna vida (para cargar partida)
    float       GetMaxVida()     const { return max_vida; }              // Vida maxima
    Vec2        GetDir()         const { return l_dir; }                 // Direccion de apuntado
    void        SetDir(Vec2 d)         { l_dir = d.unitario(); }         // Cambia direccion (normalizada)
    float       GetVelocidad()   const { return Player.vel * SPEED_SCALE; }  // Velocidad en u/s
    float       get_Cooldown()   const { return Player.cooldown; }       // Tiempo entre disparos
    bool        get_isPlayer()   const { return isPlayer; }              // Es controlado por teclado?
    void        set_isPlayer(bool ip)  { isPlayer = ip; }
    const char* GetNombre()      const { return Player.nombre; }         // Nombre del personaje
    float       GetCharSize()    const { return charSize; }              // Tamano del billboard
    Texture2D*  GetAtaqueTexture()     { return &Ataque; }               // Textura del proyectil
    float       GetAttackSpeed() const { return Player.attack_speed * SPEED_SCALE; } // Vel. proyectil en u/s

    // Recibe dano y reduce vida (minimo 0)
    void    pain(float damage);

    // Crea y devuelve un Disparo en la posicion del personaje, en su direccion de apuntado
    Disparo Shoot();

    // Reproduce el sonido de ataque del personaje
    void    PlayAttackSound();

    // Libera todas las texturas, modelos y sonidos de GPU/memoria
    void    UnloadPersonaje();
};
