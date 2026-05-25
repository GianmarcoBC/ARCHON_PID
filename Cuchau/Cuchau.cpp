//  Cuchau.cpp — Punto de entrada principal de domagic
//
//  Flujo del programa:
//    1. Inicializar ventana, audio y mostrar pantalla de carga
//    2. Crear el menu de seleccion y el gestor de musica
//    3. Bucle principal con maquina de estados:
//       Menu       → seleccion de personajes (F5 carga partida guardada)
//       Combate    → logica + renderizado (F5 guardado rapido, F6 pausa)
//       Pausa      → menu con Continuar / Guardar / Cargar / Volver al Menu
//       CargandoSave → frame de transicion para reconstruir la arena desde el save
//    4. Al cerrar la ventana, liberar recursos

#include "raylib.h"
#include "Controlador_Combate.h"
#include "Menu_Combate.h"
#include "Rolitas.h"
#include "SaveManager.h"

// Estados del juego
enum class EstadoJuego
{
    Menu,          // Seleccion de personajes / modo
    Combate,       // Combate en curso
    Pausa,         // Pausa con opciones Guardar / Cargar / Continuar / Salir
    CargandoSave   // Frame de transicion: construye la arena desde el save
};

// Menu de pausa dibujado encima del combate congelado
struct MenuPausa
{
    enum Opcion { Continuar = 0, Guardar, Cargar, VolverMenu, NUM_OPCIONES };

    int   cursor = 0;
    bool  guardadoOK = false;
    float timerMsg = 0.0f;

    void Reset() { cursor = 0; guardadoOK = false; timerMsg = 0.0f; }

    // Devuelve el indice de la opcion pulsada, o -1 si no se pulso nada
    int Update(float dt)
    {
        if (timerMsg > 0.0f) timerMsg -= dt;

        if (IsKeyPressed(KEY_S) || IsKeyPressed(KEY_DOWN))
            cursor = (cursor + 1) % NUM_OPCIONES;
        if (IsKeyPressed(KEY_W) || IsKeyPressed(KEY_UP))
            cursor = (cursor - 1 + NUM_OPCIONES) % NUM_OPCIONES;

        if (IsKeyPressed(KEY_ENTER)) return cursor;
        if (IsKeyPressed(KEY_ESCAPE)) return Continuar;
        return -1;
    }

    // Dibuja el panel de pausa
    void Draw(bool hayGuardado)
    {
        int W = GetScreenWidth();
        int H = GetScreenHeight();

        // Fondo oscuro semitransparente
        DrawRectangle(0, 0, W, H, { 0, 0, 0, 160 });

        int panelW = 380, panelH = 310;
        int px = W / 2 - panelW / 2;
        int py = H / 2 - panelH / 2;

        DrawRectangle(px, py, panelW, panelH, { 20, 20, 30, 230 });
        DrawRectangleLines(px, py, panelW, panelH, WHITE);

        const char* titulo = "PAUSA";
        DrawText(titulo, W / 2 - MeasureText(titulo, 36) / 2, py + 18, 36, YELLOW);

        const char* etiquetas[NUM_OPCIONES] = {
            "Continuar",
            "Guardar Partida",
            hayGuardado ? "Cargar Partida" : "Cargar  (sin datos)",
            "Volver al Menu"
        };

        Color colores[NUM_OPCIONES] = {
            WHITE, SKYBLUE,
            hayGuardado ? LIME : DARKGRAY,
            ORANGE
        };

        for (int i = 0; i < NUM_OPCIONES; ++i) {
            int bY = py + 88 + i * 52;
            bool sel = (i == cursor);

            if (sel) DrawRectangle(px + 20, bY - 4, panelW - 40, 38, { 255,255,255,25 });
            DrawText(etiquetas[i], W / 2 - MeasureText(etiquetas[i], 24) / 2, bY, 24, sel ? YELLOW : colores[i]);
        }

        // Mensaje de confirmacion tras guardar
        if (timerMsg > 0.0f) {
            const char* msg = guardadoOK ? "Partida guardada!" : "Error al guardar";
            DrawText(msg,
                W / 2 - MeasureText(msg, 20) / 2,
                py + panelH + 12, 20,
                guardadoOK ? GREEN : RED);
        }

        DrawText("W/S = navegar    ENTER = confirmar    ESC = continuar",
            W / 2 - MeasureText("W/S = navegar    ENTER = confirmar    ESC = continuar", 14) / 2,
            H - 28, 14, DARKGRAY);
    }
};

