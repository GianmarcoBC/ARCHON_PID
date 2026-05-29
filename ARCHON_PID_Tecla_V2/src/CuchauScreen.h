#pragma once
#include "Screen.h"
#include "Controlador_Combate.h"
#include "Menu_Combate.h"
#include "Rolitas.h"
#include "rlgl.h"

// ============================================================
// CuchauCombateScreen — Wraps the entire Cuchau combat system
// (menu + combat + pause) into a single ARCHON_PID Screen.
//
// When active, it resets the projection to standard screen coords
// so Cuchau can draw using GetScreenWidth/Height as it expects.
// ============================================================

class CuchauCombateScreen : public Screen {
    std::unique_ptr<Menu_Combate> menu;
    std::unique_ptr<Rolitas> rolitas;
    ControladorCombate* combate = nullptr;
    int estado = 0; // 0=Menu, 1=Combate, 2=Pausa, 3=CargandoSave
    bool modoIA = false;
    int dificultad = 1;
    SaveData savePendiente;
    int  pausa_submenu = 0;  // 0=menú principal, 1=eligiendo slot guardar, 2=eligiendo slot cargar
    int  pausa_slotCursor = 0;  // cursor dentro del submenú de slots

    // Pausa state
    int pausa_cursor = 0;
    bool pausa_guardadoOK = false;
    float pausa_timerMsg = 0.f;
    int slotCombate = 0;  // slot activo (0-4)

    // Returns: -1=nothing, 0=continuar, 1=guardar, 2=cargar, 3=volver menu
    int UpdatePausa(float dt);

    void DrawPausa();

public:
    void OnEnter(GameState& gs) override;

    void Draw(GameState& gs) override;

    void HandleInput(GameState& gs) override {
        // Input is handled in Update
    }

    void HandleMouse(GameState& gs) override {
        // Cuchau handles its own mouse
    }

    void Update(GameState& gs) override;

    // Transition back to an ARCHON screen, cleaning up Cuchau state
    void volverARCHON(GameState& gs, Estado destino);

    ~CuchauCombateScreen() override {
        delete combate;
    }
};
