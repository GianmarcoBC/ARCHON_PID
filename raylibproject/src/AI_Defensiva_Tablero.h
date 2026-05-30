#pragma once
#include "TableroAI.h"

/*
 * AI_Defensiva_Tablero - IA Minimax con poda Alfa-Beta para el tablero de ARCHON
 *
 * PERSONALIDAD DEFENSIVA:
 *   - Proteger sus piezas es su prioridad (especialmente el lider propio)
 *   - Ocupa puntos de poder defensivamente
 *   - Evita sacrificar sus propias piezas a menos que sea necesario
 *   - Mantiene una posición sólida y se defiende activamente
 *   - Presiona hacia el centro desde el primer turno
 *
 * USO DESDE Controlador_Tablero:
 *   AI_Defensiva_Tablero ia(OSCURIDAD, 3);
 *   MovTablero mov = ia.decidirMovimiento(tablero.getCuadricula());
 *   // Aplicar mov simulando los pasos de moverPieza()
 */

class AI_Defensiva_Tablero:public TableroAI
{
    // Heuristica: >0 = ventaja IA, <0 = ventaja rival, +-100000 = terminal
    int evaluar(const CuadriculaSim& tablero) const override;

    // Valor de cada tipo de pieza (lideres valen 500 para priorizarlos)
    int valorPieza(tipo_pj tipo) const override;

public:
    // equipoIA   - LUZ=false / OSCURIDAD=true
    // profundidad- Niveles de busqueda. 3 = fluido; 4+ puede tardar.
    AI_Defensiva_Tablero(bool equipoIA);

    // Devuelve el mejor movimiento. tableroReal nunca se modifica.
    // Retorna {-1,-1,-1,-1} si no hay movimientos posibles.
    MovTablero decidirMovimiento(PiezaTablero* const tableroReal[9][9]) override;
};
