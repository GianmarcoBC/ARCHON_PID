#pragma once
#include "SaveData.h"
#include <fstream>
#include <sstream>
#include <ctime>
#include "cuchau/Pj_info.h"
#include <cstdio>
#include <cstring>
#include <array>
struct GameState;

// Sistema de guardado/carga de partidas en fichero de texto plano.
// Cada partida ocupa un bloque de líneas con clave=valor.
// Usamos texto en vez de binario para que sea legible y no dependa
// del compilador ni del sistema operativo.

class SaveSystem {

	friend class CuchauCombateScreen;
	friend class CargarPartidaScreen;

    inline static SaveData pendiente{};

    // Reescribe el fichero entero con las partidas que quedan en memoria.
    static void reescribir(const GameState& gs);

    static std::vector<SaveData> CargarTodosCombate();

public:


    // Guarda la partida actual al final del fichero. Devuelve true si va bien.
    static bool guardar(const GameState& gs, const std::string& nombrePartida);

    // Lee todas las partidas del fichero y rellena el vector.
    static void cargarTodas(GameState& gs);

    // Borra una partida por índice reescribiendo el fichero sin ese bloque.
    static void borrar(GameState& gs, int idx);

    // Aplica los datos de una partida guardada al GameState.
    static void restaurar(GameState& gs, int idx);

    // Actualiza una partida existente en partidas.sav por índice
    static void actualizarPartida(GameState& gs, int idx);

    static bool GuardarCombate(const SaveData& d);

    static SaveData CargarCombate(int slot = 0);

	static bool ExisteGuardado(int slot = -1);

    static const Pj_info* BuscarPjPorNombre(std::string_view nombre);

    static void BorrarCombate(int slot);

    static std::array<SaveData, 4> ObtenerSlots();

    // Slots de partidas de tablero (Juego Completo)
    static std::array<PartidaGuardada, 4> ObtenerSlotsTablero();
    static void GuardarSlotTablero(const PartidaGuardada& p);
    static void BorrarSlotTablero(int slot);
};
