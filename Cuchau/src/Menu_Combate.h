#pragma once
#include "raylib.h"
#include "Pj_info.h"
#include <cstdlib>
#include <ctime>

// ============================================================================
//  Menu_Combate.h — Menu de seleccion de personajes y modo de juego
//
//  Flujo del menu:
//    1. ELIGIENDO_MODO — Elegir entre "2 Jugadores" o "VS IA"
//    2. DOS_JUGADORES  — Cada jugador elige su personaje con controles separados
//       VS_IA          — P1 elige personaje + dificultad, P2 es aleatorio
//    3. Cuando ambos confirman, Update() devuelve true para iniciar el combate
//
//  Controles:
//    Modo: W/S o Flechas para navegar, ENTER para confirmar
//    2P:   P1 usa A/D + Espacio, P2 usa Flechas + Ctrl derecho
//    IA:   A/D para personaje, Flechas para dificultad, Espacio para confirmar
//    ESC:  Volver a seleccion de modo
// ============================================================================

class Menu_Combate
{
    // --- Texturas del menu ---
    Texture2D Fondo_Menu = LoadTexture("bin/Resources/AAGraficos/the-dark-background-minimalism-tmnt-teenage-mutant-ninja-turtles-wallpaper-preview.png");

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
    enum Modo { ELIGIENDO_MODO, DOS_JUGADORES, VS_IA };
    Modo modo = ELIGIENDO_MODO;    // Pantalla actual del menu
    int  opcionModo = 0;           // 0 = 2 Jugadores, 1 = VS IA

    // Seleccion de personaje (indice 0..7 dentro de su equipo)
    int  selP1 = 0;                // Seleccion actual del jugador 1 (equipo claro)
    int  selP2 = 0;                // Seleccion actual del jugador 2 (equipo oscuro)
    bool P1Listo = false;          // P1 confirmo su seleccion
    bool P2Listo = false;          // P2 confirmo su seleccion

    // Dificultad de la IA (solo en modo VS_IA): 0=Facil, 1=Normal, 2=Dificil
    int  dificultad = 1;

    // Colores para cada jugador
    Color colores[2] = { SKYBLUE, RED };

    // Textos y colores de dificultad
    const char* nombresDif[3] = { "FACIL", "NORMAL", "DIFICIL" };
    Color       coloresDif[3] = { GREEN,   YELLOW,    RED };

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

    // Dibuja el selector de dificultad (3 botones: FACIL, NORMAL, DIFICIL)
    void DrawDificultad(int cx, int cy);

    // Dibuja la pantalla de seleccion de modo (2 Jugadores / VS IA)
    void DrawElegirModo();

    // --- Metodos privados de logica ---

    // Procesa input en pantalla de seleccion de modo. Devuelve true si se eligio un modo.
    bool UpdateModo();

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
    const Pj_info& GetSelP1()      const { return *personajes_claro[selP1]; }
    const Pj_info& GetSelP2()      const { return *personajes_oscuro[selP2]; }
    bool           EsModoIA()      const { return modo == VS_IA; }
    int            GetDificultad() const { return dificultad; }

    // Reinicia el menu al estado inicial (se llama al volver del combate)
    void Reset() {
        modo = ELIGIENDO_MODO;
        selP1 = selP2 = 0;
        P1Listo = P2Listo = false;
        dificultad = 1;
    }

    // Destructor: libera todas las texturas de los personajes y el fondo
    ~Menu_Combate() {
        UnloadTexture(Fondo_Menu);
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
