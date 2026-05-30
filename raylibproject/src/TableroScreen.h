#pragma once
#include "Screen.h"
#include "Controlador_Tablero.h"
#include "PjMapping.h"
#include "cuchau/Controlador_Combate.h"
#include "cuchau/Rolitas.h"
#include "rlgl.h"

namespace Archon_PID {

    // ============================================================
    // TableroScreen — Wraps the board game (Controlador_Tablero)
    // and its combat transitions into the ARCHON Screen system.
    //
    // Manages:
    //   - Board logic and rendering
    //   - Transition to 3D combat when pieces clash
    //   - Return to board after combat resolves
    //   - Game Over detection and return to menu
    // ============================================================

    class TableroScreen : public Screen {
        Controlador_Tablero* ctablero = nullptr;
        ControladorCombate* combate = nullptr;
        std::unique_ptr<Rolitas> rolitas;

        bool vsAI = true;
        int  dificultad = 1;
        int  equipoIA = 1;   // 0=LUZ, 1=OSCURIDAD (which team AI plays)

        bool atacanteEsIA = false;

        // 0=Tablero, 1=Combate, 2=GameOver
        int estado = 0;

        void limpiarCombate();
        void volverAlMenu(GameState& gs);

    public:
        void OnEnter(GameState& gs) override;
        void Draw(GameState& gs) override;
        void HandleInput(GameState& gs) override;
        void HandleMouse(GameState& gs) override;
        void Update(GameState& gs) override;

        ~TableroScreen() override {
            delete combate;
            delete ctablero;
            // Note: tableroActivo in GameState will be stale, but Screen destruction
            // only happens at app shutdown so this is fine.
        }
    };

} // namespace Archon_PID