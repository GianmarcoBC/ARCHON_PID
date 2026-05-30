#include "SaveSystem.h"
#include "GameState.h"

// Reescribe el fichero entero con las partidas que quedan en memoria.
static void escribirPartida(std::ofstream& f, const PartidaGuardada& p)
{
    f << "---\n";
    f << "slot=" << p.slot << "\n";
    f << "nombre=" << p.nombre << "\n";
    f << "fecha=" << p.fecha << "\n";
    f << "modo=" << p.modo << "\n";
    f << "bando=" << p.bando << "\n";
    f << "turno=" << p.turno << "\n";
    f << "progreso=" << p.progreso << "\n";
    f << "equipo1=" << p.equipo1 << "\n";
    f << "equipo2=" << p.equipo2 << "\n";
    f << "dificultad=" << p.dificultad << "\n";
    f << "vsAI=" << (p.vsAI ? 1 : 0) << "\n";
    f << "equipoIA=" << p.equipoIA << "\n";
    if (!p.boardState.empty()) {
        // boardState contiene saltos de línea internos, los reemplazamos por `|`
        std::string encoded = p.boardState;
        for (char& c : encoded) if (c == '\n') c = '|';
        f << "boardState=" << encoded << "\n";
    }
}

void SaveSystem::reescribir(const GameState& gs)
{
    std::ofstream f("partidas.sav", std::ios::trunc);
    if (!f.is_open()) return;
    for (const auto& p : gs.partidas)
        escribirPartida(f, p);
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

    PartidaGuardada p;
    p.nombre = nombrePartida;
    p.fecha = fechaBuf;
    p.modo = (gs.modoActual == MODO_COMBATE ? "Combate" : "Juego Completo");
    p.bando = (gs.bandoSel == BANDO_LUZ ? "Luz" :
        gs.bandoSel == BANDO_OSCURIDAD ? "Oscuridad" : "Random");
    p.turno = 1;
    p.progreso = 0.0f;
    p.equipo1 = gs.equipoSel1;
    p.equipo2 = gs.equipoSel2;
    p.dificultad = (int)gs.dificultadSel;
    p.vsAI = (gs.opcionSelModoSel == 1);
    p.equipoIA = 1; // default: IA juega oscuridad

    escribirPartida(f, p);
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
            if (dentroDeBloque && !p.nombre.empty()) {
                p.valida = true;
                gs.partidas.push_back(p);
            }
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

        if (clave == "slot")       p.slot = std::stoi(valor);
        else if (clave == "nombre")     p.nombre = valor;
        else if (clave == "fecha")      p.fecha = valor;
        else if (clave == "modo")       p.modo = valor;
        else if (clave == "bando")      p.bando = valor;
        else if (clave == "turno")      p.turno = std::stoi(valor);
        else if (clave == "progreso")   p.progreso = std::stof(valor);
        else if (clave == "equipo1")    p.equipo1 = std::stoi(valor);
        else if (clave == "equipo2")    p.equipo2 = std::stoi(valor);
        else if (clave == "dificultad") p.dificultad = std::stoi(valor);
        else if (clave == "vsAI")       p.vsAI = (std::stoi(valor) != 0);
        else if (clave == "equipoIA")   p.equipoIA = std::stoi(valor);
        else if (clave == "boardState") {
            // Decodificar: los `|` representan saltos de línea internos
            p.boardState = valor;
            for (char& c : p.boardState) if (c == '|') c = '\n';
        }
    }
    // El último bloque no tiene "---" de cierre
    if (dentroDeBloque && !p.nombre.empty()) {
        p.valida = true;
        gs.partidas.push_back(p);
    }

    f.close();
}

void SaveSystem::actualizarPartida(GameState& gs, int idx)
{
    if (idx < 0 || idx >= (int)gs.partidas.size()) return;
    // Actualizar fecha
    time_t ahora = time(nullptr);
    char fechaBuf[32];
    struct tm tmInfo;
    localtime_s(&tmInfo, &ahora);
    strftime(fechaBuf, sizeof(fechaBuf), "%d/%m/%Y %H:%M", &tmInfo);
    gs.partidas[idx].fecha = fechaBuf;
    reescribir(gs);
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
    gs.opcionSelModoSel = p.vsAI ? 1 : 0;
    gs.boardStatePendiente = p.boardState;
    gs.partidaActualIdx = idx;
}

