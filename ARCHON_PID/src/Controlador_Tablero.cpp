#include "Controlador_Tablero.h"



Controlador_Tablero::Controlador_Tablero(bool vsAI, int dificultad) {





    if (vsAI) {

        switch (dificultad) {
        case 0:  ia_Tablero = new AI_Facil_Tablero(); break;
        case 1:  ia_Tablero = new AI_Defensiva_Tablero(); break;
        case 2:  ia_Tablero = new AI_Agresiva_Tablero(); break;
        default: ia_Tablero = new AI_Facil_Tablero(); break;
        }

    }
    
}