int main()
{
    // Inicializar ventana a pantalla completa
    InitWindow(0, 0, "ARCHON 3D");
    SetTargetFPS(60);
    InitAudioDevice();

    // Frames de calentamiento
    for (int i = 0; i < 10; i++) {
        BeginDrawing();
        ClearBackground(BLACK);
        DrawText("Cargando...", GetScreenWidth() / 2 - 60, GetScreenHeight() / 2, 20, WHITE);
        EndDrawing();
    }

    // Crear sistemas principales
    Menu_Combate menu;
    ControladorCombate*    combate = nullptr;
    Rolitas   rolitas;
    MenuPausa pausa;

    EstadoJuego estado = EstadoJuego::Menu;

    bool modoIA = false;
    int  dificultad = 1;
    SaveData savePendiente;

    // ── Bucle principal del juego ────────────────────────────────────
    while (!WindowShouldClose()) {

        float dt = GetFrameTime();
        rolitas.Update();

        BeginDrawing();
        ClearBackground(BLACK);

        switch (estado)
        {

        // ESTADO: MENU PRINCIPAL
        case EstadoJuego::Menu:
        {
            if (menu.Update()) {
                modoIA = menu.EsModoIA();
                dificultad = menu.GetDificultad();

                if (modoIA)
                    combate = new ControladorCombate(menu.GetSelP1(), menu.GetSelP2(), true, dificultad);
                else
                    combate = new ControladorCombate(menu.GetSelP1(), menu.GetSelP2(), false, 0);

                pausa.Reset();
                estado = EstadoJuego::Combate;
            }

            menu.Draw();

            // Mostrar atajo F5 si hay partida guardada
            if (SaveManager::ExisteGuardado()) {
                const char* hint = "[ F5 ]  Continuar partida guardada";
                DrawText(hint,
                    GetScreenWidth() / 2 - MeasureText(hint, 16) / 2,
                    GetScreenHeight() - 50, 16, LIME);

                if (IsKeyPressed(KEY_F5)) {
                    savePendiente = SaveManager::Cargar();
                    if (savePendiente.valida)
                        estado = EstadoJuego::CargandoSave;
                }
            }
            break;
        }

        // ESTADO: CARGANDO SAVE (frame de transicion)
        case EstadoJuego::CargandoSave:
        {
            const Pj_info* pj1 = SaveManager::BuscarPjPorNombre(savePendiente.nombreP1);
            const Pj_info* pj2 = SaveManager::BuscarPjPorNombre(savePendiente.nombreP2);

            if (pj1 && pj2) {
                delete combate;

                if (savePendiente.modoIA)
                    combate = new ControladorCombate(*pj1, *pj2, true, savePendiente.dificultad);
                else
                    combate = new ControladorCombate(*pj1, *pj2, false, 0);

                combate->CargarEstado(savePendiente);

                modoIA     = savePendiente.modoIA;
                dificultad = savePendiente.dificultad;

                pausa.Reset();
                estado = EstadoJuego::Combate;
            }
            else {
                // Save corrupto: volver al menu
                estado = EstadoJuego::Menu;
            }

            savePendiente = SaveData{};
            break;
        }

        // ESTADO: COMBATE ACTIVO
        case EstadoJuego::Combate:
        {
            combate->Update();
            combate->Draw();

            rolitas.SetMusic(combate->GetP1Name(), combate->GetP2Name(), combate->IsGameOver());

            if (!combate->IsGameOver()) {
                // F6 = abrir pausa
                if (IsKeyPressed(KEY_F6)) {
                    pausa.Reset();
                    estado = EstadoJuego::Pausa;
                }

                // F5 = guardado rapido
                if (IsKeyPressed(KEY_F5)) {
                    bool ok = combate->GuardarEstado(modoIA, dificultad);
                    const char* msg = ok ? "Partida guardada!" : "Error al guardar";
                    DrawText(msg,
                        GetScreenWidth() / 2 - MeasureText(msg, 22) / 2,
                        36, 22, ok ? GREEN : RED);
                }
            }

            // Tras la victoria, ENTER destruye la arena y vuelve al menu
            if (combate->IsGameOver() && IsKeyPressed(KEY_ENTER)) {
                delete combate;
                combate = nullptr;
                menu.Reset();
                rolitas.ResetToMenu();
                estado = EstadoJuego::Menu;
            }
            break;
        }

        // ESTADO: PAUSA
        case EstadoJuego::Pausa:
        {
            // Dibujar el combate congelado de fondo
            combate->Draw();

            bool hayGuardado = SaveManager::ExisteGuardado();
            int accion = pausa.Update(dt);
            pausa.Draw(hayGuardado);

            if (accion == MenuPausa::Continuar) {
                estado = EstadoJuego::Combate;
            }
            else if (accion == MenuPausa::Guardar) {
                bool ok = combate->GuardarEstado(modoIA, dificultad);
                pausa.guardadoOK = ok;
                pausa.timerMsg = 2.0f;
            }
            else if (accion == MenuPausa::Cargar) {
                if (hayGuardado) {
                    savePendiente = SaveManager::Cargar();
                    if (savePendiente.valida)
                        estado = EstadoJuego::CargandoSave;
                }
            }
            else if (accion == MenuPausa::VolverMenu) {
                delete combate;
                combate = nullptr;
                menu.Reset();
                rolitas.ResetToMenu();
                estado = EstadoJuego::Menu;
            }
            break;
        }
        } // fin del switch

        EndDrawing();

    } // fin del while

    // ── Limpieza final ───────────────────────────────────────────────
    delete combate;
    CloseAudioDevice();
    CloseWindow();
    return 0;
}
