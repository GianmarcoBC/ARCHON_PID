#include "Tablero.h"
#include "PjMapping.h"
#include <iostream>
#include <cmath>
#include <string>
#include <sstream>

// Constructor: configura dimensiones del tablero y carga recursos 3D
Tablero::Tablero() : magiaTablero() {
    casillasxlado = 9;
    tamanoCasilla = 64;
    alphaDiscard = LoadShader(0, "bin/Resources/alpha_discard.fs");

    // Cargar frames del fondo animado (26 frames, 30ms cada uno)
    for (int i = 1; i <= 26; i++) {
        std::string path = "bin/Resources/AAGraficos/Fondotablero/bac086c9e1388a6f418e9b5c01b1832b (1)_30ms_" + std::to_string(i) + ".png";
        fondoFrames.push_back(LoadTexture(path.c_str()));
    }

    // Cargar audio del tablero
    musicaInicio = LoadMusicStream("bin/Resources/AAAudio/Musica/MusciaTablero Inicio.mp3");
    musicaFin = LoadMusicStream("bin/Resources/AAAudio/Musica/MusciaTableroFin.mp3");
    sfxSelectPiece = LoadSound("bin/Resources/AAAudio/Efectos/SelectPiece.wav");
    SetSoundVolume(sfxSelectPiece, 3.0f);
    reproduciendoInicio = true;
    PlayMusicStream(musicaInicio);
}

Tablero::~Tablero() {
    UnloadShader(alphaDiscard);
    for (auto& tex : fondoFrames) UnloadTexture(tex);
    UnloadMusicStream(musicaInicio);
    UnloadMusicStream(musicaFin);
    UnloadSound(sfxSelectPiece);
}

/*
 * LogicaTablero - Dispatcher principal de la logica del juego
 *
 * Segun el modo de juego actual, ejecuta la logica correspondiente:
 * - NORMAL: seleccion de pieza, movimiento, deteccion de victoria
 * - HECHIZOS: espera input de teclado para elegir hechizo
 * - Cada hechizo tiene su propio modo (HEAL, TELEPORT, etc.)
 */
void Tablero::LogicaTablero() {
    if (modoJuegoactual == ModoJuego::GAME_OVER) return;
    // Actualizar musica del tablero: Inicio y Fin en bucle alternado
    if (reproduciendoInicio) {
        UpdateMusicStream(musicaInicio);
        if (!IsMusicStreamPlaying(musicaInicio)) {
            reproduciendoInicio = false;
            PlayMusicStream(musicaFin);
        }
    } else {
        UpdateMusicStream(musicaFin);
        if (!IsMusicStreamPlaying(musicaFin)) {
            reproduciendoInicio = true;
            PlayMusicStream(musicaInicio);
        }
    }

    if (get_modoJuegoActual() == ModoJuego::NORMAL) {
        if (personaje_usando_magia != nullptr) {
            personaje_usando_magia->set_seleccionado(false);
            personaje_usando_magia = nullptr;
        }

        moverPieza();                               // Gestionar seleccion y movimiento
        detectaGanador();                           // Comprobar si alguien ha ganado
    }

    // Actualizar animacion de movimiento en curso
    if (get_modoJuegoActual() == ModoJuego::ANIMANDO_MOVIMIENTO && piezaAnimando_ != nullptr) {
        float dt = GetFrameTime();
        if (piezaAnimando_->UpdateMovimiento(dt)) {
            if (combatePendiente_) {
                // La animación llegó al destino: ahora sí lanzar combate
                // Sacar al defensor de la cuadrícula para que no aparezca debajo del atacante
                cuadricula[defensor_->get_fila()][defensor_->get_columna()] = nullptr;
                // Colocar al atacante visualmente en el destino
                cuadricula[filaOrigenAtacante_][colOrigenAtacante_] = nullptr;
                cuadricula[filaDestinoAnim_][colDestinoAnim_] = piezaAnimando_;
                piezaAnimando_->set_fila_columna(filaDestinoAnim_, colDestinoAnim_);
                piezaAnimando_ = nullptr;
                modoJuegoactual = ModoJuego::COMBATE;
            }
            else {
                // Movimiento normal sin captura
                cambioPosicionPieza(piezaAnimando_, filaDestinoAnim_, colDestinoAnim_);
                piezaAnimando_ = nullptr;
                modoJuegoactual = ModoJuego::NORMAL;
                turno = turno == LUZ ? OSCURIDAD : LUZ;
                avanceCiclo();
            }
        }
    }

    // Delegar al sistema de magia segun el hechizo activo
    if (get_modoJuegoActual() == ModoJuego::HECHIZOS)   hechizos();
    if (get_modoJuegoActual() == ModoJuego::HEAL)       magiaTablero.Heal(personaje_usando_magia, *this);
    if (get_modoJuegoActual() == ModoJuego::TELEPORT)   magiaTablero.Teleport(personaje_usando_magia, *this);
    if (get_modoJuegoActual() == ModoJuego::EXCHANGE)   magiaTablero.Exchange(personaje_usando_magia, *this);
    if (get_modoJuegoActual() == ModoJuego::IMPRISON)   magiaTablero.Imprison(personaje_usando_magia, *this);
    if (get_modoJuegoActual() == ModoJuego::REVIVE)     magiaTablero.Revive(personaje_usando_magia, *this);
    if (get_modoJuegoActual() == ModoJuego::SUMMON)     magiaTablero.Summon(personaje_usando_magia, *this);

}

/*
 * inicializarTablero - Coloca todas las piezas en su posicion inicial
 *
 * Disposicion (vista desde arriba, columnas 0-8 de izquierda a derecha):
 *
 * Equipo LUZ (columnas 0-1):
 *   Col 0: Valkyrie, Golem, Unicorn, Djinni, MH, Phoenix, Unicorn, Golem, Valkyrie
 *   Col 1: Archer, Knight x7, Archer
 *
 * Equipo OSCURIDAD (columnas 7-8):
 *   Col 7: Manticore, Goblin x7, Manticore
 *   Col 8: Banshee, Troll, Basilisk, ShapeShifter, Platero, Dragon, Basilisk, Troll, Banshee
 *
 * Tambien inicializa los colores de las casillas con el patron de Archon:
 * casillas blancas, negras y cambiantes (las cambiantes oscilan con el ciclo 0-4)
 */
