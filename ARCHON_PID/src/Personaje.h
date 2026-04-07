#pragma once
#include "raylib.h"
#include "Disparo.h"
#include "Pj.h"

class Personaje
{
    Pj Player{};
    float max_vida{};
    cntrl Controles{};
    Texture2D Sprite;
    Texture2D Ataque;
    Vector2 pos{};
    Vector2 l_dir{ 1.0f, 0.0f }; // Vector de dirección inicializado a derecha
public:
    //Constructor
    Personaje(Pj p, cntrl c, Vector2 po) 
    {
        Player = p;
        max_vida = p.vida;
        Controles = c;
        Sprite = LoadTexture(Player.Sprite);
        Ataque = LoadTexture(Player.Ataque);
        pos = po; // Posición inicial del personaje
    }

    //Actualización del arquero
    void Update(float dt);

    //Dibuja el Sprite del arquero en pantalla
    void Draw();

    //Funciones para obtener las características del personaje
    float GetFuerza()const { return Player.fuerza; }
    Vector2 GetPos()const { return pos; }
    float GetVida()const { return Player.vida; }
   

    //Daño al personaje, restando a su vida el valor del daño recibido
    void pain(float damage);

    // Crea un nuevo disparo con la posición y la textura de la flecha
    Disparo Shoot();

 
    //Destructor
    ~Personaje() {
        UnloadTexture(Sprite); 
        UnloadTexture(Ataque);
    }
};

