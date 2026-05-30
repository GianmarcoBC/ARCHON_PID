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
#include "src/Controlador_Tablero.h"
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
   /* Tablero tablero;
    tablero.inicializarTablero();
*/
    Controlador_Tablero ctablero(true,1, OSCURIDAD); //// El bool primero cambia si hay IA o no, tambien hay que cambiarlo mas abajo en logicacontrolador

    // Puntero al combate activo (nullptr cuando no hay combate en curso)
    ControladorCombate* combate = nullptr;

    bool atacanteEsIA = false; // Para determinar si el atacante es controlado por IA o por el jugador humano

    while (!WindowShouldClose())
    {
        // ========================= LOGICA =========================
        if (ctablero.cget_modoJuegoActual() == ModoJuego::GAME_OVER)
        {
            if (IsKeyPressed(KEY_ENTER)) break;  // sale del bucle y cierra el juego
        }
        else if (ctablero.cget_modoJuegoActual() == ModoJuego::COMBATE)
        {
            // Cuando el tablero detecta un ataque, creamos el ControladorCombate
            // usando los datos de combate (Pj_info) de ambas piezas enfrentadas
            if (combate == nullptr && ctablero.ccombatePendiente())
            {
                
                atacanteEsIA = (ctablero.getEquipoAtacante() == ctablero.getEquipoAI());

                Pj_info pjHumano = getCombatInfo(atacanteEsIA ? ctablero.getID_defensor() : ctablero.getID_atacante());
                Pj_info pjIA = getCombatInfo(atacanteEsIA ? ctablero.getID_atacante() : ctablero.getID_defensor());

                // vsIA=true: el defensor es controlado por IA, dificultad=1 (normal)
                combate = new ControladorCombate(pjHumano, pjIA, true, 1);
            }

            if (combate != nullptr)
            {
                // Actualizar logica del combate (movimiento, disparos, colisiones, IA)
                combate->Update();

                // Al terminar el combate, ENTER devuelve al tablero
                if (combate->IsGameOver() && IsKeyPressed(KEY_ENTER))
                {
                    bool ganaHumano = (combate->GetWinner() == 1);
                    // Si el humano es el atacante, ganaAtacante == ganaHumano
                    // Si el humano es el defensor, ganaAtacante == !ganaHumano
                    bool ganaAtacante = atacanteEsIA ? !ganaHumano : ganaHumano;
                    ctablero.cresolverCombate(ganaAtacante);
                    delete combate; combate = nullptr;
                }
            }
        }
        else
        {
            // Modo tablero: turnos, movimiento de piezas, hechizos, deteccion de victoria
           ctablero.Logica_controlador(true); //True si hay IA false si no la hay
        }

        // ========================= DIBUJO =========================
        BeginDrawing();
        ClearBackground(RAYWHITE);

        if (ctablero.cget_modoJuegoActual() == ModoJuego::COMBATE && combate != nullptr)
        {
            // Dibujar escena de combate 3D (arena, personajes, proyectiles, HUD)
            combate->Draw();
        }
        else
        {
            // Dibujar tablero 2D (casillas, piezas, indicadores)
            ctablero.draw();
        }

        EndDrawing();
    }

    // Limpieza de recursos
    delete combate;
    CloseAudioDevice();
    CloseWindow();
    return 0;
}
