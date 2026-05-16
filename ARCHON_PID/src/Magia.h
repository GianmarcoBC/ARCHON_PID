#pragma once

#include "Personaje.h"

class Tablero;

class Magia
{
    bool hechizosOscuridad[7]{ false };
    bool hechizosLuz[7]{ false };



public:
  

   Magia() = default;

   void Shift_Time(Personaje* personaje, Tablero& t);
   void Teleport(Personaje* personaje, Tablero& t);
   void Heal(Personaje* personaje, Tablero& t);
   void Exchange(Personaje* personaje, Tablero& t);
   void Imprison(Personaje* personaje, Tablero& t);
   void Revive(Personaje* personaje, Tablero& t);
};