void Tablero::inicializarTablero() {
    // Limpiar cuadricula
    for (int i = 0; i < 9; i++) {
        for (int j = 0; j < 9; j++) {
            cuadricula[i][j] = nullptr;
            movimientosPosibles[i][j] = false;
        }
    }

    personaje_seleccionado = nullptr;
    fila_seleccionada = -1;
    columna_seleccionada = -1;

    // --- Equipo LUZ (columnas 0-1, lado izquierdo) ---
    cuadricula[0][0] = new PiezaTablero(pjboard::Valkyrie, 0, 0);
    cuadricula[8][0] = new PiezaTablero(pjboard::Valkyrie, 8, 0);
    cuadricula[1][0] = new PiezaTablero(pjboard::Golem, 1, 0);
    cuadricula[7][0] = new PiezaTablero(pjboard::Golem, 7, 0);
    cuadricula[2][0] = new PiezaTablero(pjboard::Unicorn, 2, 0);
    cuadricula[6][0] = new PiezaTablero(pjboard::Unicorn, 6, 0);
    cuadricula[3][0] = new PiezaTablero(pjboard::Djinni, 3, 0);
    cuadricula[4][0] = new PiezaTablero(pjboard::MH, 4, 0);          // Lider Luz en el centro
    cuadricula[5][0] = new PiezaTablero(pjboard::Phoenix, 5, 0);
    cuadricula[0][1] = new PiezaTablero(pjboard::Archer, 0, 1);
    cuadricula[8][1] = new PiezaTablero(pjboard::Archer, 8, 1);
    for (int fila = 1; fila < casillasxlado - 1; fila++) {
        cuadricula[fila][1] = new PiezaTablero(pjboard::Knight, fila, 1);  // Peones
    }

    // --- Equipo OSCURIDAD (columnas 7-8, lado derecho) ---
    cuadricula[0][8] = new PiezaTablero(pjboard::Banshee, 0, 8);
    cuadricula[8][8] = new PiezaTablero(pjboard::Banshee, 8, 8);
    cuadricula[1][8] = new PiezaTablero(pjboard::Troll, 1, 8);
    cuadricula[7][8] = new PiezaTablero(pjboard::Troll, 7, 8);
    cuadricula[2][8] = new PiezaTablero(pjboard::Basilisk, 2, 8);
    cuadricula[6][8] = new PiezaTablero(pjboard::Basilisk, 6, 8);
    cuadricula[3][8] = new PiezaTablero(pjboard::ShapeShifter, 3, 8);
    cuadricula[4][8] = new PiezaTablero(pjboard::Platero, 4, 8);     // Lider Oscuridad en el centro
    cuadricula[5][8] = new PiezaTablero(pjboard::Dragon, 5, 8);
    cuadricula[0][7] = new PiezaTablero(pjboard::Manticore, 0, 7);
    cuadricula[8][7] = new PiezaTablero(pjboard::Manticore, 8, 7);
    for (int fila = 1; fila < casillasxlado - 1; fila++) {
        cuadricula[fila][7] = new PiezaTablero(pjboard::Goblin, fila, 7); // Peones
    }

    // --- Patron de colores de casillas ---
    // El tablero de Archon tiene casillas blancas, negras y "cambiantes"
    // Las cambiantes oscilan entre claro y oscuro segun el ciclo del juego
    // El patron forma una cruz central de casillas cambiantes
    for (int fila = 0; fila < 9; fila++) {
        for (int columna = 0; columna < 9; columna++) {
            if (fila == 0 || fila == 8) {
                if (columna != 3 && columna != 4 && columna != 5) {
                    if (columna < 4) {
                        set_colorCasilla((fila + columna) % 2 == 0 ? ColorCasilla::NEGRO : ColorCasilla::BLANCO, fila, columna);
                    } else {
                        set_colorCasilla((fila + columna) % 2 == 0 ? ColorCasilla::BLANCO : ColorCasilla::NEGRO, fila, columna);
                    }
                } else set_colorCasilla(ColorCasilla::CAMBIANTE, fila, columna);
            }
            if (fila == 1 || fila == 7) {
                if (columna != 2 && columna != 4 && columna != 6) {
                    if (columna < 3 || columna == 5) {
                        set_colorCasilla((fila + columna) % 2 == 0 ? ColorCasilla::NEGRO : ColorCasilla::BLANCO, fila, columna);
                    } else {
                        set_colorCasilla((fila + columna) % 2 == 0 ? ColorCasilla::BLANCO : ColorCasilla::NEGRO, fila, columna);
                    }
                } else set_colorCasilla(ColorCasilla::CAMBIANTE, fila, columna);
            }
            if (fila == 2 || fila == 6) {
                if (columna != 1 && columna != 4 && columna != 7) {
                    if (columna == 0 || columna == 5 || columna == 6) {
                        set_colorCasilla((fila + columna) % 2 == 0 ? ColorCasilla::NEGRO : ColorCasilla::BLANCO, fila, columna);
                    } else {
                        set_colorCasilla((fila + columna) % 2 == 0 ? ColorCasilla::BLANCO : ColorCasilla::NEGRO, fila, columna);
                    }
                } else set_colorCasilla(ColorCasilla::CAMBIANTE, fila, columna);
            }
            if (fila == 3 || fila == 5) {
                if (columna != 0 && columna != 4 && columna != 8) {
                    if (columna > 4) {
                        set_colorCasilla((fila + columna) % 2 == 0 ? ColorCasilla::NEGRO : ColorCasilla::BLANCO, fila, columna);
                    } else {
                        set_colorCasilla((fila + columna) % 2 == 0 ? ColorCasilla::BLANCO : ColorCasilla::NEGRO, fila, columna);
                    }
                } else set_colorCasilla(ColorCasilla::CAMBIANTE, fila, columna);
            }
            if (fila == 4) {
                // Fila central: casi todas cambiantes excepto los extremos
                if (columna != 0 && columna != 8) set_colorCasilla(ColorCasilla::CAMBIANTE, fila, columna);
                else {
                    if (columna == 0) set_colorCasilla(ColorCasilla::BLANCO, fila, columna);
                    if (columna == 8) set_colorCasilla(ColorCasilla::NEGRO, fila, columna);
                }
            }
        }
    }
}

/*
 * Draw - Dibuja el tablero completo en 3D con vista en perspectiva
 *
 * Orden de dibujado:
 * 1. Fondo oscuro
 * 2. Escena 3D: base del tablero, casillas, puntos de poder, resaltados, piezas
 * 3. Overlays 2D: cementerio (Revive), indicador de turno
 */
