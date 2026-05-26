#include "SaveSystem.h"

// Reescribe el fichero entero con las partidas que quedan en memoria.
void SaveSystem::reescribir(const GameState& gs)
{
    std::ofstream f("partidas.sav", std::ios::trunc);
    if (!f.is_open()) return;
    for (const auto& p : gs.partidas) {
        f << "---\n";
        f << "nombre=" << p.nombre << "\n";
        f << "fecha=" << p.fecha << "\n";
        f << "modo=" << p.modo << "\n";
        f << "bando=" << p.bando << "\n";
        f << "turno=" << p.turno << "\n";
        f << "progreso=" << p.progreso << "\n";
        f << "equipo1=" << p.equipo1 << "\n";
        f << "equipo2=" << p.equipo2 << "\n";
        f << "dificultad=" << p.dificultad << "\n";
    }
    f.close();
}

bool SaveSystem::guardar(const GameState& gs, const std::string& nombrePartida)
{
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
    f << "nombre=" << nombrePartida << "\n";
    f << "fecha=" << fechaBuf << "\n";
    f << "modo=" << (gs.modoActual == MODO_COMBATE ? "Combate" : "Juego Completo") << "\n";
    f << "bando=" << (gs.bandoSel == BANDO_LUZ ? "Luz" :
        gs.bandoSel == BANDO_OSCURIDAD ? "Oscuridad" : "Random") << "\n";
    f << "turno=" << 1 << "\n";   // el juego real lo irá actualizando
    f << "progreso=" << 0.0f << "\n";
    f << "equipo1=" << gs.equipoSel1 << "\n";
    f << "equipo2=" << gs.equipoSel2 << "\n";
    f << "dificultad=" << (int)gs.dificultadSel << "\n";
    f.close();
    return true;
}

void SaveSystem::cargarTodas(GameState& gs)
{
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

        if (clave == "nombre")     p.nombre = valor;
        else if (clave == "fecha")      p.fecha = valor;
        else if (clave == "modo")       p.modo = valor;
        else if (clave == "bando")      p.bando = valor;
        else if (clave == "turno")      p.turno = std::stoi(valor);
        else if (clave == "progreso")   p.progreso = std::stof(valor);
        else if (clave == "equipo1")    p.equipo1 = std::stoi(valor);
        else if (clave == "equipo2")    p.equipo2 = std::stoi(valor);
        else if (clave == "dificultad") p.dificultad = std::stoi(valor);
    }
    // El último bloque no tiene "---" de cierre
    if (dentroDeBloque && !p.nombre.empty())
        gs.partidas.push_back(p);

    f.close();
}

void SaveSystem::borrar(GameState& gs, int idx)
{
    if (idx < 0 || idx >= (int)gs.partidas.size()) return;
    gs.partidas.erase(gs.partidas.begin() + idx);
    reescribir(gs);
}

void SaveSystem::restaurar(GameState& gs, int idx)
{
    if (idx < 0 || idx >= (int)gs.partidas.size()) return;
    const PartidaGuardada& p = gs.partidas[idx];
    gs.modoActual = (p.modo == "Combate") ? MODO_COMBATE : MODO_COMPLETO;
    gs.equipoSel1 = p.equipo1;
    gs.equipoSel2 = p.equipo2;
    gs.dificultadSel = (Dificultad)p.dificultad;
    gs.bandoSel = (p.bando == "Luz") ? BANDO_LUZ :
        (p.bando == "Oscuridad") ? BANDO_OSCURIDAD : BANDO_RANDOM;
}

bool SaveSystem::GuardarCombate(const SaveData& d)
{
    FILE* f = fopen(SAVE_FILE, "w");
    if (!f) return false;
    fprintf(f, "# ARCHON 3D - Fichero de partida guardada\n");
    fprintf(f, "# No editar manualmente salvo que sepas lo que haces.\n\n");
    fprintf(f, "[partida]\nmodoIA=%d\ndificultad=%d\n", d.modoIA ? 1 : 0, d.dificultad);
    fprintf(f, "\n[jugador1]\nnombre=%s\nvida=%g\nposX=%g\nposY=%g\n",
        d.nombreP1.c_str(), d.vidaP1, d.posP1x, d.posP1y);
    fprintf(f, "\n[jugador2]\nnombre=%s\nvida=%g\nposX=%g\nposY=%g\n",
        d.nombreP2.c_str(), d.vidaP2, d.posP2x, d.posP2y);
    fclose(f);
    return true;
}

SaveData SaveSystem::CargarCombate()
{
    SaveData d{};
    FILE* f = fopen(SAVE_FILE, "r");
    if (!f) return d;

    char linea[256];
    int seccion = 0;

    while (fgets(linea, sizeof(linea), f)) {
        if (linea[0] == '#' || linea[0] == '\n' || linea[0] == '\r') continue;
        if (strncmp(linea, "[partida]", 9) == 0) { seccion = 1; continue; }
        if (strncmp(linea, "[jugador1]", 10) == 0) { seccion = 2; continue; }
        if (strncmp(linea, "[jugador2]", 10) == 0) { seccion = 3; continue; }

        char clave[64]{}, valor[128]{};
        if (sscanf(linea, "%63[^=]=%127[^\n\r]", clave, valor) != 2) continue;

        if (seccion == 1) {
            if (strcmp(clave, "modoIA") == 0) d.modoIA = (atoi(valor) != 0);
            if (strcmp(clave, "dificultad") == 0) d.dificultad = atoi(valor);
        }
        else if (seccion == 2) {
            if (strcmp(clave, "nombre") == 0) d.nombreP1 = valor;
            if (strcmp(clave, "vida") == 0) d.vidaP1 = (float)atof(valor);
            if (strcmp(clave, "posX") == 0) d.posP1x = (float)atof(valor);
            if (strcmp(clave, "posY") == 0) d.posP1y = (float)atof(valor);
        }
        else if (seccion == 3) {
            if (strcmp(clave, "nombre") == 0) d.nombreP2 = valor;
            if (strcmp(clave, "vida") == 0) d.vidaP2 = (float)atof(valor);
            if (strcmp(clave, "posX") == 0) d.posP2x = (float)atof(valor);
            if (strcmp(clave, "posY") == 0) d.posP2y = (float)atof(valor);
        }
    }
    fclose(f);
    d.valida = (!d.nombreP1.empty() && !d.nombreP2.empty());
    return d;
}

bool SaveSystem::ExisteGuardado()
{
    FILE* f = fopen(SAVE_FILE, "r");
    if (!f) return false;
    fclose(f);
    return true;
}

const Pj_info* SaveSystem::BuscarPjPorNombre(std::string_view nombre)
{
    static const Pj_info* todos[] = {
        &MH, &Phoenix, &Golem, &Djinni, &Unicorn, &Valkyrie, &Archer, &Knight,
        &Platero, &ShapeShifter, &Troll, &Dragon, &Basilisk, &Banshee, &Manticore, &Goblin
    };
    for (auto* pj : todos)
        if (pj->nombre == nombre) return pj;
    return nullptr;
}
