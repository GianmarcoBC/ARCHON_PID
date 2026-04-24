#include "Tablero.h"



Tablero::Tablero(){
    casillasxlado = 9;
    tamanoCasilla = 64;

}

void Tablero::inicializarTablero() {
    // Primero, nos aseguramos de que toda la matriz esté vacía
    for (int i = 0; i < 9; i++) {
        for (int j = 0; j < 9; j++) {
            cuadricula[i][j] = nullptr;
        }
    }

    personaje_seleccionado = nullptr;
    fila_seleccionada = -1; //Inicializamos a un valor imposible de introducir como índice de la matriz
    columna_seleccionada = -1; // Para que no esté ninguna casilla seleccionada en el momento incial

    // Colocamos todas las piezas en su posición inicial llamando al constructor
  
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

    //Bucle para dibujar el tablero
    for (int fila = 0; fila < casillasxlado; fila++) {
        for (int columna = 0; columna < casillasxlado; columna++) {

            Color colorCasilla; 
            if ((fila + columna) % 2 == 0) {
                colorCasilla = LIGHTGRAY; 
            }
            else {
                colorCasilla = DARKGRAY;  
            }

            int posX =  (970/2-4.5*tamanoCasilla)+columna * tamanoCasilla; //970 es el largo de la pantalla
            int posY = (580 / 2 - 4.5 * tamanoCasilla)+fila * tamanoCasilla; //580 es la altura de la pantalla

            DrawRectangle(posX, posY, tamanoCasilla, tamanoCasilla, colorCasilla);

          
            
        }
    }

    //Bucle para dibujar los personajes
    for (int fila = 0; fila < casillasxlado; fila++) {
        for (int columna = 0; columna < casillasxlado; columna++) {

            if (cuadricula[fila][columna] != nullptr) {

                cuadricula[fila][columna]->DrawT();
            }


        }
    }

    
}

#include <iostream>

void Tablero::seleccionaCasilla() {

    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        
        int mouseX = GetMouseX();
        int mouseY = GetMouseY();
        
        //Bucle para recorrer el tablero
        for (int fila = 0; fila < casillasxlado; fila++) {
            for (int columna = 0; columna < casillasxlado; columna++) {

                int posX = (970 / 2 - 4.5 * tamanoCasilla) + columna * tamanoCasilla; //970 es el largo de la pantalla
                int posY = (580 / 2 - 4.5 * tamanoCasilla) + fila * tamanoCasilla; //580 es la altura de la pantalla
               
                if (mouseX <= (posX + tamanoCasilla) && mouseX > posX) {

                    columna_seleccionada = columna;

                }
                if (mouseY <= (posY + tamanoCasilla) && mouseY > posY) {

                    fila_seleccionada = fila;

                }

            }
        }
     

    }

    std::cout << fila_seleccionada << " " << columna_seleccionada << std::endl;

 

}



void Tablero::cambioPosicionPieza(Personaje* personaje, int fil, int col) {

    cuadricula[personaje->get_fila()][personaje->get_columna()] = nullptr;
    personaje->set_fila_columna(fil, col);
    cuadricula[fil][col] = personaje;

    //Esta función asegura que la posición de los personajes siempre sea igual a la información que tiene el tablero
    //sobre la posición de cada personaje

}

void Tablero::reset_seleccion() {
    fila_seleccionada = -1;
    columna_seleccionada = -1;

}

void Tablero::moverPieza() {
        
    seleccionaCasilla();

    static Personaje* auxPersonaje=nullptr;
   

    if(cuadricula[fila_seleccionada][columna_seleccionada]!=nullptr && fila_seleccionada!=-1 && columna_seleccionada!=-1){
    
        if (turno == cuadricula[fila_seleccionada][columna_seleccionada]->get_equipo()) {

            auxPersonaje = cuadricula[fila_seleccionada][columna_seleccionada];
            reset_seleccion();

        }
    }

    if ( fila_seleccionada != -1 && columna_seleccionada != -1 && auxPersonaje!=nullptr) {

        if (cuadricula[fila_seleccionada][columna_seleccionada] != nullptr) {

            cuadricula[fila_seleccionada][columna_seleccionada]->~Personaje();

        }

        cambioPosicionPieza(auxPersonaje, fila_seleccionada, columna_seleccionada);
        auxPersonaje = nullptr;
        reset_seleccion();
        turno = turno == LUZ ? OSCURIDAD : LUZ;


    }

}