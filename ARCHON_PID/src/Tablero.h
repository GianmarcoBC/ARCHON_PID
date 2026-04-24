#pragma once
#include "raylib.h"
#include "Personaje.h"
#include "Pj.h"


class Tablero
{

    int casillasxlado;
    int tamanoCasilla;
    Personaje* cuadricula[9][9];
    Personaje* personaje_seleccionado;
    int fila_seleccionada;
    int columna_seleccionada;
    bool turno{LUZ};


public:


    Tablero();

    void Draw();
    void inicializarTablero();
    void seleccionaCasilla();
    void cambioPosicionPieza(Personaje* personaje, int fila, int columna); 
    void reset_seleccion(); //Resetea los valores de sila_seleccionada y columna_seleccionada a -1
    void moverPieza();



};

