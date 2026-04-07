#pragma once
#include "raylib.h"
#include "Pj.h"

class Menu_Combate
{
    // Miniaturas de cada personaje para mostrar en el menú
    Texture2D Fondo_Menu = LoadTexture("bin/Resources/Graficos/Sprites/the-dark-background-minimalism-tmnt-teenage-mutant-ninja-turtles-wallpaper-preview.png");
    
    Texture2D ImgMH = LoadTexture(MH.Sprite);
    Texture2D ImgPhoenix = LoadTexture(Phoenix.Sprite);
    Texture2D ImgGolem = LoadTexture(Golem.Sprite);
    Texture2D ImgDjinni = LoadTexture(Djinni.Sprite);
    Texture2D ImgUnicorn = LoadTexture(Unicorn.Sprite);
    Texture2D ImgValkyrie = LoadTexture(Valkyrie.Sprite);
    Texture2D ImgArcher = LoadTexture(Archer.Sprite);
    Texture2D ImgKnight = LoadTexture(Knight.Sprite);

    Texture2D ImgPlatero = LoadTexture(Platero.Sprite);
    Texture2D ImgShapeShifter = LoadTexture(ShapeShifter.Sprite);
    Texture2D ImgTroll = LoadTexture(Troll.Sprite);
    Texture2D ImgDragon = LoadTexture(Dragon.Sprite);
    Texture2D ImgBasilisk = LoadTexture(Basilisk.Sprite);
    Texture2D ImgBanshee = LoadTexture(Banshee.Sprite);
    Texture2D ImgManticore = LoadTexture(Manticore.Sprite);
    Texture2D ImgGoblin = LoadTexture(Goblin.Sprite);

    static const int NUM_PJS = 8;

    int selP1 = 0; // índice del personaje elegido por P1
    int selP2 = 0; // índice del personaje elegido por P2
    bool P1Listo = false;
    bool P2Listo = false;
    Color colores[NUM_PJS] = {SKYBLUE, RED};

    

    // Lista de todos los personajes disponibles
    const Pj* personajes_claro[NUM_PJS] = { &MH, &Phoenix, &Golem, &Djinni, &Unicorn, &Valkyrie, &Archer, &Knight }; // añade los que tengas
    Texture2D* imagenes_claro[NUM_PJS] = { &ImgMH, &ImgPhoenix, &ImgGolem, &ImgDjinni, &ImgUnicorn, &ImgValkyrie, &ImgArcher, &ImgKnight };

    const Pj* personajes_oscuro[NUM_PJS] = { &Platero, &ShapeShifter, &Troll, &Dragon, &Basilisk, &Banshee, &Manticore, &Goblin }; // añade los que tengas
    Texture2D* imagenes_oscuro[NUM_PJS] = { &ImgPlatero, &ImgShapeShifter, &ImgTroll, &ImgDragon, &ImgBasilisk, &ImgBanshee, &ImgManticore, &ImgGoblin };

    void DrawSeleccion(int player, int cx, int cy, int sel, bool listo);

public:
    Menu_Combate() = default;

    // Devuelve true cuando los dos jugadores han confirmado
    bool Update();
    void Draw();

    // Para obtener las elecciones al salir del menú
    const Pj& GetSelP1() const { return *personajes_claro[selP1]; }
    const Pj& GetSelP2() const { return *personajes_oscuro[selP2]; }

    ~Menu_Combate() {
        UnloadTexture(Fondo_Menu);

        UnloadTexture(ImgMH);
        UnloadTexture(ImgPhoenix);
        UnloadTexture(ImgGolem);
        UnloadTexture(ImgDjinni);
        UnloadTexture(ImgUnicorn);
        UnloadTexture(ImgValkyrie);
        UnloadTexture(ImgArcher);
        UnloadTexture(ImgKnight);

        UnloadTexture(ImgPlatero);
        UnloadTexture(ImgShapeShifter);
        UnloadTexture(ImgTroll);
        UnloadTexture(ImgDragon);
        UnloadTexture(ImgBasilisk);
        UnloadTexture(ImgBanshee);
        UnloadTexture(ImgManticore);
        UnloadTexture(ImgGoblin);
    }
};

