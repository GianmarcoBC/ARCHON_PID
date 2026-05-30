#pragma once
#include "Tablero.h"
#include "TableroAI.h"
#include <future>
#include "AI_Agresiva_Tablero.h"
#include "AI_Defensiva_Tablero.h"
#include "AI_Facil_Tablero.h"



class Controlador_Tablero
{
	Tablero tablero;
	TableroAI* ia_Tablero = nullptr;
	equipo equipoAI = OSCURIDAD; //Esto se modificaría antes, desde el menu

	std::future<MovTablero> futuroMovIA_;
	bool calculandoIA_ = false;

public:
	Controlador_Tablero(bool vsAI, int dificultad, equipo equipoAI);
	~Controlador_Tablero() { delete ia_Tablero; }
	void Logica_controlador(bool vsAI);
	void draw() { tablero.Draw(); };
	ModoJuego cget_modoJuegoActual() { return tablero.get_modoJuegoActual(); };
	bool ccombatePendiente() const { return tablero.combatePendiente(); };
	tipo_pj getID_atacante() { return tablero.getAtacante()->get_ID(); };
	tipo_pj getID_defensor() { return  tablero.getDefensor()->get_ID(); };
	float getVidaAtacante() { return tablero.getAtacante()->get_vida(); }
	float getVidaDefensor() { return tablero.getDefensor()->get_vida(); }
	void cresolverCombate(bool ganaAtacante, float vidaGanador) { tablero.resolverCombate(ganaAtacante, vidaGanador); };
	bool getEquipoAtacante() const { return tablero.atacante_->get_equipo(); }
	bool getEquipoAI()       const { return equipoAI; }

	// Estado del combate 3D para guardado/carga (lo rellena TableroScreen)
	float combateVidaP1 = -1, combateVidaP2 = -1;
	float combatePosP1x = 0, combatePosP1y = 0;
	float combatePosP2x = 0, combatePosP2y = 0;

	void setCombateState(float v1, float px1, float py1, float v2, float px2, float py2) {
		combateVidaP1 = v1; combatePosP1x = px1; combatePosP1y = py1;
		combateVidaP2 = v2; combatePosP2x = px2; combatePosP2y = py2;
	}
	void clearCombateState() { combateVidaP1 = -1; }
	bool tieneCombateState() const { return combateVidaP1 >= 0; }

	// Serialización del estado para guardado/carga
	std::string GuardarEstado() const;
	void CargarEstado(const std::string& data);
};

