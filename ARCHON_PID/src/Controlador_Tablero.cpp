#include "Controlador_Tablero.h"
#include "Personaje.h"



Controlador_Tablero::Controlador_Tablero(bool vsAI, int dificultad, equipo equipoAI):tablero(), equipoAI(equipoAI) {

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

    if (tablero.get_modoJuegoActual() != ModoJuego::NORMAL) return;
    if (!vsAI || tablero.turno != equipoAI) return;

    // Lanzar el cálculo en un hilo aparte si no está ya en curso
    if (!calculandoIA_) {
        calculandoIA_ = true;
        futuroMovIA_ = std::async(std::launch::async,
            [this]() {
                return ia_Tablero->decidirMovimiento(tablero.cuadricula);
            });
        return;
    }

    // Comprobar si el hilo ya terminó (sin bloquear)
    if (futuroMovIA_.wait_for(std::chrono::seconds(0)) != std::future_status::ready) {
        return;
    }

    // El resultado está listo: recogerlo y aplicarlo
    MovTablero movimiento = futuroMovIA_.get();
    calculandoIA_ = false;

    PiezaTablero* personaje = tablero.cuadricula[movimiento.filaOrigen][movimiento.colOrigen];

    if (tablero.cuadricula[movimiento.filaDestino][movimiento.colDestino] != nullptr) {
        tablero.combatePendiente_ = true;
        tablero.atacante_ = personaje;
        tablero.defensor_ = tablero.cuadricula[movimiento.filaDestino][movimiento.colDestino];
        tablero.filaOrigenAtacante_ = movimiento.filaOrigen;
        tablero.colOrigenAtacante_ = movimiento.colOrigen;
    }

    tablero.piezaAnimando_ = personaje;
    tablero.filaDestinoAnim_ = movimiento.filaDestino;
    tablero.colDestinoAnim_ = movimiento.colDestino;
    personaje->iniciarMovimiento(movimiento.filaDestino, movimiento.colDestino, tablero.cellSize3D);
    tablero.modoJuegoactual = ModoJuego::ANIMANDO_MOVIMIENTO;
}