bool SaveSystem::GuardarCombate(const SaveData& d)
{
    // Cargar los slots actuales
    std::vector<SaveData> slots = CargarTodosCombate();

    // Reemplazar el slot si ya existe, si no añadir
    bool encontrado = false;
    for (auto& s : slots) {
        if (s.slot == d.slot) { s = d; encontrado = true; break; }
    }
    if (!encontrado) slots.push_back(d);

    // Reescribir el fichero completo
    FILE* f = fopen(SAVE_FILE, "w");
    if (!f) return false;
    for (const auto& s : slots) {
        fprintf(f, "---\n");
        fprintf(f, "slot=%d\n", s.slot);
        fprintf(f, "modoIA=%d\n", s.modoIA ? 1 : 0);
        fprintf(f, "dificultad=%d\n", s.dificultad);
        fprintf(f, "jugador1=%s\n", s.nombreP1.c_str());
        fprintf(f, "vida1=%g\n", s.vidaP1);
        fprintf(f, "posX1=%g\n", s.posP1x);
        fprintf(f, "posY1=%g\n", s.posP1y);
        fprintf(f, "jugador2=%s\n", s.nombreP2.c_str());
        fprintf(f, "vida2=%g\n", s.vidaP2);
        fprintf(f, "posX2=%g\n", s.posP2x);
        fprintf(f, "posY2=%g\n", s.posP2y);
    }
    fclose(f);
    return true;
}

SaveData SaveSystem::CargarCombate(int slot) {
    std::vector<SaveData> slots = CargarTodosCombate();
    for (auto& s : slots)
        if (s.slot == slot) return s;
    return SaveData{};
}