void Tablero::Draw() {
    float dt = GetFrameTime();

    // 1. Fondo animado
    fondoTimer += dt;
    if (fondoTimer >= fondoFrameSpeed && !fondoFrames.empty()) {
        fondoTimer = 0.0f;
        fondoFrameActual = (fondoFrameActual + 1) % (int)fondoFrames.size();
    }
    if (!fondoFrames.empty()) {
        const Texture2D& bg = fondoFrames[fondoFrameActual];
        DrawTexturePro(bg,
            { 0, 0, (float)bg.width, (float)bg.height },
            { 0, 0, (float)GetScreenWidth(), (float)GetScreenHeight() },
            { 0, 0 }, 0.0f, { 100, 100, 100, 255 });
    }

    BeginMode3D(camera3D);

    float cubeH = 0.15f;
    float cubeSize = cellSize3D - cellGap;
    float boardExtent = 4.5f * cellSize3D;

    // Base del tablero (marco decorativo)
    DrawCube({ 0, -0.15f, 0 }, boardExtent * 2 + 0.8f, 0.2f, boardExtent * 2 + 0.8f, { 55, 35, 25, 255 });

    // 2. Dibujar casillas
    for (int fila = 0; fila < casillasxlado; fila++) {
        for (int columna = 0; columna < casillasxlado; columna++) {
            Color col = { 0, 0, 0, 255 };
            if (get_colorCasilla(fila, columna) == ColorCasilla::BLANCO) col = { 210, 210, 210, 255 };
            if (get_colorCasilla(fila, columna) == ColorCasilla::NEGRO)  col = { 35, 35, 35, 255 };
            if (get_colorCasilla(fila, columna) == ColorCasilla::CAMBIANTE) {
                if (Ciclo == 0)      col = { 230, 200, 245, 255 };
                else if (Ciclo == 1) col = { 180, 130, 210, 255 };
                else if (Ciclo == 2) col = { 130, 70, 175, 255 };
                else if (Ciclo == 3) col = { 80, 30, 120, 255 };
                else if (Ciclo == 4) col = { 40, 10, 70, 255 };
            }

            float x = (columna - 4.0f) * cellSize3D;
            float z = (fila - 4.0f) * cellSize3D;
            DrawCube({ x, 0.0f, z }, cubeSize, cubeH, cubeSize, col);

            // Puntos de poder: esfera naranja sobre la casilla
            bool puntoPoder = (fila == 0 && columna == 4) || (fila == 4 && columna == 0) ||
                              (fila == 4 && columna == 4) || (fila == 4 && columna == 8) ||
                              (fila == 8 && columna == 4);
            if (puntoPoder) {
                DrawSphere({ x, cubeH * 0.5f + 0.15f, z }, 0.35f, ORANGE);
            }
        }
    }

    // 3. Resaltado de movimientos posibles (cubos ligeramente elevados)
    DrawCasillas();

    // 4. Resaltar casilla del personaje seleccionado
    if (personaje_seleccionado != nullptr) {
        float sx = (personaje_seleccionado->get_columna() - 4.0f) * cellSize3D;
        float sz = (personaje_seleccionado->get_fila() - 4.0f) * cellSize3D;
        DrawCube({ sx, 0.02f, sz }, cubeSize, cubeH + 0.02f, cubeSize, { 255, 230, 0, 255 });
    }

    // 5. Sombras de las piezas en el suelo (antes de los billboards, igual que en combate)
    BeginShaderMode(alphaDiscard);
    for (int fila = 0; fila < casillasxlado; fila++) {
        for (int columna = 0; columna < casillasxlado; columna++) {
            if (cuadricula[fila][columna] != nullptr)
                cuadricula[fila][columna]->DrawShadow3D(cellSize3D);
        }
    }

    // 6. Dibujar piezas como billboards 3D
    for (int fila = 0; fila < casillasxlado; fila++) {
        for (int columna = 0; columna < casillasxlado; columna++) {
            if (cuadricula[fila][columna] != nullptr)
                cuadricula[fila][columna]->Draw3D(camera3D, cellSize3D);
        }
    }
    EndShaderMode();

    EndMode3D();

    // --- Overlays 2D (despues de EndMode3D) ---

    // 6. Cementerio (solo visible en modo Revive)
    if (modoJuegoactual == ModoJuego::REVIVE) {
        if (personaje_usando_magia->get_equipo() == LUZ) {
            for (int i = 0; i < (int)cementerio_Luz.size(); i++) {
                if (i < 9) cementerio_Luz[i]->set_fila_columna(i, 0);
                else cementerio_Luz[i]->set_fila_columna(i - 9, 1);
                cementerio_Luz[i]->DrawT(50, 36);
            }
        }
        if (personaje_usando_magia->get_equipo() == OSCURIDAD) {
            for (int i = 0; i < (int)cementerio_Oscuridad.size(); i++) {
                if (i < 9) cementerio_Oscuridad[i]->set_fila_columna(i, 0);
                else cementerio_Oscuridad[i]->set_fila_columna(i - 9, 1);
                cementerio_Oscuridad[i]->DrawT(850, 36);
            }
        }
    }

    // 7. Indicador de turno
    const char* turnoTxt = turno == LUZ ? "Turno: LUZ" : "Turno: OSCURIDAD";
    DrawText(turnoTxt, 10, GetScreenHeight() - 30, 20, turno == LUZ ? GOLD : PURPLE);

    // 8. Avanzar animacion de todas las piezas (despues de dibujar, para que sombra y sprite usen el mismo frame)
    for (int fila = 0; fila < casillasxlado; fila++)
        for (int columna = 0; columna < casillasxlado; columna++)
            if (cuadricula[fila][columna] != nullptr)
                cuadricula[fila][columna]->UpdateAnimacion(dt);

	// 9. Pantalla de Game Over
    if (modoJuegoactual == ModoJuego::GAME_OVER) {
        int w = GetScreenWidth(), h = GetScreenHeight();
        DrawRectangle(0, 0, w, h, { 0, 0, 0, 180 });   // fondo semitransparente
        DrawText(ganador.c_str(),
            w / 2 - MeasureText(ganador.c_str(), 60) / 2,
            h / 2 - 30, 60, GOLD);
        DrawText("Pulsa ENTER para salir",
            w / 2 - MeasureText("Pulsa ENTER para salir", 24) / 2,
            h / 2 + 50, 24, WHITE);
    }

    //10. Menú de hechizos
    if (modoJuegoactual == ModoJuego::HECHIZOS && personaje_usando_magia != nullptr) {
        int w = GetScreenWidth(), h = GetScreenHeight();
        tipo_pj lanzador = personaje_usando_magia->get_ID();

        // Panel semitransparente centrado
        int panelW = 420, panelH = 340;
        int panelX = w / 2 - panelW / 2;
        int panelY = h / 2 - panelH / 2;
        DrawRectangle(panelX, panelY, panelW, panelH, { 0, 0, 0, 210 });
        DrawRectangleLines(panelX, panelY, panelW, panelH, GOLD);

        // Título
        const char* titulo = "-- HECHIZOS --";
        DrawText(titulo, w / 2 - MeasureText(titulo, 26) / 2, panelY + 18, 26, GOLD);

        // Estructura: { tecla, nombre, índice en array }
        struct EntradaHechizo { const char* tecla; const char* nombre; int idx; };
        EntradaHechizo entradas[] = {
            { "[T]", "Teleport",  0 },
            { "[H]", "Heal",      1 },
            { "[S]", "Shift Time",2 },
            { "[E]", "Exchange",  3 },
            { "[U]", "Summon",    4 },
            { "[R]", "Revive",    5 },
            { "[I]", "Imprison",  6 },
        };

        int lineaY = panelY + 62;
        for (auto& e : entradas) {
            bool usado = magiaTablero.hechizoBloqueado(lanzador, e.idx);
            Color colorTecla = usado ? GRAY : GOLD;
            Color colorNombre = usado ? GRAY : WHITE;

            DrawText(e.tecla, panelX + 30, lineaY, 20, colorTecla);
            DrawText(e.nombre, panelX + 90, lineaY, 20, colorNombre);
            if (usado) DrawText("(usado)", panelX + 280, lineaY, 18, { 120, 120, 120, 255 });

            lineaY += 34;
        }

        // Pie
        const char* pie = "[A] Cancelar";
        DrawText(pie, w / 2 - MeasureText(pie, 18) / 2, panelY + panelH - 30, 18, { 180, 180, 180, 255 });
    }
}

