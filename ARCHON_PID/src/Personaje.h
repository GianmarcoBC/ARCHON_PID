#pragma once
#include "raylib.h"
#include "Disparo.h"
#include "Pj.h"

class Personaje
{
	Texture2D Sprite;
    Texture2D Ataque;
    Vector2 pos{};
    Vector2 l_dir{ 1.0f, 0.0f }; // Vector de dirección inicializado a derecha
    float speed;
    float vida;
public:
    //Constructor
    Personaje(Pj p) 
    {
        Sprite = LoadTexture(p.Sprite);
        Ataque = LoadTexture(p.Ataque);
        speed = p.vel;
        vida = p.vida;
    }

    //Actualización del arquero
    void Update(float dt);

    //Dibuja el Sprite del arquero en pantalla
    void Draw();
    
    // Crea un nuevo disparo con la posición y la textura de la flecha
    Disparo Shoot();

    //Destructor
    ~Personaje() {
        UnloadTexture(Sprite); 
        UnloadTexture(Ataque);
    }
};

