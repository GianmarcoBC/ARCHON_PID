#pragma once
#include "raylib.h"
#include "TiposPj.h"
#include "Pj.h"

/*
 * PiezaTablero - Representa una pieza/personaje en el tablero 9x9
 *
 * Contiene los datos necesarios para el tablero estrategico:
 * posicion (fila/columna), equipo, tipo, rango de movimiento, etc.
 *
 * El sprite se carga desde los archivos de ARCHON_PID_TECLA (640x640)
 * y se escala a 64x64 al dibujarse en el tablero.
 *
 * Cuando dos piezas se enfrentan, se usa PjMapping.h para obtener
 * los datos de combate 3D (Pj_info) a partir del tipo_pj.
 */
class PiezaTablero
{
    Texture2D Sprite;           // Textura del personaje para el tablero
    Vector2 pos{};              // Posicion en pixeles (calculada al dibujar)
    float vida;                 // Vida actual (se restaura al morir para el cementerio)
    float vidaMax;              // Vida maxima
    int fila, columna;          // Posicion en la cuadricula 9x9 (fila=Y, columna=X)
    bool team;                  // LUZ (false/0) o OSCURIDAD (true/1)
    tipo_pj ID;                 // Tipo de personaje (para identificar en combate y hechizos)
    unsigned int rangoTablero;  // Alcance de movimiento en casillas
    bool vuela;                 // Si es true, se mueve en area; si false, solo en cruz
    bool imprison{ false };     // Aprisionado por hechizo Imprison (no puede moverse)

public:
    // Constructor: carga el sprite y establece la posicion inicial en el tablero
    PiezaTablero(PjBoard p, int ifila, int icolumna)
    {
        Sprite = LoadTexture(p.Sprite);
        vida = 100.0f;
        vidaMax = 100.0f;
        fila = ifila;
        columna = icolumna;
        team = p.team;
        ID = p.ID;
        rangoTablero = p.rangoTablero;
        vuela = p.vuela;
    }

    // --- Getters y setters ---
    void set_fila_columna(int fil, int col) { fila = fil; columna = col; }
    int get_fila() { return fila; }
    int get_columna() { return columna; }
    bool get_equipo() { return team; }
    tipo_pj get_ID() { return ID; }
    unsigned int get_rangoTablero() { return rangoTablero; }
    bool get_vuela() { return vuela; }
    void heal() { vida = vidaMax; }             // Restaurar vida (al revivir o ir al cementerio)
    void set_imprison(bool aux) { imprison = aux; }
    bool get_imprison() { return imprison; }

    // Dibuja la pieza en el tablero, escalada a 64x64
    // paramx/paramy: offset en pixeles de la esquina superior-izquierda del tablero
    void DrawT(const int paramx, const int paramy);

    ~PiezaTablero() {
        UnloadTexture(Sprite);
    }
};