/*
 * seleccionaCasilla - Convierte un click del raton en coordenadas de la cuadricula
 *
 * Lanza un rayo desde la posicion del raton a traves de la camara 3D y calcula
 * la interseccion con el plano Y=0 (superficie del tablero).
 * Convierte las coordenadas mundo a fila/columna de la cuadricula.
 */
void Tablero::seleccionaCasilla() {
    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        // Usar dimensiones de render para que coincida con la proyección de BeginMode3D
        Vector2 mouse = GetMousePosition();
        int rw = GetRenderWidth(), rh = GetRenderHeight();
        int sw = GetScreenWidth(),  sh = GetScreenHeight();
        if (sw > 0 && sh > 0) {
            mouse.x = mouse.x * (float)rw / (float)sw;
            mouse.y = mouse.y * (float)rh / (float)sh;
        }
        Ray ray = GetScreenToWorldRayEx(mouse, camera3D, rw, rh);

        // Interseccion con el plano Y=0 (superficie del tablero)
        if (ray.direction.y != 0.0f) {
            float t = -ray.position.y / ray.direction.y;
            if (t > 0.0f) {
                float hitX = ray.position.x + t * ray.direction.x;
                float hitZ = ray.position.z + t * ray.direction.z;

                // Convertir coordenadas mundo a indices de cuadricula
                int col = (int)floorf(hitX / cellSize3D + 4.5f);
                int row = (int)floorf(hitZ / cellSize3D + 4.5f);

                if (col >= 0 && col < 9 && row >= 0 && row < 9) {
                    columna_seleccionada = col;
                    fila_seleccionada = row;
                }
            }
        }
    }
}

// Mover una pieza de su posicion actual a una nueva casilla en la cuadricula
void Tablero::cambioPosicionPieza(PiezaTablero* personaje, int fil, int col) {
    cuadricula[personaje->get_fila()][personaje->get_columna()] = nullptr;
    personaje->set_fila_columna(fil, col);
    cuadricula[fil][col] = personaje;
}

// Resetear la seleccion de casilla (fila/columna a -1)
void Tablero::reset_seleccion() {
    fila_seleccionada = -1;
    columna_seleccionada = -1;
}

/*
 * moverPieza - Logica principal de seleccion y movimiento de piezas
 *
 * Flujo:
 * 1. Detectar click en una casilla (seleccionaCasilla)
 * 2. Si hay pieza propia en la casilla -> seleccionarla
 * 3. Si ya hay pieza seleccionada y se clickea casilla valida:
 *    a. Casilla con enemigo -> iniciar combate (ModoJuego::COMBATE)
 *    b. Casilla vacia -> mover pieza, cambiar turno, avanzar ciclo
 */
void Tablero::moverPieza() {
    seleccionaCasilla();

    // Si se clickea una pieza del equipo del turno actual -> seleccionarla
    if (fila_seleccionada != -1 && columna_seleccionada != -1 && cuadricula[fila_seleccionada][columna_seleccionada] != nullptr ) {
        if (turno == cuadricula[fila_seleccionada][columna_seleccionada]->get_equipo()) {
            reset_MovimientosPosibles();
            if (personaje_seleccionado != nullptr) personaje_seleccionado->set_seleccionado(false);
            personaje_seleccionado = cuadricula[fila_seleccionada][columna_seleccionada];
            personaje_seleccionado->set_seleccionado(true);
            PlaySound(sfxSelectPiece);
            casillasPosibles(personaje_seleccionado);   // Calcular movimientos validos
            reset_seleccion();
        }
    }

    // Comprobar si el mago seleccionado quiere usar magia (tecla M)
    iniciaEstadoHechizos();

    // Si hay pieza seleccionada y se ha clickeado una casilla destino
    if (fila_seleccionada != -1 && columna_seleccionada != -1 && personaje_seleccionado != nullptr && get_MovimientosPosibles(fila_seleccionada, columna_seleccionada) ==true) {
        if (cuadricula[fila_seleccionada][columna_seleccionada] != nullptr) {
            // La casilla destino tiene una pieza -> comprobar si es enemiga
            PiezaTablero* personajeAtacado = cuadricula[fila_seleccionada][columna_seleccionada];

            if (personajeAtacado->get_equipo() != personaje_seleccionado->get_equipo()) {
                combatePendiente_ = true;
                atacante_ = personaje_seleccionado;
                defensor_ = personajeAtacado;
                filaOrigenAtacante_ = personaje_seleccionado->get_fila();
                colOrigenAtacante_ = personaje_seleccionado->get_columna();
                filaDestinoAnim_ = fila_seleccionada;     
                colDestinoAnim_ = columna_seleccionada;    

                // Iniciar animación en lugar de saltar directo a COMBATE
                personaje_seleccionado->set_seleccionado(false);
                personaje_seleccionado->iniciarMovimiento(fila_seleccionada, columna_seleccionada, cellSize3D);
                piezaAnimando_ = personaje_seleccionado;
                modoJuegoactual = ModoJuego::ANIMANDO_MOVIMIENTO;

                personaje_seleccionado = nullptr;
                reset_seleccion();
                reset_MovimientosPosibles();
                return;
            }
        }
        else {
            // Casilla vacia: iniciar animacion de movimiento
            personaje_seleccionado->set_seleccionado(false);
            personaje_seleccionado->iniciarMovimiento(fila_seleccionada, columna_seleccionada, cellSize3D);
            piezaAnimando_ = personaje_seleccionado;
            filaDestinoAnim_ = fila_seleccionada;
            colDestinoAnim_ = columna_seleccionada;
            modoJuegoactual = ModoJuego::ANIMANDO_MOVIMIENTO;

            personaje_seleccionado = nullptr;
            reset_seleccion();
            reset_MovimientosPosibles();
            return;
        }

        // Limpiar seleccion y cambiar turno (solo para combate, el movimiento normal sale por return arriba)
        if (personaje_seleccionado != nullptr) personaje_seleccionado->set_seleccionado(false);
        personaje_seleccionado = nullptr;
        reset_seleccion();
        for (int i = 0; i < 9; i++)
            for (int j = 0; j < 9; j++)
                movimientosPosibles[i][j] = false;
        turno = turno == LUZ ? OSCURIDAD : LUZ;
        avanceCiclo();
    }
}

