#pragma once
#include "GameState.h"
#include <fstream>
#include <sstream>
#include <ctime>
#include "Pj_info.h"
#include <cstdio>
#include <cstring>

inline constexpr const char* SAVE_FILE = "savegame.txt";

struct SaveData {
    std::string nombreP1, nombreP2;
    float vidaP1 = 0.f, posP1x = 0.f, posP1y = 0.f;
    float vidaP2 = 0.f, posP2x = 0.f, posP2y = 0.f;
    bool  modoIA = false;
    int   dificultad = 1;
    bool  valida = false;
};

// Sistema de guardado/carga de partidas en fichero de texto plano.
// Cada partida ocupa un bloque de líneas con clave=valor.
// Usamos texto en vez de binario para que sea legible y no dependa
// del compilador ni del sistema operativo.

class SaveSystem {
    // Reescribe el fichero entero con las partidas que quedan en memoria.
    static void reescribir(const GameState& gs);

public:
    // Guarda la partida actual al final del fichero. Devuelve true si va bien.
    static bool guardar(const GameState& gs, const std::string& nombrePartida);

    // Lee todas las partidas del fichero y rellena el vector.
    static void cargarTodas(GameState& gs);

    // Borra una partida por índice reescribiendo el fichero sin ese bloque.
    static void borrar(GameState& gs, int idx);

    // Aplica los datos de una partida guardada al GameState.
    static void restaurar(GameState& gs, int idx);

    static bool GuardarCombate(const SaveData& d);

    static SaveData CargarCombate();

    static bool ExisteGuardado();

    static const Pj_info* BuscarPjPorNombre(std::string_view nombre);
};
