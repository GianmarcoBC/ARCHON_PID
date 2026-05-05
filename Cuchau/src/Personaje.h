#pragma once
#include "raylib.h"
#include "Disparo.h"
#include "Pj.h"
#include "Vec2.h"

class Personaje
{
	// Parámetros del personaje
    Pj Player{};

	// Vida máxima del personaje, se inicializa con el valor de vida del Pj
    float max_vida{};
    cntrl Controles{};

	// Posición y dirección del personaje
    Vec2 pos{};
    Vec2 l_dir{ 1.0f, 0.0f }; // Vector de dirección inicializado a derecha

	// Texturas para animación y ataque
    Texture2D Ataque;
    Texture2D Frames[3]; // Array de 3 texturas
	Texture2D Frames_shadow[3]; // Array de 3 texturas para sombras

    // Variables para la sombra del personaje
    Mesh shadowMesh;
    Model shadow;

	// Sonido de ataque
    Sound efecto_ataque;

	// Variables para animación
    int   frameActual = 0;
    float frameTimer = 0.0f;
    bool  moviendose{ false };
    bool  isPlayer{ true };

	// Tamaño del personaje para colisiones y dibujo 3D
    const float charSize = 4.0;

public:
    //Constructor
    Personaje(Pj p, cntrl c, Vec2 po, bool ip)
    {
        Player = p;
        max_vida = p.vida;
        Controles = c;
        Frames[0] = LoadTexture(Player.Sprite_1);
        Frames[1] = LoadTexture(Player.Sprite_2);
        Frames[2] = LoadTexture(Player.Sprite_3);
        Frames_shadow[0] = LoadTexture(Player.Sprite_1_shadow);
        Frames_shadow[1] = LoadTexture(Player.Sprite_2_shadow);
        Frames_shadow[2] = LoadTexture(Player.Sprite_3_shadow);
        efecto_ataque = LoadSound(Player.Efecto_ataque);
        Ataque = LoadTexture(Player.Ataque);
        pos = po; // Posición inicial del personaje
        isPlayer = ip; // Indica si el personaje es controlado por el jugador o la CPU
    }

    //Actualización del arquero
    void Update(float dt);

    //Dibuja el Sprite del arquero en pantalla
    void Draw();

    //Funciones para obtener las características del personaje (getters y setters)
    float GetFuerza()const { return Player.fuerza; }
    Vec2 GetPos()const { return pos; }
    void SetPos(Vec2 p) { pos = p; }
    float GetVida()const { return Player.vida; }
    void set_isPlayer(bool ip) { isPlayer = ip; }
    bool get_isPlayer()const { return isPlayer; }
    float get_Cooldown()const { return Player.cooldown; }
    float GetVelocidad()const { return Player.vel; }
    Vec2 GetDir() { return l_dir; }
    void SetDir(Vec2 d) { l_dir = d.unitario(); } // Normaliza el vector de dirección
   
    //Hitbox del personaje para colisiones
    float GetAncho() const { return (float)Frames[frameActual].width; }
    float GetAlto()  const { return (float)Frames[frameActual].height; }

    //Daño al personaje, restando a su vida el valor del daño recibido
    void pain(float damage);

    // Crea un nuevo disparo con la posición y la textura de la flecha
    Disparo Shoot();


    //Sonidos
    void PlayAttackSound() { 
        SetSoundVolume(efecto_ataque, 3.5f);
        PlaySound(efecto_ataque); }

 
    //Destructor
    ~Personaje() {
        UnloadSound(efecto_ataque);
        UnloadTexture(Frames[0]);
        UnloadTexture(Frames[1]);
        UnloadTexture(Frames[2]);
        UnloadTexture(Ataque);
    }
};