/*
 * resolverCombate - Aplica el resultado del combate 3D al tablero
 *
 * Si gana el atacante: el defensor va al cementerio, el atacante ocupa su casilla
 * Si gana el defensor: el atacante va al cementerio, su casilla original queda vacia
 *
 * En ambos casos se restaura la vida del perdedor (para que pueda ser revivido)
 * y se cambia de turno
 */
void Tablero::resolverCombate(bool ganaAtacante, float vidaGanador) {

    if (ganaAtacante) {
        // Defensor al cementerio
        if (defensor_->get_equipo() == LUZ) { defensor_->heal(); cementerio_Luz.push_back(defensor_); }
        else { defensor_->heal(); cementerio_Oscuridad.push_back(defensor_); }

        if (summonPendiente_) {
            // Elemental ganó: desaparece junto al defensor, sin ir al cementerio
            cuadricula[filaDestinoAnim_][colDestinoAnim_] = nullptr;
            delete elemental_;
            elemental_ = nullptr;
        } else {
            atacante_->set_vida(vidaGanador);  // vida real del ganador
        }
    }
    else {
        if (summonPendiente_) {
            // Elemental perdió: eliminarlo sin cementerio
            cuadricula[filaDestinoAnim_][colDestinoAnim_] = nullptr;
            delete elemental_;
            elemental_ = nullptr;
        }
        else {
            // Atacante normal perdió: al cementerio
            if (atacante_->get_equipo() == LUZ) { atacante_->heal(); cementerio_Luz.push_back(atacante_); }
            else { atacante_->heal(); cementerio_Oscuridad.push_back(atacante_); }
            cuadricula[filaDestinoAnim_][colDestinoAnim_] = nullptr;
        }
        // Defensor sobrevive: volver a su casilla original
        defensor_->set_vida(vidaGanador); 
        cuadricula[defensor_->get_fila()][defensor_->get_columna()] = defensor_;
    }

    summonPendiente_ = false;
    combatePendiente_ = false;
    atacante_ = nullptr;
    defensor_ = nullptr;
    modoJuegoactual = ModoJuego::NORMAL;
    turno = turno == LUZ ? OSCURIDAD : LUZ;
    avanceCiclo();
}

/*
 * casillasPosibles - Calcula las casillas validas de movimiento para una pieza
 *
 * Hay dos tipos de movimiento:
 * - Voladores (vuela=true): se mueven en un area cuadrada de lado 2*rango+1
 *   centrada en su posicion, saltando sobre otras piezas
 * - Terrestres (vuela=false): se mueven en cruz (arriba/abajo/izquierda/derecha)
 *   hasta su rango, pero se bloquean con piezas aliadas en el camino
 *
 * En ambos casos, pueden moverse a casillas vacias o con enemigos (para atacar)
 */
