#include "AI_Facil_Tablero.h"
#include <cstring>
#include <algorithm>

AI_Facil_Tablero::AI_Facil_Tablero(bool equipoIA)
    : TableroAI(equipoIA, 2)
{}

// =============================================================================
// decidirMovimiento
// =============================================================================

MovTablero AI_Facil_Tablero::decidirMovimiento(PiezaTablero* const tableroReal[9][9])
{
    CuadriculaSim sim;
    copiarTablero(tableroReal, sim); // Snapshot del tablero real; no se toca mas

    std::vector<MovTablero> movimientos = generarMovimientos(sim, equipoIA_);

    MovTablero mejorMov{ -1, -1, -1, -1 };
    int mejorValor = std::numeric_limits<int>::min();

    for (const MovTablero& mov : movimientos) {
        CuadriculaSim copia;
        copiarCuadriculaSim(sim, copia); // Cada movimiento se prueba en una copia
        aplicarMovimiento(copia, mov);

        // Tras el movimiento de la IA le toca al rival (minimizador)
        int valor = minimax(copia, profundidad_ - 1,
            std::numeric_limits<int>::min(),  // alpha = -INF
            std::numeric_limits<int>::max(),  // beta  = +INF
            false);                           // siguiente: rival (MIN)

        if (valor > mejorValor) { mejorValor = valor; mejorMov = mov; }
    }

    return mejorMov;
}

// =============================================================================
// valorPieza
// =============================================================================

int AI_Facil_Tablero::valorPieza(tipo_pj tipo) const
{
    switch (tipo) {
    case tipo_pj::MH:           return 300; // Lider: objetivo prioritario
    case tipo_pj::Platero:      return 300;
    case tipo_pj::Dragon:       return 90;  // Voladores de alto rango
    case tipo_pj::Phoenix:      return 90;
    case tipo_pj::Djinni:       return 80;
    case tipo_pj::Banshee:      return 75;
    case tipo_pj::Valkyrie:     return 70;
    case tipo_pj::ShapeShifter: return 70;
    case tipo_pj::Golem:        return 60;  // Terrestres de apoyo
    case tipo_pj::Troll:        return 60;
    case tipo_pj::Unicorn:      return 55;
    case tipo_pj::Basilisk:     return 55;
    case tipo_pj::Manticore:    return 55;
    case tipo_pj::Archer:       return 50;
    case tipo_pj::Knight:       return 30;  // Peones: prescindibles
    case tipo_pj::Goblin:       return 30;
    default:                    return 30;
    }
}

// =============================================================================
// evaluar
// =============================================================================

int AI_Facil_Tablero::evaluar(const CuadriculaSim& tablero) const
{
    int puntuacion = 0;
    int contIA = 0, contRival = 0;
    int puntosPoderIA = 0, puntosPoderRival = 0;

    for (int i = 0; i < 9; i++) {
        for (int j = 0; j < 9; j++) {
            if (!tablero[i][j].ocupada) continue;

            bool esIA = (tablero[i][j].equipo == equipoIA_);
            int  valor = valorPieza(tablero[i][j].tipo);
            bool enPoder = esPuntoPoder(i, j); // De TableroAI.h, misma logica que el juego

            if (esIA) {
                contIA++;
                puntuacion += valor;  // Valor material propio

                if (enPoder) {
                    puntosPoderIA++;
                    puntuacion += 120; // Bonus: ocupar punto de poder es muy valioso
                }

                // Bonus de posicion: distancia Manhattan al centro (4,4).
                // Max distancia = 8 (esquinas). Bonus = (8-dist)*2, max +16.
                // Hace que la IA avance sin necesidad de atacar en cada turno.
                int dist = std::abs(i - 4) + std::abs(j - 4);
                puntuacion += (8 - dist) * 2;
            }
            else {
                contRival++;
                puntuacion -= valor;  // Restar valor material rival

                if (enPoder) {
                    puntosPoderRival++;
                    puntuacion -= 120; // Penalizacion por punto de poder rival
                }
            }
        }
    }

    // Estado terminal (condicion de victoria por eliminacion)
    if (contRival == 0) return  100000; // Victoria
    if (contIA == 0) return -100000;  // Derrota

    // Control global de puntos de poder (segunda condicion de victoria)
    puntuacion += puntosPoderIA * 80;
    puntuacion -= puntosPoderRival * 80;

    // Dominancia numerica: la IA agresiva prefiere tener mas piezas en juego
    puntuacion += (contIA - contRival) * 15;

    return puntuacion;
}

