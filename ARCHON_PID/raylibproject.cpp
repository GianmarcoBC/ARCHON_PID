/*
 * raylibproject.cpp - Punto de entrada principal del juego Archon
 *
 * Gestiona el bucle principal del juego alternando entre dos estados:
 *   1. TABLERO: Juego de estrategia por turnos en un tablero 9x9
 *   2. COMBATE: Combate 3D en tiempo real cuando dos piezas se enfrentan
 *
 * El flujo es: tablero -> pieza ataca a otra -> se lanza combate 3D ->
 * resultado del combate determina quien sobrevive -> vuelve al tablero
 */

#include "raylib.h"
#include "src/Tablero.h"
#include "src/PjMapping.h"
#include "src/cuchau/Controlador_Combate.h"

int main()
{
    // Inicializar ventana en pantalla completa a la resolucion nativa del monitor
    InitWindow(1920, 1080, "Archon");   // Primero crear la ventana (GLFW se inicializa aquí)
    int screenW = GetMonitorWidth(0);
    int screenH = GetMonitorHeight(0);
    SetWindowSize(screenW, screenH);
    SetConfigFlags(FLAG_FULLSCREEN_MODE);
    InitAudioDevice();
    SetTargetFPS(60);

    // Crear e inicializar el tablero con todas las piezas en posicion inicial
    Tablero tablero;
    tablero.inicializarTablero();

    // Puntero al combate activo (nullptr cuando no hay combate en curso)
    ControladorCombate* combate = nullptr;

    while (!WindowShouldClose())
    {
        // ========================= LOGICA =========================
        if (tablero.get_modoJuegoActual() == ModoJuego::GAME_OVER)
        {
            if (IsKeyPressed(KEY_ENTER)) break;  // sale del bucle y cierra el juego
        }
        else if (tablero.get_modoJuegoActual() == ModoJuego::COMBATE)
        {
            // Cuando el tablero detecta un ataque, creamos el ControladorCombate
            // usando los datos de combate (Pj_info) de ambas piezas enfrentadas
            if (combate == nullptr && tablero.combatePendiente())
            {
                // Mapear tipo_pj del tablero -> Pj_info con stats/sprites de combate
                Pj_info pj1 = getCombatInfo(tablero.getAtacante()->get_ID());
                Pj_info pj2 = getCombatInfo(tablero.getDefensor()->get_ID());

                // vsIA=true: el defensor es controlado por IA, dificultad=1 (normal)
                combate = new ControladorCombate(pj1, pj2, true, 1);
            }

            if (combate != nullptr)
            {
                // Actualizar logica del combate (movimiento, disparos, colisiones, IA)
                combate->Update();

                // Al terminar el combate, ENTER devuelve al tablero
                if (combate->IsGameOver() && IsKeyPressed(KEY_ENTER))
                {
                    // Winner==1 significa que gano P1 (el atacante)
                    bool ganaAtacante = (combate->GetWinner() == 1);

                    // Resolver consecuencias en el tablero (mover/eliminar piezas)
                    tablero.resolverCombate(ganaAtacante);
                    delete combate;
                    combate = nullptr;
                }
            }
        }
        else
        {
            // Modo tablero: turnos, movimiento de piezas, hechizos, deteccion de victoria
            tablero.LogicaTablero();
        }

        // ========================= DIBUJO =========================
        BeginDrawing();
        ClearBackground(RAYWHITE);

        if (tablero.get_modoJuegoActual() == ModoJuego::COMBATE && combate != nullptr)
        {
            // Dibujar escena de combate 3D (arena, personajes, proyectiles, HUD)
            combate->Draw();
        }
        else
        {
            // Dibujar tablero 2D (casillas, piezas, indicadores)
            tablero.Draw();
        }

        EndDrawing();
    }

    // Limpieza de recursos
    delete combate;
    CloseAudioDevice();
    CloseWindow();
    return 0;
}
