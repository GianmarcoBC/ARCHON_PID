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
            movimientosPosibles[i][j] = false;
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


    casillasPosibles(personaje_seleccionado);

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

    //std::cout << fila_seleccionada << " " << columna_seleccionada << std::endl;
   

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

    //static Personaje* auxPersonaje=nullptr;
   

    if(cuadricula[fila_seleccionada][columna_seleccionada]!=nullptr && fila_seleccionada!=-1 && columna_seleccionada!=-1){
    
        if (turno == cuadricula[fila_seleccionada][columna_seleccionada]->get_equipo()) {

            personaje_seleccionado = cuadricula[fila_seleccionada][columna_seleccionada];
            reset_seleccion();

        }
    }

    if ( fila_seleccionada != -1 && columna_seleccionada != -1 && personaje_seleccionado!=nullptr /* && get_MovimientosPosibles(fila_seleccionada, columna_seleccionada) == true*/) {

        if (cuadricula[fila_seleccionada][columna_seleccionada] != nullptr) {
            
            Personaje* personajeAtacado = cuadricula[fila_seleccionada][columna_seleccionada];
            int auxfila = personaje_seleccionado->get_fila(), auxcolumna = personaje_seleccionado->get_columna();

            personaje_seleccionado->set_fila_columna(fila_seleccionada, columna_seleccionada); //  se cambia eso pero no en la matriz
           
            //Iniciar combate (como parámetros dar los personajes implicados)
           
            Personaje* personajeGanador = personajeAtacado; //Aqui es el que gane, que se decide en la batalla
            
            if (personajeGanador == personaje_seleccionado) {//Si gana el que se ha movido:
            
                cuadricula[fila_seleccionada][columna_seleccionada]->~Personaje();
                cuadricula[fila_seleccionada][columna_seleccionada] = personaje_seleccionado;
                cuadricula[auxfila][auxcolumna] = nullptr;
             

            }
            
            else if (personajeGanador == personajeAtacado) {//Si gana el que estaba en la casilla:
                cuadricula[auxfila][auxcolumna]->~Personaje();
                cuadricula[auxfila][auxcolumna]=nullptr;
                
            }

        }
        else cambioPosicionPieza(personaje_seleccionado, fila_seleccionada, columna_seleccionada);
          personaje_seleccionado = nullptr;
       
        reset_seleccion();
        for (int i = 0; i < 9; i++) {
            for (int j = 0; j < 9; j++) {
                movimientosPosibles[i][j] = false;
            }
        }
        turno = turno == LUZ ? OSCURIDAD : LUZ;


    }

}



