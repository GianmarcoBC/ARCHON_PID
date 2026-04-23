#pragma once
#include "raylib.h"
#include "Personaje.h"
#include "Pj.h"


class Tablero
{

    int casillasxlado;
    int tamanoCasilla;
    Personaje* cuadricula[9][9]{nullptr};

public:


    Tablero();

    void Draw();
    void inicializarTablero();




};

