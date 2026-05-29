#pragma once
#include "Tablero.h"
#include "TableroAI.h"
#include "AI_Agresiva_Tablero.h"
#include "AI_Defensiva_Tablero.h"
#include "AI_Facil_Tablero.h"



class Controlador_Tablero
{
	Tablero tablero;
	TableroAI* ia_Tablero = nullptr;
	equipo equpoAI = OSCURIDAD; //Esto se modificaría antes, desde el menu



public: 
	Controlador_Tablero(bool vsAI, int dificultad);




};

