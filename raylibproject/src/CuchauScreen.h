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

namespace Archon_PID {

    class CuchauCombateScreen : public Screen {
        std::unique_ptr<Menu_Combate> menu;
        std::unique_ptr<Rolitas> rolitas;
        ControladorCombate* combate = nullptr;
        int estado = 0; // 0=Menu, 1=Combate, 3=CargandoSave
        bool modoIA = false;
        int dificultad = 1;
        SaveData savePendiente;

    public:
        void OnEnter(GameState& gs) override;

        void Draw(GameState& gs) override;

        void HandleInput(GameState& gs) override {
            // During active combat (not menu, not game over): open global PAUSA
            if (estado == 1 && combate && !combate->IsGameOver()) {
                if (IsKeyPressed(KEY_ESCAPE) || IsKeyPressed(KEY_P)) {
                    gs.estadoAnterior = gs.estadoActual;
                    gs.estadoActual = PAUSA;
                    gs.opcionPausaSel = 0;
                    gs.pausaOffset = 600.f;
                    gs.guardadoOk = false;
                }
            }
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

}