#pragma once
#include "TableroAI.h"

/*
 * AI_Facil_Tablero - IA Minimax con poda Alfa-Beta para el tablero de ARCHON
 *
 * PERSONALIDAD FÁCIL:
 *   - Prioriza la defensa y la conservación de piezas
 *   - Ocupa puntos de poder de manera defensiva
 *   - Evita sacrificar piezas sin una ganancia clara
 */

class AI_Facil_Tablero :public TableroAI {
    // Heuristica: >0 = ventaja IA, <0 = ventaja rival, +-100000 = terminal
    int evaluar(const CuadriculaSim& tablero) const override;

    // Valor de cada tipo de pieza (lideres valen 500 para priorizarlos)
    int valorPieza(tipo_pj tipo) const override;

public:
    // equipoIA   - LUZ=false / OSCURIDAD=true
    // profundidad- Niveles de busqueda. 3 = fluido; 4+ puede tardar.
    AI_Facil_Tablero(bool equipoIA);

    // Devuelve el mejor movimiento. tableroReal nunca se modifica.
    // Retorna {-1,-1,-1,-1} si no hay movimientos posibles.
    MovTablero decidirMovimiento(PiezaTablero* const tableroReal[9][9]) override;
};