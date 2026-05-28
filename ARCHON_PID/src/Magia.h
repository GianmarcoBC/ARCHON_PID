#pragma once
#include "PiezaTablero.h"
#include <vector>

class Tablero;

class Magia
{
    bool hechizosOscuridad[7]{ false };
    bool hechizosLuz[7]{ false };

public:
    Magia() = default;

    void Shift_Time(PiezaTablero* personaje, Tablero& t);
    void Teleport(PiezaTablero* personaje, Tablero& t);
    void Heal(PiezaTablero* personaje, Tablero& t);
    void Exchange(PiezaTablero* personaje, Tablero& t);
    void Imprison(PiezaTablero* personaje, Tablero& t);
    void Revive(PiezaTablero* personaje, Tablero& t);
    void EliminaMuerto(PiezaTablero* personaje_muerto_seleccionado, std::vector<PiezaTablero*>& Vector);
};
