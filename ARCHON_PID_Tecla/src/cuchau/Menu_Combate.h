#pragma once
#include "raylib.h"
#include "Pj_info.h"
#include <cstdlib>
#include <ctime>

// ============================================================================
//  Menu_Combate.h — Menu de seleccion de personajes y modo de juego
//
//  Flujo del menu:
//    1. DOS_JUGADORES  — Cada jugador elige su personaje con controles separados
//       VS_IA          — P1 elige personaje, P2 es aleatorio del equipo contrario
//    2. Cuando ambos confirman, Update() devuelve true para iniciar el combate
//    El modo (PvP/IA) y bandos se establecen externamente via SetModo().
//
//  Controles:
//    2P:   P1 usa A/D + Espacio, P2 usa Flechas + Ctrl derecho
//    IA:   A/D para personaje, Espacio para confirmar
//    ESC:  Volver a seleccion de modo
// ============================================================================

class Menu_Combate
{
    // Texturas de los 8 personajes del equipo claro (se usan como miniaturas e iconos)
    Texture2D ImgMH          = LoadTexture(MH.Sprites[0].data());
    Texture2D ImgPhoenix     = LoadTexture(Phoenix.Sprites[0].data());
    Texture2D ImgGolem       = LoadTexture(Golem.Sprites[0].data());
    Texture2D ImgDjinni      = LoadTexture(Djinni.Sprites[0].data());
    Texture2D ImgUnicorn     = LoadTexture(Unicorn.Sprites[0].data());
    Texture2D ImgValkyrie    = LoadTexture(Valkyrie.Sprites[0].data());
    Texture2D ImgArcher      = LoadTexture(Archer.Sprites[0].data());
    Texture2D ImgKnight      = LoadTexture(Knight.Sprites[0].data());

    // Texturas de los 8 personajes del equipo oscuro
    Texture2D ImgPlatero     = LoadTexture(Platero.Sprites[0].data());
    Texture2D ImgShapeShifter= LoadTexture(ShapeShifter.Sprites[0].data());
    Texture2D ImgTroll       = LoadTexture(Troll.Sprites[0].data());
    Texture2D ImgDragon      = LoadTexture(Dragon.Sprites[0].data());
    Texture2D ImgBasilisk    = LoadTexture(Basilisk.Sprites[0].data());
    Texture2D ImgBanshee     = LoadTexture(Banshee.Sprites[0].data());
    Texture2D ImgManticore   = LoadTexture(Manticore.Sprites[0].data());
    Texture2D ImgGoblin      = LoadTexture(Goblin.Sprites[0].data());

    static const int NUM_PJS = 8;  // Personajes por equipo

    // --- Estado del menu ---
    enum Modo { DOS_JUGADORES, VS_IA };
    Modo modo = DOS_JUGADORES;     // Pantalla actual del menu
    // Seleccion de personaje (indice 0..7 dentro de su equipo)
    int  selP1 = 0;                // Seleccion actual del jugador 1
    int  selP2 = 0;                // Seleccion actual del jugador 2
    bool P1Listo = false;          // P1 confirmo su seleccion
    bool P2Listo = false;          // P2 confirmo su seleccion
    bool p1WasReady = false;       // P1 was ready before this frame's HandleMouse
    bool p2WasReady = false;       // P2 was ready before this frame's HandleMouse

    // Dificultad de la IA (0=Facil, 1=Normal, 2=Dificil, 3=Maestro)
    int  dificultad = 1;
    int bandoP1 = 0;  // 0 = claro, 1 = oscuro
    int bandoP2 = 1;  // 0 = claro, 1 = oscuro

    // Flag: ESC pressed from character selection
    bool quiereVolver = false;

    // Colores para cada jugador
    Color colores[2] = { SKYBLUE, RED };

    // Textos y colores de dificultad (3 niveles)
    const char* nombresDif[3] = { "PLATERO", "MH", "SANSEGUNDO" };
    Color       coloresDif[3] = { GREEN, YELLOW, RED };

    // Arrays de punteros a los datos y texturas de cada equipo
    const Pj_info* personajes_claro[NUM_PJS]  = { &MH, &Phoenix, &Golem, &Djinni,
                                                    &Unicorn, &Valkyrie, &Archer, &Knight };
    Texture2D*     imagenes_claro[NUM_PJS]    = { &ImgMH, &ImgPhoenix, &ImgGolem, &ImgDjinni,
                                                    &ImgUnicorn, &ImgValkyrie, &ImgArcher, &ImgKnight };

    const Pj_info* personajes_oscuro[NUM_PJS] = { &Platero, &ShapeShifter, &Troll, &Dragon,
                                                    &Basilisk, &Banshee, &Manticore, &Goblin };
    Texture2D*     imagenes_oscuro[NUM_PJS]   = { &ImgPlatero, &ImgShapeShifter, &ImgTroll, &ImgDragon,
                                                    &ImgBasilisk, &ImgBanshee, &ImgManticore, &ImgGoblin };

    // --- Metodos privados de dibujo ---

    // Dibuja el panel de seleccion de un jugador (marco, sprite grande, miniaturas, nombre)
    void DrawSeleccion(int player, int cx, int cy, int sel, bool listo);

    // Dibuja el panel del enemigo IA (??? hasta que P1 confirme, luego muestra el personaje aleatorio)
    void DrawPanelIA(int cx, int cy);

    // --- Metodos privados de logica ---

    // Procesa input en modo 2 jugadores. Devuelve true si ambos estan listos y se pulsa ENTER.
    bool UpdateDosJugadores();

    // Procesa input en modo VS IA. Devuelve true si P1 esta listo y se pulsa ENTER.
    bool UpdateVsIA();

public:
    // Constructor: inicializa la semilla aleatoria (para seleccion de personaje IA)
    Menu_Combate() { srand(static_cast<unsigned int>(time(NULL))); }

    // Actualiza el menu segun el estado actual. Devuelve true cuando se debe iniciar el combate.
    bool Update();
    void Draw();

    // Getters para obtener los personajes seleccionados
    const Pj_info& GetSelP1()      const { return bandoP1 == 0 ? *personajes_claro[selP1] : *personajes_oscuro[selP1]; }
    const Pj_info& GetSelP2()      const { return bandoP2 == 0 ? *personajes_claro[selP2] : *personajes_oscuro[selP2]; }
    bool           EsModoIA()      const { return modo == VS_IA; }
    int            GetDificultad() const { return dificultad; }
    int            GetBandoP1()    const { return bandoP1; }


    // Reinicia el menu al estado inicial (se llama al volver del combate)
    void Reset();

    // Set mode directly, skipping the mode selection screen
    void SetModo(bool ia, int bando1 = 0, int bando2 = 1, int difi = 1);

    // True when ESC was pressed from character selection — caller should go back
    bool QuiereVolver() const { return quiereVolver; }

    void HandleMouse();

    // Destructor: libera todas las texturas de los personajes
    ~Menu_Combate() {
        UnloadTexture(ImgMH);        UnloadTexture(ImgPhoenix);
        UnloadTexture(ImgGolem);     UnloadTexture(ImgDjinni);
        UnloadTexture(ImgUnicorn);   UnloadTexture(ImgValkyrie);
        UnloadTexture(ImgArcher);    UnloadTexture(ImgKnight);
        UnloadTexture(ImgPlatero);   UnloadTexture(ImgShapeShifter);
        UnloadTexture(ImgTroll);     UnloadTexture(ImgDragon);
        UnloadTexture(ImgBasilisk);  UnloadTexture(ImgBanshee);
        UnloadTexture(ImgManticore); UnloadTexture(ImgGoblin);
    }
};
