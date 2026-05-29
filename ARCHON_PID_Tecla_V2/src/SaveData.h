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