void Tablero::casillasPosibles(PiezaTablero* p) {
    if (p == nullptr) { reset_MovimientosPosibles(); return; }

    // Pieza aprisionada no puede moverse
    if (p->get_imprison()) return;

    // --- Movimiento de voladores: area cuadrada ---
    if (p->get_vuela()) {
        for (int fila = (p->get_fila() - (int)p->get_rangoTablero()); fila <= (p->get_fila() + (int)p->get_rangoTablero()); fila++) {
            for (int columna = (p->get_columna() - (int)p->get_rangoTablero()); columna <= (p->get_columna() + (int)p->get_rangoTablero()); columna++) {
                if (fila < 0 || columna < 0 || fila > 8 || columna > 8) continue; // Fuera del tablero
                if (cuadricula[fila][columna] == nullptr) set_MovimientosPosibles(true, fila, columna); // Casilla vacia
                else if (cuadricula[fila][columna]->get_equipo() != p->get_equipo()) set_MovimientosPosibles(true, fila, columna); // Enemigo
            }
        }
    }

    // --- Movimiento de terrestres: en cruz, bloqueado por aliados ---
    if (!p->get_vuela()) {
        // Hacia abajo (filas crecientes)
        for (int fila = p->get_fila(); fila <= (p->get_fila() + (int)p->get_rangoTablero()); fila++) {
            if (cuadricula[fila - 1][p->get_columna()] != nullptr && fila != p->get_fila()) {
                if (cuadricula[fila - 1][p->get_columna()]->get_equipo() != p->get_equipo()) break;
            }
            if (cuadricula[fila][p->get_columna()] == nullptr && fila >= 0 && p->get_columna() >= 0 && fila <= 8 && p->get_columna() <= 8)
                set_MovimientosPosibles(true, fila, p->get_columna());
            else if (cuadricula[fila][p->get_columna()] != nullptr && fila >= 0 && p->get_columna() >= 0 && fila <= 8 && p->get_columna() <= 8) {
                if (cuadricula[fila][p->get_columna()]->get_equipo() == p->get_equipo() && fila != p->get_fila()) break; // Bloqueado por aliado
                if (cuadricula[fila][p->get_columna()]->get_equipo() != p->get_equipo()) set_MovimientosPosibles(true, fila, p->get_columna()); // Enemigo
            }
            if (fila == 8) break;
        }

        // Hacia arriba (filas decrecientes)
        for (int fila = p->get_fila(); fila >= (p->get_fila() - (int)p->get_rangoTablero()); fila--) {
            if (cuadricula[fila + 1][p->get_columna()] != nullptr && fila != p->get_fila()) {
                if (cuadricula[fila + 1][p->get_columna()]->get_equipo() != p->get_equipo()) break;
            }
            if (cuadricula[fila][p->get_columna()] == nullptr && fila >= 0 && p->get_columna() >= 0 && fila <= 8 && p->get_columna() <= 8)
                set_MovimientosPosibles(true, fila, p->get_columna());
            else if (cuadricula[fila][p->get_columna()] != nullptr && fila >= 0 && p->get_columna() >= 0 && fila <= 8 && p->get_columna() <= 8) {
                if (cuadricula[fila][p->get_columna()]->get_equipo() == p->get_equipo() && fila != p->get_fila()) break;
                if (cuadricula[fila][p->get_columna()]->get_equipo() != p->get_equipo()) set_MovimientosPosibles(true, fila, p->get_columna());
            }
            if (fila == 0) break;
        }

        // Hacia la derecha (columnas crecientes)
        for (int columna = p->get_columna(); columna <= (p->get_columna() + (int)p->get_rangoTablero()); columna++) {
            if (cuadricula[p->get_fila()][columna - 1] != nullptr && columna != p->get_columna()) {
                if (cuadricula[p->get_fila()][columna - 1]->get_equipo() != p->get_equipo()) break;
            }
            if (cuadricula[p->get_fila()][columna] == nullptr && p->get_fila() >= 0 && columna >= 0)
                set_MovimientosPosibles(true, p->get_fila(), columna);
            else if (cuadricula[p->get_fila()][columna] != nullptr && p->get_fila() >= 0 && columna >= 0 && p->get_fila() <= 8 && columna <= 8) {
                if (cuadricula[p->get_fila()][columna]->get_equipo() == p->get_equipo() && columna != p->get_columna()) break;
                if (cuadricula[p->get_fila()][columna]->get_equipo() != p->get_equipo()) set_MovimientosPosibles(true, p->get_fila(), columna);
            }
            if (columna == 8) break;
        }

        // Hacia la izquierda (columnas decrecientes)
        for (int columna = p->get_columna(); columna >= (p->get_columna() - (int)p->get_rangoTablero()); columna--) {
            if (cuadricula[p->get_fila()][columna + 1] != nullptr && columna != p->get_columna()) {
                if (cuadricula[p->get_fila()][columna + 1]->get_equipo() != p->get_equipo()) break;
            }
            if (cuadricula[p->get_fila()][columna] == nullptr && p->get_fila() >= 0 && columna >= 0 && p->get_fila() <= 8 && columna <= 8)
                set_MovimientosPosibles(true, p->get_fila(), columna);
            else if (cuadricula[p->get_fila()][columna] != nullptr && p->get_fila() >= 0 && columna >= 0 && p->get_fila() <= 8 && columna <= 8) {
                if (cuadricula[p->get_fila()][columna]->get_equipo() == p->get_equipo() && columna != p->get_columna()) break;
                if (cuadricula[p->get_fila()][columna]->get_equipo() != p->get_equipo()) set_MovimientosPosibles(true, p->get_fila(), columna);
            }
            if (columna == 0) break;
        }
    }
}

// Dibujar resaltado azul/morado en 3D sobre las casillas donde la pieza puede moverse
void Tablero::DrawCasillas() {
    float cubeSize = cellSize3D - cellGap;
    float cubeH = 0.15f;
    for (int i = 0; i < 9; i++) {
        for (int j = 0; j < 9; j++) {
            if (movimientosPosibles[i][j]) {
                Color col = (i + j) % 2 == 0 ?
                    Color{ 0, 40, 160, 255 } : Color{ 90, 0, 140, 255 };
                float x = (j - 4.0f) * cellSize3D;
                float z = (i - 4.0f) * cellSize3D;
                DrawCube({ x, 0.02f, z }, cubeSize, cubeH + 0.02f, cubeSize, col);
            }
        }
    }
}

/*
 * detectaGanador - Comprueba las condiciones de victoria
 *
 * Se gana de 3 formas:
 * 1. Controlar los 5 puntos de poder (casillas especiales en cruz)
 * 2. Eliminar todas las piezas del enemigo
 * 3. Si al enemigo solo le queda 1 pieza y esta aprisionada (Imprison)
 */
void Tablero::detectaGanador() {
    bool ganaLuz = false, ganaOscuridad = false;
    int contadorLuz = 0, contadorOscuridad = 0;                    // Puntos de poder ocupados
    int contadorPersonajesLuz = 0, contadorPersonajesOscuridad = 0; // Piezas vivas

    for (int i = 0; i < 9; i++) {
        for (int j = 0; j < 9; j++) {
            // Los 5 puntos de poder forman una cruz: centro y 4 extremos del eje central
            bool puntoPoder = (i == 0 && j == 4) || (i == 4 && j == 0) || (i == 4 && j == 4) || (i == 4 && j == 8) || (i == 8 && j == 4);
            if (cuadricula[i][j] != nullptr) {
                if (cuadricula[i][j]->get_equipo() == LUZ) contadorPersonajesLuz++;
                if (cuadricula[i][j]->get_equipo() == OSCURIDAD) contadorPersonajesOscuridad++;
                if (puntoPoder) {
                    if (cuadricula[i][j]->get_equipo() == LUZ) contadorLuz++;
                    if (cuadricula[i][j]->get_equipo() == OSCURIDAD) contadorOscuridad++;
                }
            }
        }
    }

    // Condicion especial: si solo queda 1 pieza y esta aprisionada, pierde
    if (contadorPersonajesLuz == 1) {
        for (int i = 0; i < 9; i++)
            for (int j = 0; j < 9; j++)
                if (cuadricula[i][j] != nullptr && cuadricula[i][j]->get_equipo() == LUZ && cuadricula[i][j]->get_imprison())
                    ganaOscuridad = true;
    }
    if (contadorPersonajesOscuridad == 1) {
        for (int i = 0; i < 9; i++)
            for (int j = 0; j < 9; j++)
                if (cuadricula[i][j] != nullptr && cuadricula[i][j]->get_equipo() == OSCURIDAD && cuadricula[i][j]->get_imprison())
                    ganaLuz = true;
    }

    // Anunciar ganador
    if (ganaLuz || contadorLuz == 5 || contadorPersonajesOscuridad == 0) {
        modoJuegoactual = ModoJuego::GAME_OVER;
        ganador = "¡Gana LUZ!";
    }
    if (ganaOscuridad || contadorOscuridad == 5 || contadorPersonajesLuz == 0) {
        modoJuegoactual = ModoJuego::GAME_OVER;
        ganador = "¡Gana OSCURIDAD!";
    }
}

