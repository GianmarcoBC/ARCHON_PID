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
	equipo equipoAI = OSCURIDAD; //Esto se modificaría antes, desde el menu



public: 
	Controlador_Tablero(bool vsAI, int dificultad);
	void Logica_controlador(bool vsAI);
	void draw() { tablero.Draw(); };
	ModoJuego cget_modoJuegoActual() { return tablero.get_modoJuegoActual(); };
	bool ccombatePendiente() const { return tablero.combatePendiente(); };
	tipo_pj getID_atacante() { return tablero.getAtacante()->get_ID(); };
	tipo_pj getID_defensor() { return  tablero.getDefensor()->get_ID(); };
	void cresolverCombate(bool ganaAtacante) { tablero.resolverCombate(ganaAtacante); };

};

