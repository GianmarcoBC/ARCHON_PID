#include "SaveManager.h"
#include <iostream>
#include <cstring>

// ── Ruta del fichero de guardado ──────────────────────────────────────────
const char* SaveManager::RUTA_GUARDADO = "savegame.txt";

// ── Lista completa de personajes para búsqueda por nombre ────────────────
static const Pj* TODOS_LOS_PJS[] = {
    &MH, &Phoenix, &Golem, &Djinni, &Unicorn, &Valkyrie, &Archer, &Knight,
    &Platero, &ShapeShifter, &Troll, &Dragon, &Basilisk, &Banshee, &Manticore, &Goblin
};
static constexpr int NUM_TOTAL_PJS = 16;

// ─────────────────────────────────────────────────────────────────────────
//  Parsea el fichero .txt con formato [seccion] / clave=valor
//  Devuelve mapa con claves "seccion.clave"
// ─────────────────────────────────────────────────────────────────────────
std::map<std::string, std::string>
SaveManager::Parsear(const std::string& ruta)
{
    std::map<std::string, std::string> resultado;
    std::ifstream f(ruta);
    if (!f.is_open()) return resultado;

    std::string linea, seccionActual;
    while (std::getline(f, linea)) {
        // Quitar espacios al inicio
        size_t ini = linea.find_first_not_of(" \t\r\n");
        if (ini == std::string::npos) continue;
        linea = linea.substr(ini);

        // Ignorar comentarios
        if (linea[0] == '#' || linea[0] == ';') continue;

        // Detectar sección [nombre]
        if (linea[0] == '[') {
            size_t fin = linea.find(']');
            if (fin != std::string::npos)
                seccionActual = linea.substr(1, fin - 1);
            continue;
        }

        // Parsear clave=valor
        size_t eq = linea.find('=');
        if (eq != std::string::npos && !seccionActual.empty()) {
            std::string clave = seccionActual + "." + linea.substr(0, eq);
            std::string valor = linea.substr(eq + 1);
            resultado[clave] = valor;
        }
    }
    return resultado;
}

float SaveManager::LeerFloat(const std::map<std::string, std::string>& m,
                              const std::string& clave, float defecto)
{
    auto it = m.find(clave);
    if (it == m.end()) return defecto;
    try { return std::stof(it->second); }
    catch (...) { return defecto; }
}

int SaveManager::LeerInt(const std::map<std::string, std::string>& m,
                          const std::string& clave, int defecto)
{
    auto it = m.find(clave);
    if (it == m.end()) return defecto;
    try { return std::stoi(it->second); }
    catch (...) { return defecto; }
}

std::string SaveManager::LeerStr(const std::map<std::string, std::string>& m,
                                  const std::string& clave,
                                  const std::string& defecto)
{
    auto it = m.find(clave);
    if (it == m.end()) return defecto;
    return it->second;
}

// ─────────────────────────────────────────────────────────────────────────
//  Guardar
// ─────────────────────────────────────────────────────────────────────────
bool SaveManager::Guardar(const SaveData& datos)
{
    std::ofstream f(RUTA_GUARDADO);
    if (!f.is_open()) {
        std::cerr << "[SaveManager] Error: no se pudo abrir " << RUTA_GUARDADO << " para escribir.\n";
        return false;
    }

    f << "# ARCHON PID - Fichero de partida guardada\n";
    f << "# No editar manualmente salvo que sepas lo que haces.\n\n";

    f << "[partida]\n";
    f << "modoIA="     << (datos.modoIA ? 1 : 0) << "\n";
    f << "dificultad=" << datos.dificultad        << "\n\n";

    f << "[jugador1]\n";
    f << "nombre=" << datos.nombreP1              << "\n";
    f << "vida="   << datos.vidaP1                << "\n";
    f << "posX="   << datos.posP1x                << "\n";
    f << "posY="   << datos.posP1y                << "\n\n";

    f << "[jugador2]\n";
    f << "nombre=" << datos.nombreP2              << "\n";
    f << "vida="   << datos.vidaP2                << "\n";
    f << "posX="   << datos.posP2x                << "\n";
    f << "posY="   << datos.posP2y                << "\n";

    f.close();
    return true;
}

// ─────────────────────────────────────────────────────────────────────────
//  Cargar
// ─────────────────────────────────────────────────────────────────────────
SaveData SaveManager::Cargar()
{
    SaveData d;
    auto m = Parsear(RUTA_GUARDADO);
    if (m.empty()) return d;  // d.valida == false

    d.modoIA     = (LeerInt(m, "partida.modoIA",     0) != 0);
    d.dificultad =  LeerInt(m, "partida.dificultad", 1);

    d.nombreP1 = LeerStr  (m, "jugador1.nombre", "");
    d.vidaP1   = LeerFloat(m, "jugador1.vida",   0.0f);
    d.posP1x   = LeerFloat(m, "jugador1.posX",   100.0f);
    d.posP1y   = LeerFloat(m, "jugador1.posY",   100.0f);

    d.nombreP2 = LeerStr  (m, "jugador2.nombre", "");
    d.vidaP2   = LeerFloat(m, "jugador2.vida",   0.0f);
    d.posP2x   = LeerFloat(m, "jugador2.posX",   800.0f);
    d.posP2y   = LeerFloat(m, "jugador2.posY",   100.0f);

    // Validación mínima: nombres no vacíos y vida > 0
    d.valida = (!d.nombreP1.empty() && !d.nombreP2.empty()
                && d.vidaP1 > 0.0f && d.vidaP2 > 0.0f);

    return d;
}

// ─────────────────────────────────────────────────────────────────────────
//  ExisteGuardado
// ─────────────────────────────────────────────────────────────────────────
bool SaveManager::ExisteGuardado()
{
    std::ifstream f(RUTA_GUARDADO);
    return f.good();
}

// ─────────────────────────────────────────────────────────────────────────
//  BuscarPjPorNombre
// ─────────────────────────────────────────────────────────────────────────
const Pj* SaveManager::BuscarPjPorNombre(const std::string& nombre)
{
    for (int i = 0; i < NUM_TOTAL_PJS; ++i)
        if (nombre == TODOS_LOS_PJS[i]->nombre)
            return TODOS_LOS_PJS[i];
    return nullptr;
}