/*
 * iniciaEstadoHechizos - Detecta si el jugador quiere usar magia
 *
 * Solo MH (Luz) y Platero (Oscuridad) pueden lanzar hechizos.
 * Al pulsar M con uno de ellos seleccionado, se entra al modo HECHIZOS.
 */
void Tablero::iniciaEstadoHechizos() {
    if (personaje_seleccionado != nullptr) {
        if (personaje_seleccionado->get_ID() == tipo_pj::MH || personaje_seleccionado->get_ID() == tipo_pj::Platero) {
            if (IsKeyPressed(KEY_M)) {
                modoJuegoactual = ModoJuego::HECHIZOS;
                personaje_usando_magia = personaje_seleccionado;
                reset_MovimientosPosibles();
            }
        }
    }
}

/*
 * hechizos - Menu de seleccion de hechizo por teclado
 *
 * Teclas:
 *   A - Abortar (volver al modo normal)
 *   S - Shift Time (invierte la direccion del ciclo de casillas cambiantes)
 *   H - Heal (restaura vida de una pieza aliada)
 *   T - Teleport (mueve una pieza aliada a cualquier casilla vacia)
 *   E - Exchange (intercambia la posicion de dos piezas cualesquiera)
 *   I - Imprison (inmoviliza una pieza enemiga)
 *   R - Revive (resucita una pieza del cementerio)
 *
 * Cada hechizo solo se puede usar una vez por partida por equipo.
 */
void Tablero::hechizos() {
    if (IsKeyPressed(KEY_A)) { if (personaje_usando_magia) personaje_usando_magia->set_seleccionado(false); personaje_usando_magia = nullptr; modoJuegoactual = ModoJuego::NORMAL; }
    if (IsKeyPressed(KEY_S)) magiaTablero.Shift_Time(personaje_usando_magia, *this);
    if (IsKeyPressed(KEY_H)) { personaje_seleccionado = nullptr; modoJuegoactual = ModoJuego::HEAL; }
    if (IsKeyPressed(KEY_T)) { personaje_seleccionado = nullptr; modoJuegoactual = ModoJuego::TELEPORT; }
    if (IsKeyPressed(KEY_E)) { personaje_seleccionado = nullptr; modoJuegoactual = ModoJuego::EXCHANGE; }
    if (IsKeyPressed(KEY_I)) { personaje_seleccionado = nullptr; modoJuegoactual = ModoJuego::IMPRISON; }
    if (IsKeyPressed(KEY_R)) { personaje_seleccionado = nullptr; modoJuegoactual = ModoJuego::REVIVE; }
    if (IsKeyPressed(KEY_U)) { personaje_seleccionado = nullptr; modoJuegoactual = ModoJuego::SUMMON; }
}

/*
 * avanceCiclo - Avanza el ciclo de oscilacion de las casillas cambiantes
 *
 * El ciclo va de 0 a 4 y luego vuelve de 4 a 0 (oscilacion).
 * Cada turno avanza un paso. El color de las casillas cambiantes
 * depende del valor del ciclo (0=claro, 4=oscuro).
 *
 * Efecto adicional: cuando el ciclo llega a 0 se liberan las piezas
 * de Luz aprisionadas, y cuando llega a 4 se liberan las de Oscuridad.
 */
void Tablero::avanceCiclo() {
    bool antirrebote = false;
    if (avance == false) {
        Ciclo++;
        if (Ciclo == 4) avance = !avance;   // Al llegar a 4, empezar a bajar
        antirrebote = true;
    }
    if (avance == true && antirrebote == false) {
        Ciclo--;
        if (Ciclo == 0) avance = !avance;   // Al llegar a 0, empezar a subir
    }

    // Liberar piezas aprisionadas al llegar a los extremos del ciclo
    if (Ciclo == 0) {
        for (int fila = 0; fila < 9; fila++)
            for (int columna = 0; columna < 9; columna++)
                if (cuadricula[fila][columna] != nullptr && cuadricula[fila][columna]->get_equipo() == LUZ && cuadricula[fila][columna]->get_imprison())
                    cuadricula[fila][columna]->set_imprison(false);
    }
    if (Ciclo == 4) {
        for (int fila = 0; fila < 9; fila++)
            for (int columna = 0; columna < 9; columna++)
                if (cuadricula[fila][columna] != nullptr && cuadricula[fila][columna]->get_equipo() == OSCURIDAD && cuadricula[fila][columna]->get_imprison())
                    cuadricula[fila][columna]->set_imprison(false);
    }
}

// Limpiar todas las casillas marcadas como movimiento posible
void Tablero::reset_MovimientosPosibles() {
    for (int i = 0; i < 9; i++)
        for (int j = 0; j < 9; j++)
            movimientosPosibles[i][j] = false;
}

// Serializa el estado completo del tablero a una cadena de texto
std::string Tablero::serializarEstado() const {
    std::string s;
    // Turno
    s += "turnoActual=" + std::to_string(turno ? 1 : 0) + "\n";
    // Ciclo de casillas cambiantes
    s += "ciclo=" + std::to_string(Ciclo) + "\n";
    s += "avance=" + std::to_string(avance ? 1 : 0) + "\n";
    // Hechizos usados (7 por equipo)
    std::string hL, hO;
    for (int i = 0; i < 7; i++) { hL += (magiaTablero.getHechizo(false, i) ? '1' : '0'); }
    for (int i = 0; i < 7; i++) { hO += (magiaTablero.getHechizo(true, i) ? '1' : '0'); }
    s += "hechizosLuz=" + hL + "\n";
    s += "hechizosOscuridad=" + hO + "\n";
    // Cuadrícula 9x9: tipo:equipo:imprison o -1
    s += "grid=";
    for (int f = 0; f < 9; f++) {
        for (int c = 0; c < 9; c++) {
            if (f > 0 || c > 0) s += ",";
            if (cuadricula[f][c] == nullptr) {
                s += "-1";
            } else {
                PiezaTablero* p = cuadricula[f][c];
                s += std::to_string((int)p->get_ID()) + ":"
                   + std::to_string(p->get_equipo() ? 1 : 0) + ":"
                   + std::to_string(p->get_imprison() ? 1 : 0);
            }
        }
    }
    s += "\n";
    // Cementerios
    s += "cemLuz=";
    for (int i = 0; i < (int)cementerio_Luz.size(); i++) {
        if (i > 0) s += ",";
        s += std::to_string((int)cementerio_Luz[i]->get_ID());
    }
    s += "\n";
    s += "cemOsc=";
    for (int i = 0; i < (int)cementerio_Oscuridad.size(); i++) {
        if (i > 0) s += ",";
        s += std::to_string((int)cementerio_Oscuridad[i]->get_ID());
    }
    s += "\n";
    // Combate pendiente (si se guardó durante un combate)
    // El defensor NO está en la cuadrícula durante el combate (fue sacado en ANIMANDO_MOVIMIENTO),
    // así que guardamos su tipo y equipo aparte para poder recrearlo al cargar.
    if (combatePendiente_ && atacante_ && defensor_) {
        s += "combatePendiente=1\n";
        s += "atacanteFila=" + std::to_string(atacante_->get_fila()) + "\n";
        s += "atacanteCol=" + std::to_string(atacante_->get_columna()) + "\n";
        s += "defensorTipo=" + std::to_string((int)defensor_->get_ID()) + "\n";
        s += "defensorEquipo=" + std::to_string(defensor_->get_equipo() ? 1 : 0) + "\n";
        s += "defensorFila=" + std::to_string(defensor_->get_fila()) + "\n";
        s += "defensorCol=" + std::to_string(defensor_->get_columna()) + "\n";
        s += "origenAtaFila=" + std::to_string(filaOrigenAtacante_) + "\n";
        s += "origenAtaCol=" + std::to_string(colOrigenAtacante_) + "\n";
    }
    return s;
}

