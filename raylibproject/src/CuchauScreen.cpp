#include "CuchauScreen.h"

namespace Archon_PID {

    void CuchauCombateScreen::OnEnter(GameState& gs)
    {
        // Reiniciar música de Cuchau siempre al entrar
        rolitas = std::make_unique<Rolitas>();

        // Si venimos con un slot pendiente del menú, cargarlo directamente.
        // Hacemos el delete aquí, en OnEnter, donde no hay ningún frame de draw
        // activo — evita el crash por UnloadTexture/UnloadModel dentro de un frame.
        if (SaveSystem::pendiente.valida) {
            savePendiente = SaveSystem::pendiente;
            SaveSystem::pendiente = SaveData{};
            // Liberar combate anterior de forma limpia antes de crear el nuevo
            delete combate;
            combate = nullptr;
            gs.combateActivo = nullptr;
            estado = 3;
            return;
        }
        // Reset Cuchau state when entering
        delete combate;
        combate = nullptr;
        gs.combateActivo = nullptr;
        estado = 0; // Menu
        // Skip the Cuchau mode selection — use the choice from SELECCION_MODO
        if (!menu) menu = std::make_unique<Menu_Combate>();
        else       menu->Reset();
        if (gs.modoActual == MODO_COMBATE) {
            bool ia = (gs.opcionSelModoSel == 1);
            int bando1 = gs.equipoSel1;  // 0=claro, 1=oscuro
            int bando2 = gs.equipoSel2;
            int difi = ia ? (int)gs.dificultadSel : 1;
            menu->SetModo(ia, bando1, bando2, difi);
        }
    }

    void CuchauCombateScreen::Draw(GameState& gs)
    {
        // Reset projection to default screen coordinates for Cuchau
        rlDrawRenderBatchActive();
        rlMatrixMode(RL_PROJECTION);
        rlLoadIdentity();
        rlOrtho(0, GetScreenWidth(), GetScreenHeight(), 0, -1, 1);
        rlMatrixMode(RL_MODELVIEW);
        rlLoadIdentity();

        ClearBackground(BLACK);

        switch (estado) {
        case 0: // Menu
            if (menu) menu->Draw();
            break;

        case 1: // Combate
            if (combate) {
                combate->Draw();
            }
            break;

        case 3: // CargandoSave (frame de transicion)
            break;
        }
    }

    void CuchauCombateScreen::Update(GameState& gs)
    {
        if (rolitas) rolitas->Update();

        switch (estado) {
        case 0: // Menu (character selection)
        {
            if (!menu) {
                menu = std::make_unique<Menu_Combate>();
                rolitas = std::make_unique<Rolitas>();
            }

            if (menu) menu->HandleMouse();

            // ESC from character selection → back to ARCHON SELECCION_MODO
            if (menu->QuiereVolver()) {
                volverARCHON(gs, SELECCION_MODO);
                return;
            }

            if (menu->Update()) {
                modoIA = menu->EsModoIA();
                dificultad = menu->GetDificultad();

                delete combate;
                if (modoIA)
                    combate = new ControladorCombate(menu->GetSelP1(), menu->GetSelP2(), true, dificultad);
                else
                    combate = new ControladorCombate(menu->GetSelP1(), menu->GetSelP2(), false, 0);

                gs.combateActivo = combate;
                gs.combateModoIA = modoIA;
                gs.combateDificultad = dificultad;
                estado = 1;
            }
            break;
        }

        case 1: // Combate
        {
            if (!combate) break;
            combate->Update();

            if (rolitas)
                rolitas->SetMusic(combate->GetP1Name().data(), combate->GetP2Name().data(), combate->IsGameOver());

            // Victoria: ENTER o click vuelve al menu principal
            if (combate->IsGameOver() && (IsKeyPressed(KEY_ENTER) || IsMouseButtonPressed(MOUSE_BUTTON_LEFT))) {
                volverARCHON(gs, MENU);
                return;
            }
            break;
        }

        case 3: // CargandoSave
        {
            const Pj_info* pj1 = SaveSystem::BuscarPjPorNombre(savePendiente.nombreP1);
            const Pj_info* pj2 = SaveSystem::BuscarPjPorNombre(savePendiente.nombreP2);

            if (pj1 && pj2) {
                delete combate;
                combate = nullptr;

                if (savePendiente.modoIA)
                    combate = new ControladorCombate(*pj1, *pj2, true, savePendiente.dificultad);
                else
                    combate = new ControladorCombate(*pj1, *pj2, false, 0);

                combate->CargarEstado(savePendiente);
                modoIA = savePendiente.modoIA;
                dificultad = savePendiente.dificultad;
                gs.combateActivo = combate;
                gs.combateModoIA = modoIA;
                gs.combateDificultad = dificultad;
                estado = 1;
            }
            else {
                delete combate;
                combate = nullptr;
                gs.combateActivo = nullptr;
                estado = 0;
            }
            savePendiente = SaveData{};
            break;
        }
        }
    }

    void CuchauCombateScreen::volverARCHON(GameState& gs, Estado destino)
    {
        EndDrawing();       // cierra el frame 3D limpiamente
        BeginDrawing();     // abre uno nuevo en blanco
        EndDrawing();       // y lo cierra


        delete combate;
        combate = nullptr;
        gs.combateActivo = nullptr;
        if (menu) menu->Reset();
        if (rolitas) rolitas->Stop();
        estado = 0;
        gs.estadoAnterior = gs.estadoActual;
        gs.slashActivo = true;
        gs.siguienteEstado = destino;
    }

}