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
    std::array<SaveData, 5> slots{};
    int seccionActiva = 0; // 0=partidas menú, 1=slots combate
public:
    ~CargarPartidaScreen() override = default;
    void Draw(GameState& gs)        override;
    void HandleInput(GameState& gs) override;
    void HandleMouse(GameState& gs) override;
    void OnEnter(GameState& gs)     override;
};

// Overlay de pausa — aparece sin slash, desde arriba
class PausaScreen : public Screen {
public:
    ~PausaScreen() override = default;
    void Draw(GameState& gs)        override;
    void HandleInput(GameState& gs) override;
    void HandleMouse(GameState& gs) override;
    void Update(GameState& gs)      override;
    void OnEnter(GameState& gs)     override;
};