// Deserializa el estado del tablero desde una cadena de texto
void Tablero::deserializarEstado(const std::string& data) {
    if (data.empty()) return;

    // Limpiar el tablero actual
    for (int f = 0; f < 9; f++)
        for (int c = 0; c < 9; c++) {
            delete cuadricula[f][c];
            cuadricula[f][c] = nullptr;
        }
    for (auto* p : cementerio_Luz) delete p;
    for (auto* p : cementerio_Oscuridad) delete p;
    cementerio_Luz.clear();
    cementerio_Oscuridad.clear();
    personaje_seleccionado = nullptr;
    personaje_usando_magia = nullptr;
    personaje_auxiliar = nullptr;
    personaje_muerto_seleccionado = nullptr;
    fila_seleccionada = -1;
    columna_seleccionada = -1;
    modoJuegoactual = ModoJuego::NORMAL;
    reset_MovimientosPosibles();

    // Variables temporales para combate pendiente
    bool tieneCombate = false;
    int ataF = -1, ataC = -1, orgF = -1, orgC = -1;
    int defTipo = -1, defEquipo = -1, defF = -1, defC = -1;

    // Parsear key=value
    std::istringstream stream(data);
    std::string linea;
    while (std::getline(stream, linea)) {
        auto eq = linea.find('=');
        if (eq == std::string::npos) continue;
        std::string clave = linea.substr(0, eq);
        std::string valor = linea.substr(eq + 1);

        if (clave == "combatePendiente") {
            tieneCombate = (std::stoi(valor) != 0);
        } else if (clave == "atacanteFila") { ataF = std::stoi(valor);
        } else if (clave == "atacanteCol")  { ataC = std::stoi(valor);
        } else if (clave == "defensorTipo")  { defTipo = std::stoi(valor);
        } else if (clave == "defensorEquipo"){ defEquipo = std::stoi(valor);
        } else if (clave == "defensorFila") { defF = std::stoi(valor);
        } else if (clave == "defensorCol")  { defC = std::stoi(valor);
        } else if (clave == "origenAtaFila"){ orgF = std::stoi(valor);
        } else if (clave == "origenAtaCol") { orgC = std::stoi(valor);
        } else if (clave == "turnoActual") {
            turno = (std::stoi(valor) != 0);
        } else if (clave == "ciclo") {
            Ciclo = std::stoi(valor);
        } else if (clave == "avance") {
            avance = (std::stoi(valor) != 0);
        } else if (clave == "hechizosLuz") {
            for (int i = 0; i < 7 && i < (int)valor.size(); i++)
                magiaTablero.setHechizo(false, i, valor[i] == '1');
        } else if (clave == "hechizosOscuridad") {
            for (int i = 0; i < 7 && i < (int)valor.size(); i++)
                magiaTablero.setHechizo(true, i, valor[i] == '1');
        } else if (clave == "grid") {
            // Parsear 81 celdas separadas por coma
            std::istringstream gs(valor);
            std::string celda;
            int idx = 0;
            while (std::getline(gs, celda, ',') && idx < 81) {
                int f = idx / 9, c = idx % 9;
                if (celda == "-1") {
                    cuadricula[f][c] = nullptr;
                } else {
                    // formato: tipo:equipo:imprison
                    int tipo = 0, eq2 = 0, imp = 0;
                    sscanf(celda.c_str(), "%d:%d:%d", &tipo, &eq2, &imp);
                    PjBoard pb = getBoardPj((tipo_pj)tipo);
                    cuadricula[f][c] = new PiezaTablero(pb, f, c, eq2 ? OSCURIDAD : LUZ);
                    if (imp) cuadricula[f][c]->set_imprison(true);
                }
                idx++;
            }
        } else if (clave == "cemLuz") {
            if (!valor.empty()) {
                std::istringstream cs(valor);
                std::string tok;
                while (std::getline(cs, tok, ',')) {
                    PjBoard pb = getBoardPj((tipo_pj)std::stoi(tok));
                    cementerio_Luz.push_back(new PiezaTablero(pb, -1, -1, LUZ));
                }
            }
        } else if (clave == "cemOsc") {
            if (!valor.empty()) {
                std::istringstream cs(valor);
                std::string tok;
                while (std::getline(cs, tok, ',')) {
                    PjBoard pb = getBoardPj((tipo_pj)std::stoi(tok));
                    cementerio_Oscuridad.push_back(new PiezaTablero(pb, -1, -1, OSCURIDAD));
                }
            }
        }
    }

    // Restaurar combate pendiente si se guardó durante uno
    // El defensor no está en la cuadrícula (fue sacado antes del combate),
    // así que lo recreamos como pieza temporal a partir del tipo/equipo guardados.
    if (tieneCombate && ataF >= 0 && ataC >= 0 && defTipo >= 0 && defEquipo >= 0) {
        atacante_ = cuadricula[ataF][ataC];
        if (atacante_) {
            PjBoard pb = getBoardPj((tipo_pj)defTipo);
            defensor_ = new PiezaTablero(pb, defF, defC, defEquipo ? OSCURIDAD : LUZ);
            combatePendiente_ = true;
            filaOrigenAtacante_ = orgF;
            colOrigenAtacante_ = orgC;
            filaDestinoAnim_ = ataF;
            colDestinoAnim_ = ataC;
            modoJuegoactual = ModoJuego::COMBATE;
        }
    }
}