void Tablero::casillasPosibles(Personaje* p) {

    if(p != nullptr){
         
        if(personaje_seleccionado->get_vuela() == true) { //Si el personaje es Volador o Tp
            for (int fila = (p->get_fila() - static_cast<int>(p->get_rangoTablero())); fila <= (p->get_fila() + static_cast<int>(p->get_rangoTablero())); fila++) {
                for (int columna = (p->get_columna() - static_cast<int>(p->get_rangoTablero())); columna <= (p->get_columna() + static_cast<int>(p->get_rangoTablero())); columna++){
                   
                    if(cuadricula[fila][columna]==nullptr && fila>=0 && columna>=0 && fila <= 8 && columna <= 8)  DrawCasillas(fila, columna);

                    else if (cuadricula[fila][columna] != nullptr && fila >= 0 && columna >= 0 && fila<=8 && columna<=8) {
                       
                        if (cuadricula[fila][columna]->get_equipo() != p->get_equipo()) DrawCasillas(fila, columna);
    
                    }

                }
            }
        }
    
        if (personaje_seleccionado->get_vuela() == false) { //Si el personaje es Terrestre
            
            //Usamos 2 bucles por cada eje para ir del centro hacia fuera
            for (int fila = p->get_fila() ; fila <= (p->get_fila() + static_cast<int>(p->get_rangoTablero())); fila++) {

                if ( cuadricula[fila - 1][personaje_seleccionado->get_columna()] != nullptr && fila != p->get_fila()) {

                    if (cuadricula[fila - 1][personaje_seleccionado->get_columna()]->get_equipo() != p->get_equipo()) break;
                }

                if (cuadricula[fila][personaje_seleccionado->get_columna()] == nullptr && fila >= 0 && personaje_seleccionado->get_columna() >= 0 && fila <= 8 && personaje_seleccionado->get_columna() <= 8)  DrawCasillas(fila, personaje_seleccionado->get_columna());

                else if (cuadricula[fila][personaje_seleccionado->get_columna()] != nullptr && fila >= 0 && personaje_seleccionado->get_columna() >= 0 && fila <= 8 && personaje_seleccionado->get_columna() <= 8) {
                    
                    if (cuadricula[fila][personaje_seleccionado->get_columna()]->get_equipo() == p->get_equipo() && fila!= personaje_seleccionado->get_fila()) break;
                    if (cuadricula[fila][personaje_seleccionado->get_columna()]->get_equipo() != p->get_equipo()) DrawCasillas(fila, personaje_seleccionado->get_columna());

                }
               
                if (fila == 8)break;

            }
           
            for (int fila = p->get_fila() ; fila >= (p->get_fila() - static_cast<int>(p->get_rangoTablero())); fila--) {
                
                if ( cuadricula[fila + 1][personaje_seleccionado->get_columna()] != nullptr && fila!= p->get_fila()) {

                    if (cuadricula[fila + 1][personaje_seleccionado->get_columna()]->get_equipo() != p->get_equipo()) break;
                }

                if (cuadricula[fila][personaje_seleccionado->get_columna()] == nullptr && fila >= 0 && personaje_seleccionado->get_columna() >= 0 && fila <= 8 && personaje_seleccionado->get_columna() <= 8)  DrawCasillas(fila, personaje_seleccionado->get_columna());

                else if (cuadricula[fila][personaje_seleccionado->get_columna()] != nullptr && fila >= 0 && personaje_seleccionado->get_columna() >= 0 && fila <= 8 && personaje_seleccionado->get_columna() <= 8) {

                    if (cuadricula[fila][personaje_seleccionado->get_columna()]->get_equipo() == p->get_equipo() && fila != personaje_seleccionado->get_fila()) break;
                    if (cuadricula[fila][personaje_seleccionado->get_columna()]->get_equipo() != p->get_equipo()) DrawCasillas(fila, personaje_seleccionado->get_columna());

                }
                if (fila == 0)break;

            }

            for (int columna = p->get_columna() ; columna <= (p->get_columna() + static_cast<int>(p->get_rangoTablero())); columna++) {

                if ( cuadricula[personaje_seleccionado->get_fila()][columna - 1] != nullptr && columna != p->get_columna()) {

                    if (cuadricula[personaje_seleccionado->get_fila()][columna-1]->get_equipo() != p->get_equipo()) break;
                }

                if (cuadricula[personaje_seleccionado->get_fila()][columna] == nullptr && personaje_seleccionado->get_fila() >= 0 && columna >= 0)  DrawCasillas(personaje_seleccionado->get_fila(), columna);

                else if (cuadricula[personaje_seleccionado->get_fila()][columna] != nullptr && personaje_seleccionado->get_fila() >= 0 && columna >= 0 && personaje_seleccionado->get_fila() <= 8 && columna <=80) {

                    if (cuadricula[personaje_seleccionado->get_fila()][columna]->get_equipo() == p->get_equipo() && columna != personaje_seleccionado->get_columna()) break;
                    if (cuadricula[personaje_seleccionado->get_fila()][columna]->get_equipo() != p->get_equipo()) DrawCasillas(personaje_seleccionado->get_fila(), columna);

                }
                if (columna == 8)break;
            }

            for (int columna = p->get_columna(); columna >= (p->get_columna() - static_cast<int>(p->get_rangoTablero())); columna--) {

                if (cuadricula[personaje_seleccionado->get_fila()][columna + 1] != nullptr && columna != p->get_columna()) {

                    if (cuadricula[personaje_seleccionado->get_fila()][columna + 1]->get_equipo() != p->get_equipo()) break;
                }

                if (cuadricula[personaje_seleccionado->get_fila()][columna] == nullptr && personaje_seleccionado->get_fila() >= 0 && columna >= 0 && personaje_seleccionado->get_fila() <= 8 && columna <= 8)  DrawCasillas(personaje_seleccionado->get_fila(), columna);

                else if (cuadricula[personaje_seleccionado->get_fila()][columna] != nullptr && personaje_seleccionado->get_fila() >= 0 && columna >= 0 && personaje_seleccionado->get_fila() <= 8 && columna <= 8) {

                    if (cuadricula[personaje_seleccionado->get_fila()][columna]->get_equipo() == p->get_equipo() && columna != personaje_seleccionado->get_columna()) break;
                    if (cuadricula[personaje_seleccionado->get_fila()][columna]->get_equipo() != p->get_equipo()) DrawCasillas(personaje_seleccionado->get_fila(), columna);

                }

                if (columna == 0)break;
            }

        }
    }
}


void Tablero::DrawCasillas(int fila, int columna) {

    Color colorCasilla;
    if ((fila + columna) % 2 == 0) {
        colorCasilla = DARKBLUE;
    }
    else {
        colorCasilla = DARKPURPLE;
    }

    set_MovimientosPosibles(true, fila, columna);

    int posX = (970 / 2 - 4.5 * tamanoCasilla) + columna * tamanoCasilla; //970 es el largo de la pantalla
    int posY = (580 / 2 - 4.5 * tamanoCasilla) + fila * tamanoCasilla; //580 es la altura de la pantalla

    DrawRectangle(posX, posY, tamanoCasilla, tamanoCasilla, colorCasilla);

}





void Tablero::detectaGanador() {


    //Primera forma: no hay ninguna pieza de un equipo
    //Segunda forma: dominio de un equipo de todos los puntos de poder

    bool ganaLuz = true, ganaOscuridad = true;
    int contadorLuz = 0, contadorOscuridad = 0;
  

    for (int i = 0; i < 9; i++) {
        for (int j = 0; j < 9; j++) {

            //Las posiciones de puntos de poder son: [0][4], [4][1], [4][4], [4][8], [8][4]
            bool puntoPoder = (i == 0 && j == 4) || (i == 4 && j == 0) || (i == 4 && j == 4) || (i == 4 && j == 8) || (i == 8 && j == 4);
            
            if (cuadricula[i][j] != nullptr) { //Primero nos aseguramos que es un personaje

                if (cuadricula[i][j]->get_equipo() == LUZ) ganaOscuridad = false;
                if (cuadricula[i][j]->get_equipo() == OSCURIDAD)  ganaLuz = false;

                if (puntoPoder == true) {
                    
                    if (cuadricula[i][j]->get_equipo() == LUZ) contadorLuz++;
                    if (cuadricula[i][j]->get_equipo() == OSCURIDAD)  contadorOscuridad++;
                }
            }
        }
    }

    std::cout << contadorLuz << std::endl;

    if (ganaLuz == true||contadorLuz==5) { //Se acaba el juego, gana luz
        std::cout << "Gana Luz" << std::endl;
    }
    if (ganaOscuridad == true||contadorOscuridad==5) { //Se acaba el juego, gana oscuridad

        std::cout << "Gana Oscuridad" << std::endl;

    }











 }
 










