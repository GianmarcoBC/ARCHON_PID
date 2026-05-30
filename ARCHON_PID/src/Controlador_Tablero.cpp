#include "Controlador_Tablero.h"
#include "Personaje.h"



Controlador_Tablero::Controlador_Tablero(bool vsAI, int dificultad):tablero() {

    tablero.inicializarTablero();

    if (vsAI) {

        switch (dificultad) {
        case 0:  ia_Tablero = new AI_Facil_Tablero(equipoAI); break;
        case 1:  ia_Tablero = new AI_Defensiva_Tablero(equipoAI); break;
        case 2:  ia_Tablero = new AI_Agresiva_Tablero(equipoAI); break;
        default: ia_Tablero = new AI_Facil_Tablero(equipoAI); break;
        }

    }
    
}

void Controlador_Tablero::Logica_controlador(bool vsAI) {

   tablero.LogicaTablero();

    if (tablero.turno == equipoAI && vsAI == true) {

         MovTablero movimiento = ia_Tablero->decidirMovimiento(tablero.cuadricula);

         PiezaTablero* personaje = tablero.cuadricula[movimiento.filaOrigen][movimiento.colOrigen];


         if (tablero.cuadricula[movimiento.filaDestino][movimiento.colDestino] != nullptr) {
             // La casilla destino tiene una pieza -> comprobar si es enemiga
             PiezaTablero* personajeAtacado = tablero.cuadricula[movimiento.filaDestino][movimiento.colDestino];

             
                 // Pieza enemiga: guardar datos y lanzar combate 3D
                 tablero.combatePendiente_ = true;
                 tablero.atacante_ = tablero.cuadricula[movimiento.filaOrigen][movimiento.colOrigen];
                 tablero.defensor_ = personajeAtacado;
                 tablero.filaOrigenAtacante_ = movimiento.filaOrigen;
                 tablero.colOrigenAtacante_ = movimiento.colOrigen;
                 tablero.modoJuegoactual = ModoJuego::COMBATE;
             

         }

         tablero.piezaAnimando_ = personaje;
         tablero.cuadricula[movimiento.filaOrigen][movimiento.colOrigen] = nullptr;
         tablero.cuadricula[movimiento.filaDestino][movimiento.colDestino] = personaje;
         tablero.cuadricula[movimiento.filaDestino][movimiento.colDestino]->set_fila_columna(movimiento.filaDestino, movimiento.colDestino);
         
         personaje->iniciarMovimiento(movimiento.filaDestino, movimiento.colDestino, tablero.cellSize3D);
         tablero.filaDestinoAnim_ = movimiento.filaDestino;
         tablero.colDestinoAnim_ = movimiento.colDestino;
         tablero.modoJuegoactual = ModoJuego::ANIMANDO_MOVIMIENTO;

         
    }

}