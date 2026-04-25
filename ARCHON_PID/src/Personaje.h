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
    int fila, columna; // Posicion del personaje en la matriz
    bool equipo;
    tipo_pj ID;
    unsigned int rangoTablero;
    bool vuela;

public:
    //Constructor
    Personaje(Pj p, int ifila, int icolumna) 
    {
        Sprite = LoadTexture(p.Sprite);
        Ataque = LoadTexture(p.Ataque);
        speed = p.vel;
        vida = p.vida;
        fila = ifila;
        columna = icolumna;
        equipo = p.equipo;
        ID = p.ID;
        rangoTablero = p.rangoTablero;
        vuela = p.vuela;
    }

    void set_fila_columna(int fil, int col);
    int get_fila() { return fila; };
    int get_columna() { return columna; };
    bool get_equipo() { return equipo; };
    tipo_pj get_ID() { return ID; };
    unsigned int get_rangoTablero() { return rangoTablero; };
    bool get_vuela() { return vuela; };
    


    //Actualización del arquero
    void Update(float dt);

    //Dibuja el Sprite del arquero en pantalla
    void Draw();
    void DrawT();  //Funcion de dibujar en el tablero
    
    // Crea un nuevo disparo con la posición y la textura de la flecha
    Disparo Shoot();

    //Destructor
    ~Personaje() {
        UnloadTexture(Sprite); 
        UnloadTexture(Ataque);
    }
};

