#include "TableroAI.h"


// esPuntoPoder - true si (fila, col) es uno de los 5 puntos de poder.
//
// Forman una cruz:
//   Centro:            (4, 4)
//   Extremo superior:  (0, 4)
//   Extremo inferior:  (8, 4)
//   Extremo izquierdo: (4, 0)
//   Extremo derecho:   (4, 8)


bool TableroAI::esPuntoPoder(int fila, int col) const
{
    {
        return (fila == 4 && col == 4)   // Centro
            || (fila == 0 && col == 4)   // Arriba
            || (fila == 8 && col == 4)   // Abajo
            || (fila == 4 && col == 0)   // Izquierda
            || (fila == 4 && col == 8);  // Derecha
    }
}


// =============================================================================
// copiarTablero
// =============================================================================

void TableroAI::copiarTablero(PiezaTablero* const src[9][9],
    CuadriculaSim& dst) const
{
    for (int i = 0; i < 9; i++)
        for (int j = 0; j < 9; j++) {
            if (src[i][j] == nullptr) {
                dst[i][j] = CeldaSim{};     // Casilla vacia
            }
            else {
                dst[i][j].ocupada = true;
                dst[i][j].equipo = src[i][j]->get_equipo();
                dst[i][j].tipo = src[i][j]->get_ID();
                dst[i][j].rango = src[i][j]->get_rangoTablero();
                dst[i][j].vuela = src[i][j]->get_vuela();
                dst[i][j].imprison = src[i][j]->get_imprison();
            }
        }
}

// =============================================================================
// copiarCuadriculaSim
// =============================================================================

void TableroAI::copiarCuadriculaSim(const CuadriculaSim& src,
    CuadriculaSim& dst) const
{
    // CeldaSim es POD (sin punteros ni vtable): memcpy es seguro y rapido
    std::memcpy(&dst, &src, sizeof(CuadriculaSim));
}

// =============================================================================
// casillasPosiblesSim - Adaptador CuadriculaSim -> logica compartida
// =============================================================================

void TableroAI::casillasPosiblesSim(const CuadriculaSim& tablero,
    int fila, int col,
    bool posibles[9][9]) const
{
    for (int i = 0; i < 9; i++)
        for (int j = 0; j < 9; j++)
            posibles[i][j] = false;

    const CeldaSim& p = tablero[fila][col];
    if (!p.ocupada || p.imprison) return; // Vacia o aprisionada

    int  rango = (int)p.rango;
    bool equipo = p.equipo;

    // Voladores: area cuadrada
    if (p.vuela) {
        for (int r = fila - rango; r <= fila + rango; r++)
            for (int c = col - rango; c <= col + rango; c++) {
                if (r < 0 || c < 0 || r > 8 || c > 8) continue;
                if (!tablero[r][c].ocupada || tablero[r][c].equipo != equipo)
                    posibles[r][c] = true;
            }
        return;
    }

    // Terrestres: cruz con bloqueo (misma logica que calcularCasillasPosibles)
    for (int r = fila + 1; r <= fila + rango && r <= 8; r++) { // ABAJO
        if (!tablero[r][col].ocupada) { posibles[r][col] = true; }
        else if (tablero[r][col].equipo != equipo) { posibles[r][col] = true; break; }
        else { break; }
    }
    for (int r = fila - 1; r >= fila - rango && r >= 0; r--) { // ARRIBA
        if (!tablero[r][col].ocupada) { posibles[r][col] = true; }
        else if (tablero[r][col].equipo != equipo) { posibles[r][col] = true; break; }
        else { break; }
    }
    for (int c = col + 1; c <= col + rango && c <= 8; c++) {  // DERECHA
        if (!tablero[fila][c].ocupada) { posibles[fila][c] = true; }
        else if (tablero[fila][c].equipo != equipo) { posibles[fila][c] = true; break; }
        else { break; }
    }
    for (int c = col - 1; c >= col - rango && c >= 0; c--) {  // IZQUIERDA
        if (!tablero[fila][c].ocupada) { posibles[fila][c] = true; }
        else if (tablero[fila][c].equipo != equipo) { posibles[fila][c] = true; break; }
        else { break; }
    }
}

// =============================================================================
// generarMovimientos
// =============================================================================

std::vector<MovTablero> TableroAI::generarMovimientos(
    const CuadriculaSim& tablero, bool equipo) const
{
    std::vector<MovTablero> movs;
    bool posibles[9][9];

    for (int i = 0; i < 9; i++)
        for (int j = 0; j < 9; j++) {
            if (!tablero[i][j].ocupada || tablero[i][j].equipo != equipo) continue;
            casillasPosiblesSim(tablero, i, j, posibles);
            for (int r = 0; r < 9; r++)
                for (int c = 0; c < 9; c++)
                    if (posibles[r][c])
                        movs.push_back({ i, j, r, c });
        }
    return movs;
}

// =============================================================================
// aplicarMovimiento
// =============================================================================

void TableroAI::aplicarMovimiento(CuadriculaSim& tablero,
    const MovTablero& mov) const
{
    // Destino recibe la pieza (captura al enemigo si habia uno)
    tablero[mov.filaDestino][mov.colDestino] = tablero[mov.filaOrigen][mov.colOrigen];
    tablero[mov.filaOrigen][mov.colOrigen] = CeldaSim{}; // Origen queda vacio
}


// =============================================================================
// minimax con poda Alfa-Beta
// =============================================================================

int TableroAI::minimax(CuadriculaSim& tablero, int profundidad,
    int alpha, int beta, bool maximizando) const
{
    if (profundidad == 0)
        return evaluar(tablero); // Hoja: evaluar directamente sin expandir mas

    bool equipoActual = maximizando ? equipoIA_ : !equipoIA_;
    std::vector<MovTablero> movimientos = generarMovimientos(tablero, equipoActual);

    if (movimientos.empty())
        return evaluar(tablero); // Sin movimientos: estado terminal

    if (maximizando) {
        // Nodo MAX: la IA elige el movimiento de mayor valor
        int mejorValor = std::numeric_limits<int>::min();

        for (const MovTablero& mov : movimientos) {
            CuadriculaSim copia;
            copiarCuadriculaSim(tablero, copia);
            aplicarMovimiento(copia, mov);

            int valor = minimax(copia, profundidad - 1, alpha, beta, false);
            mejorValor = std::max(mejorValor, valor);
            alpha = std::max(alpha, mejorValor);

            // PODA BETA: el minimizador en un nivel superior ya tiene algo
            // mejor que lo que podemos garantizarnos aqui. Cortar la rama.
            if (alpha >= beta) break;
        }
        return mejorValor;

    }
    else {
        // Nodo MIN: el rival elige el movimiento de menor valor para la IA
        int peorValor = std::numeric_limits<int>::max();

        for (const MovTablero& mov : movimientos) {
            CuadriculaSim copia;
            copiarCuadriculaSim(tablero, copia);
            aplicarMovimiento(copia, mov);

            int valor = minimax(copia, profundidad - 1, alpha, beta, true);
            peorValor = std::min(peorValor, valor);
            beta = std::min(beta, peorValor);

            // PODA ALFA: el maximizador en un nivel superior ya tiene algo
            // mejor que lo que el rival puede ofrecernos por esta rama. Cortar.
            if (beta <= alpha) break;
        }
        return peorValor;
    }
}