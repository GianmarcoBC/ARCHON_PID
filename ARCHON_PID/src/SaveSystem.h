#pragma once
#include "GameState.h"
#include <fstream>
#include <sstream>
#include <ctime>

// Sistema de guardado/carga de partidas en fichero de texto plano.
// Cada partida ocupa un bloque de líneas con clave=valor.
// Usamos texto en vez de binario para que sea legible y no dependa
// del compilador ni del sistema operativo.

class SaveSystem {
public:
    // Guarda la partida actual al final del fichero. Devuelve true si va bien.
    static bool guardar(const GameState& gs, const std::string& nombrePartida) {
        std::ofstream f("partidas.sav", std::ios::app);
        if (!f.is_open()) return false;

        // Timestamp para la fecha de guardado
        time_t ahora = time(nullptr);
        char fechaBuf[32];
        struct tm tmInfo;
        localtime_s(&tmInfo, &ahora);
        strftime(fechaBuf, sizeof(fechaBuf), "%d/%m/%Y %H:%M", &tmInfo);

        // Bloque de datos — formato clave=valor, separado por ---
        f << "---\n";
        f << "nombre="   << nombrePartida << "\n";
        f << "fecha="    << fechaBuf      << "\n";
        f << "modo="     << (gs.modoActual == MODO_COMBATE ? "Combate" : "Juego Completo") << "\n";
        f << "bando="    << (gs.bandoSel == BANDO_LUZ ? "Luz" :
                             gs.bandoSel == BANDO_OSCURIDAD ? "Oscuridad" : "Random") << "\n";
        f << "turno="    << 1             << "\n";   // el juego real lo irá actualizando
        f << "progreso=" << 0.0f          << "\n";
        f << "equipo1="  << gs.equipoSel1 << "\n";
        f << "equipo2="  << gs.equipoSel2 << "\n";
        f << "dificultad=" << (int)gs.dificultadSel << "\n";
        f.close();
        return true;
    }

    // Lee todas las partidas del fichero y rellena el vector.
    static void cargarTodas(GameState& gs) {
        gs.partidas.clear();
        std::ifstream f("partidas.sav");
        if (!f.is_open()) return;

        std::string linea;
        PartidaGuardada p{};
        bool dentroDeBloque = false;

        while (std::getline(f, linea)) {
            if (linea == "---") {
                // Si ya teníamos datos de un bloque anterior, guardarlo
                if (dentroDeBloque && !p.nombre.empty())
                    gs.partidas.push_back(p);
                p = PartidaGuardada{};
                dentroDeBloque = true;
                continue;
            }
            if (!dentroDeBloque) continue;

            // Partir por el primer '='
            auto eq = linea.find('=');
            if (eq == std::string::npos) continue;
            std::string clave = linea.substr(0, eq);
            std::string valor = linea.substr(eq + 1);

            if      (clave == "nombre")     p.nombre    = valor;
            else if (clave == "fecha")      p.fecha     = valor;
            else if (clave == "modo")       p.modo      = valor;
            else if (clave == "bando")      p.bando     = valor;
            else if (clave == "turno")      p.turno     = std::stoi(valor);
            else if (clave == "progreso")   p.progreso  = std::stof(valor);
            else if (clave == "equipo1")    p.equipo1   = std::stoi(valor);
            else if (clave == "equipo2")    p.equipo2   = std::stoi(valor);
            else if (clave == "dificultad") p.dificultad= std::stoi(valor);
        }
        // El último bloque no tiene "---" de cierre
        if (dentroDeBloque && !p.nombre.empty())
            gs.partidas.push_back(p);

        f.close();
    }

    // Borra una partida por índice reescribiendo el fichero sin ese bloque.
    static void borrar(GameState& gs, int idx) {
        if (idx < 0 || idx >= (int)gs.partidas.size()) return;
        gs.partidas.erase(gs.partidas.begin() + idx);
        reescribir(gs);
    }

    // Aplica los datos de una partida guardada al GameState.
    static void restaurar(GameState& gs, int idx) {
        if (idx < 0 || idx >= (int)gs.partidas.size()) return;
        const PartidaGuardada& p = gs.partidas[idx];
        gs.modoActual  = (p.modo == "Combate") ? MODO_COMBATE : MODO_COMPLETO;
        gs.equipoSel1  = p.equipo1;
        gs.equipoSel2  = p.equipo2;
        gs.dificultadSel = (Dificultad)p.dificultad;
        gs.bandoSel    = (p.bando == "Luz")       ? BANDO_LUZ :
                         (p.bando == "Oscuridad") ? BANDO_OSCURIDAD : BANDO_RANDOM;
    }

private:
    // Reescribe el fichero entero con las partidas que quedan en memoria.
    static void reescribir(const GameState& gs) {
        std::ofstream f("partidas.sav", std::ios::trunc);
        if (!f.is_open()) return;
        for (const auto& p : gs.partidas) {
            f << "---\n";
            f << "nombre="     << p.nombre     << "\n";
            f << "fecha="      << p.fecha      << "\n";
            f << "modo="       << p.modo       << "\n";
            f << "bando="      << p.bando      << "\n";
            f << "turno="      << p.turno      << "\n";
            f << "progreso="   << p.progreso   << "\n";
            f << "equipo1="    << p.equipo1    << "\n";
            f << "equipo2="    << p.equipo2    << "\n";
            f << "dificultad=" << p.dificultad << "\n";
        }
        f.close();
    }
};
