#pragma once
#include "TableroAI.h"
#include "PiezaTablero.h"
#include "TiposPj.h"
#include <vector>
#include <limits>
#include <algorithm>

/*
 * AI_Agresiva_Tablero - IA Minimax con poda Alfa-Beta para el tablero de ARCHON
 *
 * PERSONALIDAD AGRESIVA:
 *   - Capturar piezas enemigas es su prioridad (especialmente el lider rival)
 *   - Ocupa puntos de poder activamente
 *   - Acepta sacrificar sus propias piezas si la ganancia lo justifica
 *   - Presiona hacia el centro desde el primer turno
 *
 * USO DESDE Controlador_Tablero:
 *   AI_Agresiva_Tablero ia(OSCURIDAD, 3);
 *   MovTablero mov = ia.decidirMovimiento(tablero.getCuadricula());
 *   // Aplicar mov simulando los pasos de moverPieza()
 */

class AI_Agresiva_Tablero :public TableroAI {

    // Heuristica: >0 = ventaja IA, <0 = ventaja rival, +-100000 = terminal
    int evaluar(const CuadriculaSim& tablero) const override;

    // Valor de cada tipo de pieza (lideres valen 500 para priorizarlos)
    int valorPieza(tipo_pj tipo) const override;

public:
    // equipoIA   - LUZ=false / OSCURIDAD=true
    // profundidad- Niveles de busqueda. 3 = fluido; 4+ puede tardar.
    AI_Agresiva_Tablero(bool equipoIA);

    // Devuelve el mejor movimiento. tableroReal nunca se modifica.
    // Retorna {-1,-1,-1,-1} si no hay movimientos posibles.
    MovTablero decidirMovimiento(PiezaTablero* const tableroReal[9][9]) override;
};