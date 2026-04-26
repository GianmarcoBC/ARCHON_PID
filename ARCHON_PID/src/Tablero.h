#pragma once
#include "raylib.h"
#include "Personaje.h"
#include "Pj.h"

enum class ColorCasilla{BLANCO,NEGRO,CAMBIANTE};

class Tablero
{

    int casillasxlado;
    int tamanoCasilla;
    Personaje* cuadricula[9][9];
    Personaje* personaje_seleccionado{nullptr};
    int fila_seleccionada{-1};
    int columna_seleccionada{-1};
    bool turno{LUZ};
    bool movimientosPosibles[9][9]{ false };
    ColorCasilla colorCasilla[9][9];
    int Ciclo{};
    bool avance{false};
    bool hechizosOscuridad[7]{false};
    bool hechizosLuz[7]{ false };


public:


    Tablero();

    void Draw();
    void inicializarTablero();
    void seleccionaCasilla();
    void cambioPosicionPieza(Personaje* personaje, int fila, int columna); 
    void reset_seleccion(); //Resetea los valores de sila_seleccionada y columna_seleccionada a -1
    void moverPieza();
    void casillasPosibles(Personaje* p);
    void DrawCasillas( int fila, int columna);
    void set_MovimientosPosibles(bool set, int fila, int columna){ movimientosPosibles[fila][columna] = set; };
    bool get_MovimientosPosibles(int fila, int columna) { return movimientosPosibles[fila][columna]; };
    void set_colorCasilla(ColorCasilla color,int fil, int col) { colorCasilla[fil][col] = color; };
    void set_avance(bool aux) { avance = aux; };
    ColorCasilla get_colorCasilla(int fil, int col) { return colorCasilla[fil][col]; }
    void detectaGanador();
    void hechizos();
    void avanceCiclo();

    //Hechizos
    void Shift_Time(Personaje* personaje);
    void Teleport(Personaje* personaje);

};

