#pragma once
#include "raylib.h"
#include "Disparo.h"
#include "Pj_info.h"
#include "Vec2.h"

//  Personaje.h — Clase unificada de personaje 3D
//
//  Combina la logica de juego de ARCHON (vida, disparo, controles, animacion)
//  con el renderizado 3D de Cuchau (billboard + sombra en el suelo).
//
//  Posicion interna: Vector3 pos3d (3D para rendering).
//  Logica de juego:  Vec2 en plano XZ (GetPos/SetPos).
//  Conversion:       Vec2.x = pos3d.x, Vec2.y = pos3d.z

class Personaje
{
    Pj_info Player{};             // Datos del personaje (stats, rutas de sprites, etc.)
    float max_vida{};             // Vida maxima (para calcular porcentaje en barras de vida)
	float max_cooldown{};          // Cooldown de disparo (para calcular porcentaje en barra de cooldown)
    cntrl Controles{};            // Teclas de movimiento asignadas
    bool  isPlayer{ true };       // true = controlado por teclado, false = controlado por IA

    // --- Posicion y direccion ---
    Vector3 pos3d{};              // Posicion 3D (x = lateral, y = altura billboard, z = profundidad)
    Vec2    l_dir{ 1.0f, 0.0f }; // Ultima direccion de movimiento en plano XZ (para apuntar disparos)

    // --- Constantes de renderizado y escala ---
    static constexpr float Size3D    = 4.0f;   // Tamano del billboard en unidades 3D
    static constexpr float SPEED_SCALE = 0.02f;  // Factor de conversion: pixeles/s -> unidades/s
                                                  // (arena 40u / pantalla ~2000px ≈ 0.02)

    // --- Texturas y modelos ---
    std::vector<Texture2D> Frames{};          // frames de animacion del personaje

    std::vector<Texture2D> Frames_shadow{};   // frames de sombra correspondientes
    std::vector<Mesh> shadowMesh{};          // Mallas planas para las sombras (una por frame)
    std::vector<Model> shadow{};              // Modelos de sombra (malla + textura)

    Texture2D Ataque{};             // Textura del proyectil que dispara este personaje
    Sound efecto_ataque{};          // Sonido que se reproduce al disparar

    // --- Animacion ---
	int   frameActual = 0; 	 // Frame actual de animacion (index en Frames y shadow)    
    float frameTimer  = 0.0f;       // Acumulador de tiempo para cambiar de frame
    bool  moviendose{ false };      // true si el personaje se esta moviendo (para animar)

    // Dibuja la sombra del frame actual en la posicion dada
    void drawshadow(Vector3 shadowpos) const;

	// Dibuja la vida, el cooldown y el nombre del personaje sobre su cabeza (en coordenadas 2D)
	void drawHUD(Camera camera, Color color) const; // Dibuja la barra de vida, el cooldown y el nombre del personaje sobre su cabeza (en coordenadas 2D)

public:
    Personaje() = default;

    // Constructor: recibe datos del personaje, controles, posicion inicial, y si es jugador
    Personaje(Pj_info p, cntrl c, Vector3 po, bool ip);

	// Mueve el personaje en la direccion dada por el jugador real
    void Move(Vec2 dir, float dt);
    
    // Actualiza movimiento y animacion
    void Update(float dt);

	void Draw(Camera camera, Color color) const;

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
    //float       GetVelocidad()   const { return Player.vel * SPEED_SCALE; }  // Velocidad en u/s
    float       get_Cooldown()   const { return Player.cooldown; }       // Tiempo entre disparos
    bool        get_isPlayer()   const { return isPlayer; }              // Es controlado por teclado?
    //void        set_isPlayer(bool ip)  { isPlayer = ip; }
    //std::string_view GetNombre()      const { return Player.nombre; }         // Nombre del personaje
    //float       GetCharSize()    const { return Size3D; }              // Tamano del billboard
    //Texture2D*  GetAtaqueTexture()     { return &Ataque; }               // Textura del proyectil
    //float       GetAttackSpeed() const { return Player.attack_speed * SPEED_SCALE; } // Vel. proyectil en u/s

    // Recibe dano y reduce vida (minimo 0)
    void    pain(float damage);

    // Crea y devuelve un Disparo en la posicion del personaje, en su direccion de apuntado
    Disparo Shoot();

    // Reproduce el sonido de ataque del personaje
    void    PlayAttackSound();

    // Libera todas las texturas, modelos y sonidos de GPU/memoria
    ~Personaje() {
        for (int i = 0; i < Frames.size(); i++) {
            UnloadModel(shadow[i]);
            UnloadTexture(Frames_shadow[i]);
            UnloadTexture(Frames[i]);
        }
        UnloadTexture(Ataque);
        UnloadSound(efecto_ataque);
    }
};
