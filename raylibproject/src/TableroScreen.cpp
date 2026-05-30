#include "TableroScreen.h"

void TableroScreen::limpiarCombate() {
    delete combate;
    combate = nullptr;
    if (rolitas) rolitas->Stop();
    rolitas.reset();
}

void TableroScreen::volverAlMenu(GameState& gs) {
    // Flush frames to avoid stale 3D state
    EndDrawing();
    BeginDrawing();
    EndDrawing();

    limpiarCombate();
    gs.combateActivo = nullptr;
    delete ctablero;
    ctablero = nullptr;
    gs.tableroActivo = nullptr;
    gs.partidaActualIdx = -1;
    estado = 0;

    gs.estadoAnterior = gs.estadoActual;
    gs.slashActivo = true;
    gs.siguienteEstado = MENU;
}

void TableroScreen::OnEnter(GameState& gs) {
    // Clean up any previous state
    limpiarCombate();
    gs.combateActivo = nullptr;
    delete ctablero;
    ctablero = nullptr;
    gs.tableroActivo = nullptr;
    estado = 0;

    // Determine game configuration from menu selections
    vsAI = (gs.opcionSelModoSel == 1);  // 0=PvP, 1=vs IA

    // Map difficulty: DIFI_FACIL=0, DIFI_NORMAL=1, DIFI_DIFICIL=2
    dificultad = (int)gs.dificultadSel;

    // Determine which team the AI controls
    if (!gs.boardStatePendiente.empty()) {
        // Cargando partida: usar el equipoIA guardado
        equipoIA = gs.partidas[gs.partidaActualIdx].equipoIA;
    } else if (gs.bandoSel == BANDO_LUZ) {
        equipoIA = OSCURIDAD;
    } else if (gs.bandoSel == BANDO_OSCURIDAD) {
        equipoIA = LUZ;
    } else {
        equipoIA = (GetRandomValue(0, 1) == 0) ? LUZ : OSCURIDAD;
    }

    // Create the board controller
    ctablero = new Controlador_Tablero(vsAI, dificultad, (equipo)equipoIA);
    gs.tableroActivo = ctablero;

    // Si hay un estado de tablero pendiente de cargar, aplicarlo
    if (!gs.boardStatePendiente.empty()) {
        ctablero->CargarEstado(gs.boardStatePendiente);
        gs.boardStatePendiente.clear();
    }

    // Actualizar el equipoIA en la partida guardada (importante para BANDO_RANDOM)
    if (gs.partidaActualIdx >= 0 && gs.partidaActualIdx < (int)gs.partidas.size()) {
        gs.partidas[gs.partidaActualIdx].equipoIA = equipoIA;
        gs.partidas[gs.partidaActualIdx].vsAI = vsAI;
    }
}

void TableroScreen::Update(GameState& gs) {
    if (!ctablero) return;

    ModoJuego modoTablero = ctablero->cget_modoJuegoActual();

    // --- GAME OVER ---
    if (modoTablero == ModoJuego::GAME_OVER) {
        if (IsKeyPressed(KEY_ENTER) || IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            volverAlMenu(gs);
        }
        return;
    }

    // --- COMBAT in progress ---
    if (modoTablero == ModoJuego::COMBATE) {
        if (combate == nullptr && ctablero->ccombatePendiente()) {
            // Determine who is the attacker (AI or human)
            atacanteEsIA = vsAI && (ctablero->getEquipoAtacante() == ctablero->getEquipoAI());

            Pj_info pjP1 = getCombatInfo(atacanteEsIA ? ctablero->getID_defensor() : ctablero->getID_atacante());
            Pj_info pjP2 = getCombatInfo(atacanteEsIA ? ctablero->getID_atacante() : ctablero->getID_defensor());


            // Create combat: in board mode, AI always controls P2 if vsAI
            if (vsAI) {
                combate = new ControladorCombate(pjP1, pjP2, true, dificultad);
            } else {
                combate = new ControladorCombate(pjP1, pjP2, false, 0);
            }

            gs.combateActivo = combate;

            // Restaurar estado del combate si se cargó desde guardado
            if (ctablero->tieneCombateState()) {
                SaveData sd;
                sd.vidaP1  = ctablero->combateVidaP1;
                sd.posP1x  = ctablero->combatePosP1x;
                sd.posP1y  = ctablero->combatePosP1y;
                sd.vidaP2  = ctablero->combateVidaP2;
                sd.posP2x  = ctablero->combatePosP2x;
                sd.posP2y  = ctablero->combatePosP2y;
                combate->CargarEstado(sd);
                ctablero->clearCombateState();
            }

            // Start combat music
            rolitas = std::make_unique<Rolitas>();
        }

        if (combate) {
            combate->Update();

            if (rolitas)
                rolitas->SetMusic(combate->GetP1Name().data(), combate->GetP2Name().data(), combate->IsGameOver());
            if (rolitas) rolitas->Update();

            // Combat finished - ENTER returns to board
            if (combate->IsGameOver() && (IsKeyPressed(KEY_ENTER) || IsMouseButtonPressed(MOUSE_BUTTON_LEFT))) {
                bool ganaHumano = (combate->GetWinner() == 1);
                // Map winner back to attacker/defender perspective
                bool ganaAtacante = atacanteEsIA ? !ganaHumano : ganaHumano;

                float vidaGanador = (combate->GetWinner() == 1)
                    ? combate->GetP1Vida()
                    : combate->GetP2Vida();

                ctablero->cresolverCombate(ganaAtacante, vidaGanador);

                limpiarCombate();
                gs.combateActivo = nullptr;
            }
        }
        return;
    }

    // --- BOARD LOGIC (normal play, spells, animations) ---
    ctablero->Logica_controlador(vsAI);
}

void TableroScreen::Draw(GameState& gs) {
    if (!ctablero) return;

    // Reset projection to default screen coordinates
    rlDrawRenderBatchActive();
    rlMatrixMode(RL_PROJECTION);
    rlLoadIdentity();
    rlOrtho(0, GetScreenWidth(), GetScreenHeight(), 0, -1, 1);
    rlMatrixMode(RL_MODELVIEW);
    rlLoadIdentity();

    ClearBackground(RAYWHITE);

    ModoJuego modoTablero = ctablero->cget_modoJuegoActual();

    if (modoTablero == ModoJuego::COMBATE && combate != nullptr) {
        combate->Draw();
    } else {
        ctablero->draw();
    }
}

void TableroScreen::HandleInput(GameState& gs) {
    // ESC/P handled by App::HandleInput (opens PAUSA)
}

void TableroScreen::HandleMouse(GameState& gs) {
    // Board handles its own mouse input via seleccionaCasilla()
}
