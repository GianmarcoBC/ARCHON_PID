#include "CuchauScreen.h"

int CuchauCombateScreen::UpdatePausa(float dt)
{
    if (IsKeyPressed(KEY_S) || IsKeyPressed(KEY_DOWN))
        pausa_cursor = (pausa_cursor + 1) % 4;
    if (IsKeyPressed(KEY_W) || IsKeyPressed(KEY_UP))
        pausa_cursor = (pausa_cursor - 1 + 4) % 4;
    if (IsKeyPressed(KEY_ENTER)) return pausa_cursor;
    if (IsKeyPressed(KEY_ESCAPE)) return 0;
    return -1;
}

void CuchauCombateScreen::DrawPausa()
{
    int W = GetScreenWidth();
    int H = GetScreenHeight();
    DrawRectangle(0, 0, W, H, { 0, 0, 0, 160 });

    int panelW = 380, panelH = 310;
    int px = W / 2 - panelW / 2;
    int py = H / 2 - panelH / 2;

    DrawRectangle(px, py, panelW, panelH, { 20, 20, 30, 230 });
    DrawRectangleLines(px, py, panelW, panelH, WHITE);

    const char* titulo = "PAUSA";
    DrawText(titulo, W / 2 - MeasureText(titulo, 36) / 2, py + 18, 36, YELLOW);

    bool hayGuardado = SaveSystem::ExisteGuardado();
    const char* etiquetas[4] = {
        "Continuar", "Guardar Partida",
        hayGuardado ? "Cargar Partida" : "Cargar  (sin datos)",
        "Volver al Menu"
    };
    Color colores[4] = { WHITE, SKYBLUE, hayGuardado ? LIME : DARKGRAY, ORANGE };

    for (int i = 0; i < 4; ++i) {
        int bY = py + 88 + i * 52;
        bool sel = (i == pausa_cursor);
        if (sel) DrawRectangle(px + 20, bY - 4, panelW - 40, 38, { 255,255,255,25 });
        DrawText(etiquetas[i], W / 2 - MeasureText(etiquetas[i], 24) / 2, bY, 24, sel ? YELLOW : colores[i]);
    }

    if (pausa_timerMsg > 0.f) {
        const char* msg = pausa_guardadoOK ? "Partida guardada!" : "Error al guardar";
        DrawText(msg, W / 2 - MeasureText(msg, 20) / 2, py + panelH + 12, 20,
            pausa_guardadoOK ? GREEN : RED);
    }

    DrawText("W/S = navegar    ENTER = confirmar    ESC = continuar",
        W / 2 - MeasureText("W/S = navegar    ENTER = confirmar    ESC = continuar", 14) / 2,
        H - 28, 14, DARKGRAY);
}

void CuchauCombateScreen::OnEnter(GameState& gs)
{
    // Reset Cuchau state when entering
    delete combate;
    combate = nullptr;
    estado = 0; // Menu
    pausa_cursor = 0;
    pausa_guardadoOK = false;
    pausa_timerMsg = 0.f;
    // Skip the Cuchau mode selection — use the choice from SELECCION_MODO
    if (!menu) menu = std::make_unique<Menu_Combate>();
    else       menu->Reset();
    if (gs.modoActual == MODO_COMBATE) {
        bool ia = (gs.opcionSelModoSel == 1);
        menu->SetModo(ia);
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
        if (SaveSystem::ExisteGuardado()) {
            const char* hint = "[ F5 ]  Continuar partida guardada";
            DrawText(hint,
                GetScreenWidth() / 2 - MeasureText(hint, 16) / 2,
                GetScreenHeight() - 50, 16, LIME);
        }
        break;

    case 1: // Combate
        if (combate) {
            combate->Draw();
            if (!combate->IsGameOver()) {
                if (pausa_timerMsg > 0.f) {
                    const char* msg = "Partida guardada!";
                    DrawText(msg,
                        GetScreenWidth() / 2 - MeasureText(msg, 22) / 2,
                        36, 22, GREEN);
                }
            }
        }
        break;

    case 2: // Pausa
        if (combate) combate->Draw();
        DrawPausa();
        break;

    case 3: // CargandoSave (frame de transicion)
        break;
    }
}

void CuchauCombateScreen::Update(GameState& gs)
{
    float dt = GetFrameTime();
    if (rolitas) rolitas->Update();

    switch (estado) {
    case 0: // Menu (character selection)
    {
        if (!menu) {
            menu = std::make_unique<Menu_Combate>();
            rolitas = std::make_unique<Rolitas>();
        }

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

            pausa_cursor = 0;
            pausa_guardadoOK = false;
            pausa_timerMsg = 0.f;
            estado = 1;
        }

        // F5 carga partida guardada
        if (SaveSystem::ExisteGuardado() && IsKeyPressed(KEY_F5)) {
            SaveData save = SaveSystem::CargarCombate();
            if (save.valida) {
                savePendiente = save;
                estado = 3; // CargandoSave
            }
        }
        break;
    }

    case 1: // Combate
    {
        if (!combate) break;
        combate->Update();

        if (rolitas)
            rolitas->SetMusic(combate->GetP1Name().data(), combate->GetP2Name().data(), combate->IsGameOver());

        if (!combate->IsGameOver()) {
            if (IsKeyPressed(KEY_F6) || IsKeyPressed(KEY_ESCAPE)) {
                pausa_cursor = 0;
                pausa_guardadoOK = false;
                pausa_timerMsg = 0.f;
                estado = 2;
            }
            if (IsKeyPressed(KEY_F5)) {
                combate->GuardarEstado(modoIA, dificultad);
                pausa_timerMsg = 2.f;
            }
        }

        if (pausa_timerMsg > 0.f) pausa_timerMsg -= dt;

        // Victoria: ENTER vuelve al menu principal
        if (combate->IsGameOver() && IsKeyPressed(KEY_ENTER)) {
            volverARCHON(gs, MENU);
            return;
        }
        break;
    }

    case 2: // Pausa
    {
        if (pausa_timerMsg > 0.f) pausa_timerMsg -= dt;
        int accion = UpdatePausa(dt);
        if (accion == 0) { // Continuar
            estado = 1;
        }
        else if (accion == 1) { // Guardar
            if (combate) {
                pausa_guardadoOK = combate->GuardarEstado(modoIA, dificultad);
                pausa_timerMsg = 2.f;
            }
        }
        else if (accion == 2) { // Cargar
            if (SaveSystem::ExisteGuardado()) {
                SaveData save = SaveSystem::CargarCombate();
                if (save.valida) {
                    savePendiente = save;
                    estado = 3;
                }
            }
        }
        else if (accion == 3) { // Volver al menu principal
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
            if (savePendiente.modoIA)
                combate = new ControladorCombate(*pj1, *pj2, true, savePendiente.dificultad);
            else
                combate = new ControladorCombate(*pj1, *pj2, false, 0);
            combate->CargarEstado(savePendiente);
            modoIA = savePendiente.modoIA;
            dificultad = savePendiente.dificultad;
            pausa_cursor = 0;
            estado = 1;
        }
        else {
            estado = 0;
        }
        savePendiente = SaveData{};
        break;
    }
    }
}

void CuchauCombateScreen::volverARCHON(GameState& gs, Estado destino)
{
    delete combate;
    combate = nullptr;
    if (menu) menu->Reset();
    if (rolitas) rolitas->ResetToMenu();
    estado = 0;
    gs.estadoAnterior = gs.estadoActual;
    gs.slashActivo = true;
    gs.siguienteEstado = destino;
}
