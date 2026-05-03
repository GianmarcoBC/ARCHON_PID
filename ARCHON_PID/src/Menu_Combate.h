#pragma once
#include "raylib.h"
#include "Pj.h"
#include <cstdlib>
#include <ctime>

class Menu_Combate
{
    // ── Texturas ──────────────────────────────────────────────────────────────
    Texture2D Fondo_Menu = LoadTexture("bin/Resources/Graficos/Sprites/the-dark-background-minimalism-tmnt-teenage-mutant-ninja-turtles-wallpaper-preview.png");

    Texture2D ImgMH = LoadTexture(MH.Sprite_1);
    Texture2D ImgPhoenix = LoadTexture(Phoenix.Sprite_1);
    Texture2D ImgGolem = LoadTexture(Golem.Sprite_1);
    Texture2D ImgDjinni = LoadTexture(Djinni.Sprite_1);
    Texture2D ImgUnicorn = LoadTexture(Unicorn.Sprite_1);
    Texture2D ImgValkyrie = LoadTexture(Valkyrie.Sprite_1);
    Texture2D ImgArcher = LoadTexture(Archer.Sprite_1);
    Texture2D ImgKnight = LoadTexture(Knight.Sprite_1);

    Texture2D ImgPlatero = LoadTexture(Platero.Sprite_1);
    Texture2D ImgShapeShifter = LoadTexture(ShapeShifter.Sprite_1);
    Texture2D ImgTroll = LoadTexture(Troll.Sprite_1);
    Texture2D ImgDragon = LoadTexture(Dragon.Sprite_1);
    Texture2D ImgBasilisk = LoadTexture(Basilisk.Sprite_1);
    Texture2D ImgBanshee = LoadTexture(Banshee.Sprite_1);
    Texture2D ImgManticore = LoadTexture(Manticore.Sprite_1);
    Texture2D ImgGoblin = LoadTexture(Goblin.Sprite_1);

    static const int NUM_PJS = 8;

    // ── Modo de juego ─────────────────────────────────────────────────────────
    enum Modo { ELIGIENDO_MODO, DOS_JUGADORES, VS_IA };
    Modo modo = ELIGIENDO_MODO;
    int  opcionModo = 0;   // 0 = 2J, 1 = IA  (cursor en pantalla de modo)

    // ── Selección personajes ──────────────────────────────────────────────────
    int  selP1 = 0;
    int  selP2 = 0;     // en VS_IA lo elige la IA al azar
    bool P1Listo = false;
    bool P2Listo = false; // en VS_IA se pone true automáticamente

    // ── Dificultad (solo VS_IA) ───────────────────────────────────────────────
    int  dificultad = 1;   // 0=Fácil  1=Normal  2=Difícil
    bool selDificultad = false; // false = navegando pj, true = eligiendo dif

    Color colores[2] = { SKYBLUE, RED };

    const char* nombresDif[3] = { "FACIL", "NORMAL", "DIFICIL" };
    Color       coloresDif[3] = { GREEN,   YELLOW,    RED };

    // ── Listas de personajes ──────────────────────────────────────────────────
    const Pj* personajes_claro[NUM_PJS] = { &MH,      &Phoenix, &Golem,    &Djinni,
                                              &Unicorn, &Valkyrie,&Archer,   &Knight };
    Texture2D* imagenes_claro[NUM_PJS] = { &ImgMH,   &ImgPhoenix,&ImgGolem,&ImgDjinni,
                                              &ImgUnicorn,&ImgValkyrie,&ImgArcher,&ImgKnight };

    const Pj* personajes_oscuro[NUM_PJS] = { &Platero, &ShapeShifter,&Troll,  &Dragon,
                                              &Basilisk,&Banshee,     &Manticore,&Goblin };
    Texture2D* imagenes_oscuro[NUM_PJS] = { &ImgPlatero,&ImgShapeShifter,&ImgTroll,&ImgDragon,
                                              &ImgBasilisk,&ImgBanshee,&ImgManticore,&ImgGoblin };

    // ── Métodos privados ──────────────────────────────────────────────────────
    void DrawSeleccion(int player, int cx, int cy, int sel, bool listo);
    void DrawPanelIA(int cx, int cy);          // panel derecho modo IA
    void DrawDificultad(int cx, int cy);
    void DrawElegirModo();

    bool UpdateModo();   // devuelve true si se eligió modo
    bool UpdateDosJugadores();
    bool UpdateVsIA();

public:
    Menu_Combate() { srand(static_cast<unsigned int>(time(NULL))); }

    bool Update();   // devuelve true = empieza el combate
    void Draw();

    // ── Getters ───────────────────────────────────────────────────────────────
    const Pj& GetSelP1()       const { return *personajes_claro[selP1]; }
    const Pj& GetSelP2()       const { return *personajes_oscuro[selP2]; }
    bool      EsModoIA()       const { return modo == VS_IA; }
    int       GetDificultad()  const { return dificultad; }

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