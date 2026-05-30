#pragma once
#include "Screen.h"
#include "SaveData.h"  

// Pantallas legacy que siguen funcionando sin herencia
class Screens {
public:
    static void menuPrincipal(GameState& gs);
    static void menuOpciones(GameState& gs);
    static void menuMusica(GameState& gs);
    static void enciclopedia(GameState& gs);
    static void espadaSlash(GameState& gs);
};

// --- Pantallas nuevas, todas heredan de Screen ---

// "¿Combate o Juego Completo?"
class ModoJuegoScreen : public Screen {
public:
    ~ModoJuegoScreen() override = default;
    void Draw(GameState& gs)        override;
    void HandleInput(GameState& gs) override;
    void HandleMouse(GameState& gs) override;
    void Update(GameState& gs)      override {}
    void OnEnter(GameState& gs)     override;
};

// "¿PvP o Contra IA?" — misma pantalla para ambos modos
class SeleccionModoScreen : public Screen {
public:
    ~SeleccionModoScreen() override = default;
    void Draw(GameState& gs)        override;
    void HandleInput(GameState& gs) override;
    void HandleMouse(GameState& gs) override;
    void OnEnter(GameState& gs)     override;
};

// Bando + Dificultad para Juego Completo con IA
class ConfigJuegoCompletoScreen : public Screen {
    void drawPanelBando(GameState& gs, float ox);
    void drawPanelDificultad(GameState& gs, float ox);
    std::string nombrePartida = "Partida Nueva";
    bool editandoNombre = false;
public:
    ~ConfigJuegoCompletoScreen() override = default;
    void Draw(GameState& gs)        override;
    void HandleInput(GameState& gs) override;
    void HandleMouse(GameState& gs) override;
    void OnEnter(GameState& gs)     override;
};

// Elegir equipo (Luz u Oscuridad) en Combate
class SeleccionEquipoScreen : public Screen {
    void drawTarjetaEquipo(GameState& gs, int idx,
        float cx, float cy,
        bool seleccionado, float t);
    void drawBarraStat(float x, float y, float w, int val, Color col);
public:
    ~SeleccionEquipoScreen() override = default;
    void Draw(GameState& gs)        override;
    void HandleInput(GameState& gs) override;
    void HandleMouse(GameState& gs) override;
    void OnEnter(GameState& gs)     override;
};

// Dificultad IA para Combate (después de elegir equipo)
class ConfigDificultadScreen : public Screen {
public:
    ~ConfigDificultadScreen() override = default;
    void Draw(GameState& gs)        override;
    void HandleInput(GameState& gs) override;
    void HandleMouse(GameState& gs) override;
    void OnEnter(GameState& gs)     override;
};

// Lista de partidas guardadas con carga y borrado
class CargarPartidaScreen : public Screen {
    void drawTarjetaPartida(const PartidaGuardada& p,
        float x, float y, float w,
        bool sel, float t);
    void drawTarjetaSlot(const SaveData& s,
        float x, float y, float w,
        bool sel, float t);
    std::array<PartidaGuardada, 4> slotsTablero{};
    std::array<SaveData, 4> slotsCombate{};
    int seccionActiva = 0; // 0=partidas tablero, 1=slots combate

    // Diálogo de acciones sobre un slot
    bool dialogoActivo = false;
    int  dialogoOpcion = 0;    // 0=CARGAR, 1=RENOMBRAR, 2=ELIMINAR
    bool editandoNombre = false;
    std::string nuevoNombre;

    void drawDialogo(GameState& gs, float t, float ox);
    void accionCargar(GameState& gs);
    void accionEliminar(GameState& gs);
public:
    ~CargarPartidaScreen() override = default;
    void Draw(GameState& gs)        override;
    void HandleInput(GameState& gs) override;
    void HandleMouse(GameState& gs) override;
    void OnEnter(GameState& gs)     override;
};

// Overlay de pausa — aparece sin slash, desde arriba
class PausaScreen : public Screen {
    int  submenu = 0;       // 0=menú principal, 1=eligiendo slot para guardar
    int  slotCursor = 0;    // cursor dentro del selector de slots
    bool esTablero = false; // true=guardando tablero, false=guardando combate
    void DrawSlotPicker(GameState& gs, float panX, float panY, float panW, float panH, float t);
public:
    ~PausaScreen() override = default;
    void Draw(GameState& gs)        override;
    void HandleInput(GameState& gs) override;
    void HandleMouse(GameState& gs) override;
    void Update(GameState& gs)      override;
    void OnEnter(GameState& gs)     override;
};