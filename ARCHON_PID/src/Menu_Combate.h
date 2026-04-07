#pragma once
#include "raylib.h"
#include "Pj.h"

class Menu_Combate
{
    // Miniaturas de cada personaje para mostrar en el menú
    Texture2D ImgMH = LoadTexture("bin/Resources/Graficos/Sprites/SpriteMH_0001.png");
    Texture2D ImgDragon = LoadTexture("bin/Resources/Graficos/Sprites/SpriteDragon.png");
    // ... un texture por personaje

    int selP1 = 0; // índice del personaje elegido por P1
    int selP2 = 0; // índice del personaje elegido por P2
    bool P1Listo = false;
    bool P2Listo = false;

    // Lista de todos los personajes disponibles
    const Pj* personajes[2] = { &MH, &Dragon }; // añade los que tengas
    const char* nombres[2] = { "MH", "Dragon" };
    Texture2D* imagenes[2] = { &ImgMH, &ImgDragon };

public:
    Menu_Combate() = default;

    // Devuelve true cuando los dos jugadores han confirmado
    bool Update();
    void Draw();

    // Para obtener las elecciones al salir del menú
    const Pj& GetSelP1() const { return *personajes[selP1]; }
    const Pj& GetSelP2() const { return *personajes[selP2]; }

    ~Menu_Combate() {
        UnloadTexture(ImgMH);
        UnloadTexture(ImgDragon);
    }
};

