#pragma once
#include "raylib.h"
#include "Disparo.h"

class Archer
{
	Texture2D Sprite = LoadTexture("bin/Resources/Graficos/Sprites/SpriteArcherpxo.png");
    Texture2D Flecha = LoadTexture("bin/Resources/Graficos/Sprites/SpriteFlecha.png");
    Vector2 pos{};
    Vector2 l_dir{ 1.0f, 0.0f }; // Vector de dirección inicializado a derecha
    float speed = 500.0f;
public:
    //Constructor
    Archer(Vector2 p) {pos = p;}

    //Actualización del arquero
    void Update(float dt);

    //Dibuja el Sprite del arquero en pantalla
    void Draw();
    
    // Crea un nuevo disparo con la posición y la textura de la flecha
    Disparo Shoot();

    //Destructor
    ~Archer() { 
        UnloadTexture(Sprite); 
        UnloadTexture(Flecha);
    }
};

