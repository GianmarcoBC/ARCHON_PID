#pragma once
#include <string>
#include <array> 
#include <vector>

inline constexpr const char* SAVE_FILE = "savegame.txt";

struct SaveData {
    int   slot = 0;
    std::string nombreP1, nombreP2;
    float vidaP1 = 0.f, posP1x = 0.f, posP1y = 0.f;
    float vidaP2 = 0.f, posP2x = 0.f, posP2y = 0.f;
    bool  modoIA = false;
    int   dificultad = 1;
    bool  valida = false;
};

// Partida guardada — lo que se escribe en disco
struct PartidaGuardada {
    int   slot       = -1;  // slot 0-3 (-1 = sin asignar)
    std::string nombre, fecha, modo, bando;
    int   turno      = 0;
    float progreso   = 0.f;
    int   equipo1    = 0;   // índice en gs.equipos
    int   equipo2    = 1;
    int   dificultad = 1;   // valor del enum Dificultad
    // --- Estado del tablero (solo Juego Completo) ---
    bool  vsAI       = true;
    int   equipoIA   = 1;   // 0=LUZ, 1=OSCURIDAD
    std::string boardState;  // Estado serializado del tablero (vacío = partida nueva)
    bool  valida     = false; // slot ocupado
};