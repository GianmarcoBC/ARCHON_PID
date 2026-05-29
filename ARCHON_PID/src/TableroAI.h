#pragma once
#include "PiezaTablero.h"

struct MovTablero {
    int filaOrigen, colOrigen;  // Casilla donde esta la pieza ahora
    int filaDestino, colDestino; // Casilla destino
};

class TableroAI
{
protected:
    bool equipoIA_;
    int  profundidad_;

    // CeldaSim: version ligera de PiezaTablero para la simulacion interna.
    // Solo los campos que necesitan la logica de movimiento y la heuristica.
    // sizeof(CuadriculaSim) ~ 1944 bytes -> memcpy por nodo es barato.
    struct CeldaSim {
        bool         ocupada{ false };
        bool         equipo{ false };           // LUZ=false / OSCURIDAD=true
        tipo_pj      tipo{ tipo_pj::Knight };
        unsigned int rango{ 3 };
        bool         vuela{ false };
        bool         imprison{ false };
    };

    using CuadriculaSim = CeldaSim[9][9];

    bool esPuntoPoder(int fila, int col) const;

    // Convierte cuadricula real (PiezaTablero*) a CuadriculaSim
    void copiarTablero(PiezaTablero* const src[9][9], CuadriculaSim& dst) const;

    // Copia rapida entre dos CuadriculaSim (memcpy seguro: CeldaSim es POD)
    void copiarCuadriculaSim(const CuadriculaSim& src, CuadriculaSim& dst) const;

    // Genera todos los movimientos legales del equipo dado
    std::vector<MovTablero> generarMovimientos(const CuadriculaSim& tablero,
        bool equipo) const;

    // Adaptador: calcula casillas posibles sobre CuadriculaSim
    // Replica la misma logica que calcularCasillasPosibles de TableroAI
    void casillasPosiblesSim(const CuadriculaSim& tablero, int fila, int col,
        bool posibles[9][9]) const;

    // Mueve pieza de origen a destino en la copia simulada (captura incluida)
    void aplicarMovimiento(CuadriculaSim& tablero, const MovTablero& mov) const;

    virtual int evaluar(const CuadriculaSim& tablero) const = 0; // Heuristica de evaluacion

    // Valor de cada tipo de pieza (lideres valen 500 para priorizarlos)
    virtual int valorPieza(tipo_pj tipo) const = 0;

    // Minimax recursivo con poda Alfa-Beta
    //   alpha = mejor garantia del MAX (-INF al inicio)
    //   beta  = mejor garantia del MIN (+INF al inicio)
    //   Poda beta: en MAX, si alpha>=beta el MIN nunca elegiria esta rama
    //   Poda alfa: en MIN, si beta<=alpha el MAX nunca elegiria esta rama
    int minimax(CuadriculaSim& tablero, int profundidad,
        int alpha, int beta, bool maximizando) const;

public:
    TableroAI(bool equipoIA, int profundidad = 3) : equipoIA_(equipoIA), profundidad_(profundidad) {}
    virtual MovTablero decidirMovimiento(PiezaTablero* const tableroReal[9][9]) = 0;
};