bool SaveSystem::ExisteGuardado(int slot) {
    // slot == -1: comprueba si hay algún slot ocupado
    // slot >= 0:  comprueba si ese slot concreto está ocupado
    std::vector<SaveData> slots = CargarTodosCombate();
    if (slot < 0) return !slots.empty();
    for (const auto& s : slots)
        if (s.slot == slot) return true;
    return false;
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

std::vector<SaveData> SaveSystem::CargarTodosCombate() {
    std::vector<SaveData> slots;
    FILE* f = fopen(SAVE_FILE, "r");
    if (!f) return slots;

    char linea[256];
    SaveData d{};
    bool dentroDeBloque = false;

    while (fgets(linea, sizeof(linea), f)) {
        if (linea[0] == '#' || linea[0] == '\n' || linea[0] == '\r') continue;
        if (strncmp(linea, "---", 3) == 0) {
            if (dentroDeBloque) {
                d.valida = (!d.nombreP1.empty() && !d.nombreP2.empty());
                if (d.valida) slots.push_back(d);
            }
            d = SaveData{};
            dentroDeBloque = true;
            continue;
        }
        if (!dentroDeBloque) continue;

        char clave[64]{}, valor[128]{};
        if (sscanf(linea, "%63[^=]=%127[^\n\r]", clave, valor) != 2) continue;

        if (strcmp(clave, "slot") == 0) d.slot = atoi(valor);
        else if (strcmp(clave, "modoIA") == 0) d.modoIA = (atoi(valor) != 0);
        else if (strcmp(clave, "dificultad") == 0) d.dificultad = atoi(valor);
        else if (strcmp(clave, "jugador1") == 0) d.nombreP1 = valor;
        else if (strcmp(clave, "vida1") == 0) d.vidaP1 = (float)atof(valor);
        else if (strcmp(clave, "posX1") == 0) d.posP1x = (float)atof(valor);
        else if (strcmp(clave, "posY1") == 0) d.posP1y = (float)atof(valor);
        else if (strcmp(clave, "jugador2") == 0) d.nombreP2 = valor;
        else if (strcmp(clave, "vida2") == 0) d.vidaP2 = (float)atof(valor);
        else if (strcmp(clave, "posX2") == 0) d.posP2x = (float)atof(valor);
        else if (strcmp(clave, "posY2") == 0) d.posP2y = (float)atof(valor);
    }
    if (dentroDeBloque) {
        d.valida = (!d.nombreP1.empty() && !d.nombreP2.empty());
        if (d.valida) slots.push_back(d);
    }    fclose(f);

    return slots;
}

void SaveSystem::BorrarCombate(int slot) {
    std::vector<SaveData> slots = CargarTodosCombate();
    slots.erase(std::remove_if(slots.begin(), slots.end(),
        [slot](const SaveData& s) { return s.slot == slot; }), slots.end());

    FILE* f = fopen(SAVE_FILE, "w");
    if (!f) return;
    for (const auto& s : slots) {
        fprintf(f, "---\n");
        fprintf(f, "slot=%d\n", s.slot);
        fprintf(f, "modoIA=%d\n", s.modoIA ? 1 : 0);
        fprintf(f, "dificultad=%d\n", s.dificultad);
        fprintf(f, "jugador1=%s\n", s.nombreP1.c_str());
        fprintf(f, "vida1=%g\n", s.vidaP1);
        fprintf(f, "posX1=%g\n", s.posP1x);
        fprintf(f, "posY1=%g\n", s.posP1y);
        fprintf(f, "jugador2=%s\n", s.nombreP2.c_str());
        fprintf(f, "vida2=%g\n", s.vidaP2);
        fprintf(f, "posX2=%g\n", s.posP2x);
        fprintf(f, "posY2=%g\n", s.posP2y);
    }
    fclose(f);
}

std::array<SaveData, 4> SaveSystem::ObtenerSlots()
{
    std::array<SaveData, 4> resultado{};
    for (int i = 0; i < 4; i++) resultado[i].slot = i;

    std::vector<SaveData> slots = CargarTodosCombate();
    for (const auto& s : slots)
        if (s.slot >= 0 && s.slot < 4) resultado[s.slot] = s;

    return resultado;
}

std::array<PartidaGuardada, 4> SaveSystem::ObtenerSlotsTablero()
{
    std::array<PartidaGuardada, 4> resultado{};
    for (int i = 0; i < 4; i++) resultado[i].slot = i;

    // Leer partidas.sav y mapear por slot
    std::ifstream f("partidas.sav");
    if (!f.is_open()) return resultado;

    std::string linea;
    PartidaGuardada p{};
    bool dentroDeBloque = false;

    auto guardarBloque = [&]() {
        if (dentroDeBloque && !p.nombre.empty() && p.slot >= 0 && p.slot < 4) {
            p.valida = true;
            resultado[p.slot] = p;
        }
    };

    while (std::getline(f, linea)) {
        if (linea == "---") {
            guardarBloque();
            p = PartidaGuardada{};
            dentroDeBloque = true;
            continue;
        }
        if (!dentroDeBloque) continue;
        auto eq = linea.find('=');
        if (eq == std::string::npos) continue;
        std::string clave = linea.substr(0, eq);
        std::string valor = linea.substr(eq + 1);

        if (clave == "slot")        p.slot = std::stoi(valor);
        else if (clave == "nombre") p.nombre = valor;
        else if (clave == "fecha")  p.fecha = valor;
        else if (clave == "modo")   p.modo = valor;
        else if (clave == "bando")  p.bando = valor;
        else if (clave == "turno")  p.turno = std::stoi(valor);
        else if (clave == "progreso")   p.progreso = std::stof(valor);
        else if (clave == "equipo1")    p.equipo1 = std::stoi(valor);
        else if (clave == "equipo2")    p.equipo2 = std::stoi(valor);
        else if (clave == "dificultad") p.dificultad = std::stoi(valor);
        else if (clave == "vsAI")       p.vsAI = (std::stoi(valor) != 0);
        else if (clave == "equipoIA")   p.equipoIA = std::stoi(valor);
        else if (clave == "boardState") {
            p.boardState = valor;
            for (char& c : p.boardState) if (c == '|') c = '\n';
        }
    }
    guardarBloque();
    f.close();
    return resultado;
}

void SaveSystem::GuardarSlotTablero(const PartidaGuardada& p)
{
    auto slots = ObtenerSlotsTablero();
    if (p.slot >= 0 && p.slot < 4) slots[p.slot] = p;

    std::ofstream f("partidas.sav", std::ios::trunc);
    if (!f.is_open()) return;
    for (const auto& s : slots)
        if (s.valida) escribirPartida(f, s);
    f.close();
}

void SaveSystem::BorrarSlotTablero(int slot)
{
    auto slots = ObtenerSlotsTablero();
    if (slot >= 0 && slot < 4) {
        slots[slot] = PartidaGuardada{};
        slots[slot].slot = slot;
    }
    std::ofstream f("partidas.sav", std::ios::trunc);
    if (!f.is_open()) return;
    for (const auto& s : slots)
        if (s.valida) escribirPartida(f, s);
    f.close();
}
