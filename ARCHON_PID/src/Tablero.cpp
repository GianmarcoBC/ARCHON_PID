#include "Tablero.h"



Tablero::Tablero(){
    casillasxlado = 9;
    tamanoCasilla = 64;

}

void Tablero::inicializarTablero() {
    // Primero, nos aseguramos de que toda la matriz esté vacía (punteros nulos)
    for (int i = 0; i < 9; i++) {
        for (int j = 0; j < 9; j++) {
            cuadricula[i][j] = nullptr;
        }
    }

    // Ahora llamamos a los constructores para colocar las piezas iniciales
   

    //ASI CON TODOS 

    /*cuadricula[0][0] = new Personaje(MH,0,0);
    cuadricula[1][0] = new Personaje(Phoenix,1,0);
    cuadricula[2][0] = new Personaje(Golem, 2, 0);
    cuadricula[3][0] = new Personaje(Djinni, 3, 0);

    cuadricula[4][0] = new Personaje(Unicorn, 4, 0);
    cuadricula[5][0] = new Personaje(Valkyrie, 5, 0);
    cuadricula[6][0] = new Personaje(Archer, 6, 0);
    cuadricula[7][0] = new Personaje(Knight, 7, 0);

    cuadricula[8][0] = new Personaje(Platero, 8, 0);
    cuadricula[0][1] = new Personaje(ShapeShifter, 0, 1);
    cuadricula[1][1] = new Personaje(Troll, 1, 1);
    cuadricula[2][1] = new Personaje(Dragon, 2, 1);

    cuadricula[3][1] = new Personaje(Basilisk, 3, 1);
    cuadricula[4][1] = new Personaje(Banshee, 4, 1);
    cuadricula[5][1] = new Personaje(Manticore, 5, 1);
    cuadricula[6][2] = new Personaje(Goblin, 6, 1);
 */

    //LUZ
    cuadricula[0][0] = new Personaje(Valkyrie, 0, 0);
    cuadricula[8][0] = new Personaje(Valkyrie, 8, 0);

    cuadricula[1][0] = new Personaje(Golem, 1, 0);
    cuadricula[7][0] = new Personaje(Golem, 7, 0);

    cuadricula[2][0] = new Personaje(Unicorn, 2, 0);
    cuadricula[6][0] = new Personaje(Unicorn, 6, 0);

    cuadricula[3][0] = new Personaje(Djinni, 3, 0);
    cuadricula[4][0] = new Personaje(MH, 4, 0); 
    cuadricula[5][0] = new Personaje(Phoenix, 5, 0);

    cuadricula[0][1] = new Personaje(Archer, 0, 1);
    cuadricula[8][1] = new Personaje(Archer, 8, 1);

    for (int fila = 1; fila < casillasxlado-1; fila++) {
        cuadricula[fila][1] = new Personaje(Knight, fila, 1);
    }

    //OSCURIDAD
    cuadricula[0][8] = new Personaje(Banshee, 0, 8);
    cuadricula[8][8] = new Personaje(Banshee, 8, 8);

    cuadricula[1][8] = new Personaje(Troll, 1, 8);
    cuadricula[7][8] = new Personaje(Troll, 7, 8);

    cuadricula[2][8] = new Personaje(Basilisk, 2, 8);
    cuadricula[6][8] = new Personaje(Basilisk, 6, 8);

    cuadricula[3][8] = new Personaje(ShapeShifter, 3, 8);
    cuadricula[4][8] = new Personaje(Platero, 4, 8);
    cuadricula[5][8] = new Personaje(Dragon, 5, 8);

    cuadricula[0][7] = new Personaje(Manticore, 0, 7);
    cuadricula[8][7] = new Personaje(Manticore, 8, 7);

    for (int fila = 1; fila < casillasxlado - 1; fila++) {
        cuadricula[fila][7] = new Personaje(Goblin, fila, 7);
    }


    
    
}



void Tablero::Draw() {
    for (int fila = 0; fila < casillasxlado; fila++) {
        for (int columna = 0; columna < casillasxlado; columna++) {

            Color colorCasilla;
            if ((fila + columna) % 2 == 0) {
                colorCasilla = LIGHTGRAY; // Casilla de Luz
            }
            else {
                colorCasilla = DARKGRAY;  // Casilla de Oscuridad
            }

            int posX =  (970/2-4.5*tamanoCasilla)+columna * tamanoCasilla; //970 es el largo de la pantalla
            int posY = (580 / 2 - 4.5 * tamanoCasilla)+fila * tamanoCasilla; //580 es la altura de la pantalla

            DrawRectangle(posX, posY, tamanoCasilla, tamanoCasilla, colorCasilla);

          
            
        }
    }

    for (int fila = 0; fila < casillasxlado; fila++) {
        for (int columna = 0; columna < casillasxlado; columna++) {

            if (cuadricula[fila][columna] != nullptr) {

                cuadricula[fila][columna]->DrawT();
            }


        }
    }

    
}