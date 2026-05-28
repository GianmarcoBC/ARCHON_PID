#include "Tablero.h"
#include <iostream>

// Constructor: configura dimensiones del tablero
Tablero::Tablero() : magiaTablero() {
    casillasxlado = 9;
    tamanoCasilla = 64;
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
    if (get_modoJuegoActual() == ModoJuego::NORMAL) {
        personaje_usando_magia = nullptr;
       
        moverPieza();                               // Gestionar seleccion y movimiento
        detectaGanador();                           // Comprobar si alguien ha ganado
    }

    // Delegar al sistema de magia segun el hechizo activo
    if (get_modoJuegoActual() == ModoJuego::HECHIZOS)  hechizos();
    if (get_modoJuegoActual() == ModoJuego::HEAL)      magiaTablero.Heal(personaje_usando_magia, *this);
    if (get_modoJuegoActual() == ModoJuego::TELEPORT)   magiaTablero.Teleport(personaje_usando_magia, *this);
    if (get_modoJuegoActual() == ModoJuego::EXCHANGE)   magiaTablero.Exchange(personaje_usando_magia, *this);
    if (get_modoJuegoActual() == ModoJuego::IMPRISON)   magiaTablero.Imprison(personaje_usando_magia, *this);
    if (get_modoJuegoActual() == ModoJuego::REVIVE)     magiaTablero.Revive(personaje_usando_magia, *this);
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
 * Draw - Dibuja el tablero completo
 *
 * Orden de dibujado:
 * 1. Casillas de fondo (blancas, negras, cambiantes segun ciclo)
 * 2. Resaltado de casillas de movimiento posible (DrawCasillas)
 * 3. Cementerio si estamos en modo Revive
 * 4. Piezas sobre las casillas
 * 5. Indicador de turno en la esquina inferior
 */
void Tablero::Draw() {
    // 1. Dibujar casillas de fondo
    for (int fila = 0; fila < casillasxlado; fila++) {
        for (int columna = 0; columna < casillasxlado; columna++) {
            Color col;
            if (get_colorCasilla(fila, columna) == ColorCasilla::BLANCO) col = WHITE;
            if (get_colorCasilla(fila, columna) == ColorCasilla::NEGRO) col = BLACK;
            // Las casillas cambiantes oscilan entre morado claro y oscuro segun el Ciclo (0-4)
            if (get_colorCasilla(fila, columna) == ColorCasilla::CAMBIANTE) {
                if (Ciclo == 0) col = Color{ 230, 200, 245, 255 };       // Muy claro
                else if (Ciclo == 1) col = Color{ 180, 130, 210, 255 };
                else if (Ciclo == 2) col = Color{ 130, 70, 175, 255 };
                else if (Ciclo == 3) col = Color{ 80, 30, 120, 255 };
                else if (Ciclo == 4) col = Color{ 40, 10, 70, 255 };     // Muy oscuro
            }
            // Centrar el tablero en pantalla
            int posX = (int)(GetScreenWidth() / 2 - 4.5 * tamanoCasilla) + columna * tamanoCasilla;
            int posY = (int)(GetScreenHeight() / 2 - 4.5 * tamanoCasilla) + fila * tamanoCasilla;
            DrawRectangle(posX, posY, tamanoCasilla, tamanoCasilla, col);

            // Dibujamos puntos de poder
           // Lo hacemos atributo? 
            bool puntoPoder = (fila == 0 && columna == 4) || (fila == 4 && columna == 0) || (fila == 4 && columna == 4) || (fila == 4 && columna == 8) || (fila == 8 && columna == 4);

            if(puntoPoder){
            int centroX = (int)(GetScreenWidth() / 2 - 4 * tamanoCasilla) + columna * tamanoCasilla;
            int centroY = (int)(GetScreenHeight() / 2 - 4 * tamanoCasilla) + fila * tamanoCasilla;
            int radio = 15; // Un círculo pequeñito, como una ficha

            // Lo dibujamos
            DrawCircle(centroX, centroY, radio, ORANGE);
            }

        }
    }

    // 2. Resaltado de movimientos posibles
    DrawCasillas();


    // 3. Cementerio (solo visible en modo Revive)
    if (modoJuegoactual == ModoJuego::REVIVE) {
        // Dibujar piezas muertas del equipo del mago que lanza Revive
        if (personaje_usando_magia->get_equipo() == LUZ) {
            for (int i = 0; i < (int)cementerio_Luz.size(); i++) {
                if (i < 9) cementerio_Luz[i]->set_fila_columna(i, 0);
                else cementerio_Luz[i]->set_fila_columna(i - 9, 1);
                cementerio_Luz[i]->DrawT(50, 36);   // Lado izquierdo de la pantalla
            }
        }
        if (personaje_usando_magia->get_equipo() == OSCURIDAD) {
            for (int i = 0; i < (int)cementerio_Oscuridad.size(); i++) {
                if (i < 9) cementerio_Oscuridad[i]->set_fila_columna(i, 0);
                else cementerio_Oscuridad[i]->set_fila_columna(i - 9, 1);
                cementerio_Oscuridad[i]->DrawT(850, 36); // Lado derecho de la pantalla
            }
        }
    }

    // 4. Resaltar el personaje seleccionado
    if (personaje_seleccionado != nullptr) {
        int posX = (int)(GetScreenWidth() / 2 - 4.5 * tamanoCasilla) + personaje_seleccionado->get_columna() * tamanoCasilla;
        int posY = (int)(GetScreenHeight() / 2 - 4.5 * tamanoCasilla) + personaje_seleccionado->get_fila() * tamanoCasilla;
        DrawRectangle(posX, posY, tamanoCasilla, tamanoCasilla, YELLOW);

    }

    // 5. Dibujar todas las piezas en el tablero
    for (int fila = 0; fila < casillasxlado; fila++) {
        for (int columna = 0; columna < casillasxlado; columna++) {
            if (cuadricula[fila][columna] != nullptr)
                cuadricula[fila][columna]->DrawT((int)(GetScreenWidth() / 2 - 4.5 * tamanoCasilla), (int)(GetScreenHeight() / 2 - 4.5 * tamanoCasilla));
        }
    }

    // 6. Indicador de turno
    const char* turnoTxt = turno == LUZ ? "Turno: LUZ" : "Turno: OSCURIDAD";
    DrawText(turnoTxt, 10, GetScreenHeight() - 20, 16, turno == LUZ ? GOLD : PURPLE);

   


}

/*
 * seleccionaCasilla - Convierte un click del raton en coordenadas de la cuadricula
 *
 * Recorre todas las casillas y comprueba si el click cae dentro de alguna.
 * Actualiza fila_seleccionada y columna_seleccionada.
 */
void Tablero::seleccionaCasilla() {
    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        int mouseX = GetMouseX();
        int mouseY = GetMouseY();
        for (int fila = 0; fila < casillasxlado; fila++) {
            for (int columna = 0; columna < casillasxlado; columna++) {
                int posX = (int)(GetScreenWidth() / 2 - 4.5 * tamanoCasilla) + columna * tamanoCasilla;
                int posY = (int)(GetScreenHeight() / 2 - 4.5 * tamanoCasilla) + fila * tamanoCasilla;
                if (mouseX <= (posX + tamanoCasilla) && mouseX > posX) columna_seleccionada = columna;
                if (mouseY <= (posY + tamanoCasilla) && mouseY > posY) fila_seleccionada = fila;
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
    if (cuadricula[fila_seleccionada][columna_seleccionada] != nullptr && fila_seleccionada != -1 && columna_seleccionada != -1) {
        if (turno == cuadricula[fila_seleccionada][columna_seleccionada]->get_equipo()) {
            reset_MovimientosPosibles();
            personaje_seleccionado = cuadricula[fila_seleccionada][columna_seleccionada];
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
                // Pieza enemiga: guardar datos y lanzar combate 3D
                combatePendiente_ = true;
                atacante_ = personaje_seleccionado;
                defensor_ = personajeAtacado;
                filaOrigenAtacante_ = personaje_seleccionado->get_fila();
                colOrigenAtacante_ = personaje_seleccionado->get_columna();
                modoJuegoactual = ModoJuego::COMBATE;

                // Limpiar estado de seleccion
                personaje_seleccionado = nullptr;
                reset_seleccion();
                reset_MovimientosPosibles();
                return;
            }
        }
        else {
            // Casilla vacia: mover la pieza
            cambioPosicionPieza(personaje_seleccionado, fila_seleccionada, columna_seleccionada);
        }

        // Limpiar seleccion y cambiar turno
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
void Tablero::resolverCombate(bool ganaAtacante) {
    if (ganaAtacante) {
        // El defensor muere y va al cementerio de su equipo
        if (defensor_->get_equipo() == LUZ) {
            defensor_->heal();
            cementerio_Luz.push_back(defensor_);
        } else {
            defensor_->heal();
            cementerio_Oscuridad.push_back(defensor_);
        }
        // El atacante se mueve a la casilla del defensor
        cuadricula[defensor_->get_fila()][defensor_->get_columna()] = atacante_;
        cuadricula[filaOrigenAtacante_][colOrigenAtacante_] = nullptr;
        atacante_->set_fila_columna(defensor_->get_fila(), defensor_->get_columna());
    } else {
        // El atacante muere y va al cementerio de su equipo
        if (atacante_->get_equipo() == LUZ) {
            atacante_->heal();
            cementerio_Luz.push_back(atacante_);
        } else {
            atacante_->heal();
            cementerio_Oscuridad.push_back(atacante_);
        }
        // La casilla de origen del atacante queda vacia
        cuadricula[filaOrigenAtacante_][colOrigenAtacante_] = nullptr;
    }

    // Volver al modo normal
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
    if (personaje_seleccionado->get_vuela()) {
        for (int fila = (p->get_fila() - (int)p->get_rangoTablero()); fila <= (p->get_fila() + (int)p->get_rangoTablero()); fila++) {
            for (int columna = (p->get_columna() - (int)p->get_rangoTablero()); columna <= (p->get_columna() + (int)p->get_rangoTablero()); columna++) {
                if (fila < 0 || columna < 0 || fila > 8 || columna > 8) continue; // Fuera del tablero
                if (cuadricula[fila][columna] == nullptr) set_MovimientosPosibles(true, fila, columna); // Casilla vacia
                else if (cuadricula[fila][columna]->get_equipo() != p->get_equipo()) set_MovimientosPosibles(true, fila, columna); // Enemigo
            }
        }
    }

    // --- Movimiento de terrestres: en cruz, bloqueado por aliados ---
    if (!personaje_seleccionado->get_vuela()) {
        // Hacia abajo (filas crecientes)
        for (int fila = p->get_fila(); fila <= (p->get_fila() + (int)p->get_rangoTablero()); fila++) {
            if (cuadricula[fila - 1][personaje_seleccionado->get_columna()] != nullptr && fila != p->get_fila()) {
                if (cuadricula[fila - 1][personaje_seleccionado->get_columna()]->get_equipo() != p->get_equipo()) break;
            }
            if (cuadricula[fila][personaje_seleccionado->get_columna()] == nullptr && fila >= 0 && personaje_seleccionado->get_columna() >= 0 && fila <= 8 && personaje_seleccionado->get_columna() <= 8)
                set_MovimientosPosibles(true, fila, personaje_seleccionado->get_columna());
            else if (cuadricula[fila][personaje_seleccionado->get_columna()] != nullptr && fila >= 0 && personaje_seleccionado->get_columna() >= 0 && fila <= 8 && personaje_seleccionado->get_columna() <= 8) {
                if (cuadricula[fila][personaje_seleccionado->get_columna()]->get_equipo() == p->get_equipo() && fila != personaje_seleccionado->get_fila()) break; // Bloqueado por aliado
                if (cuadricula[fila][personaje_seleccionado->get_columna()]->get_equipo() != p->get_equipo()) set_MovimientosPosibles(true, fila, personaje_seleccionado->get_columna()); // Enemigo
            }
            if (fila == 8) break;
        }

        // Hacia arriba (filas decrecientes)
        for (int fila = p->get_fila(); fila >= (p->get_fila() - (int)p->get_rangoTablero()); fila--) {
            if (cuadricula[fila + 1][personaje_seleccionado->get_columna()] != nullptr && fila != p->get_fila()) {
                if (cuadricula[fila + 1][personaje_seleccionado->get_columna()]->get_equipo() != p->get_equipo()) break;
            }
            if (cuadricula[fila][personaje_seleccionado->get_columna()] == nullptr && fila >= 0 && personaje_seleccionado->get_columna() >= 0 && fila <= 8 && personaje_seleccionado->get_columna() <= 8)
                set_MovimientosPosibles(true, fila, personaje_seleccionado->get_columna());
            else if (cuadricula[fila][personaje_seleccionado->get_columna()] != nullptr && fila >= 0 && personaje_seleccionado->get_columna() >= 0 && fila <= 8 && personaje_seleccionado->get_columna() <= 8) {
                if (cuadricula[fila][personaje_seleccionado->get_columna()]->get_equipo() == p->get_equipo() && fila != personaje_seleccionado->get_fila()) break;
                if (cuadricula[fila][personaje_seleccionado->get_columna()]->get_equipo() != p->get_equipo()) set_MovimientosPosibles(true, fila, personaje_seleccionado->get_columna());
            }
            if (fila == 0) break;
        }

        // Hacia la derecha (columnas crecientes)
        for (int columna = p->get_columna(); columna <= (p->get_columna() + (int)p->get_rangoTablero()); columna++) {
            if (cuadricula[personaje_seleccionado->get_fila()][columna - 1] != nullptr && columna != p->get_columna()) {
                if (cuadricula[personaje_seleccionado->get_fila()][columna - 1]->get_equipo() != p->get_equipo()) break;
            }
            if (cuadricula[personaje_seleccionado->get_fila()][columna] == nullptr && personaje_seleccionado->get_fila() >= 0 && columna >= 0)
                set_MovimientosPosibles(true, personaje_seleccionado->get_fila(), columna);
            else if (cuadricula[personaje_seleccionado->get_fila()][columna] != nullptr && personaje_seleccionado->get_fila() >= 0 && columna >= 0 && personaje_seleccionado->get_fila() <= 8 && columna <= 8) {
                if (cuadricula[personaje_seleccionado->get_fila()][columna]->get_equipo() == p->get_equipo() && columna != personaje_seleccionado->get_columna()) break;
                if (cuadricula[personaje_seleccionado->get_fila()][columna]->get_equipo() != p->get_equipo()) set_MovimientosPosibles(true, personaje_seleccionado->get_fila(), columna);
            }
            if (columna == 8) break;
        }

        // Hacia la izquierda (columnas decrecientes)
        for (int columna = p->get_columna(); columna >= (p->get_columna() - (int)p->get_rangoTablero()); columna--) {
            if (cuadricula[personaje_seleccionado->get_fila()][columna + 1] != nullptr && columna != p->get_columna()) {
                if (cuadricula[personaje_seleccionado->get_fila()][columna + 1]->get_equipo() != p->get_equipo()) break;
            }
            if (cuadricula[personaje_seleccionado->get_fila()][columna] == nullptr && personaje_seleccionado->get_fila() >= 0 && columna >= 0 && personaje_seleccionado->get_fila() <= 8 && columna <= 8)
                set_MovimientosPosibles(true, personaje_seleccionado->get_fila(), columna);
            else if (cuadricula[personaje_seleccionado->get_fila()][columna] != nullptr && personaje_seleccionado->get_fila() >= 0 && columna >= 0 && personaje_seleccionado->get_fila() <= 8 && columna <= 8) {
                if (cuadricula[personaje_seleccionado->get_fila()][columna]->get_equipo() == p->get_equipo() && columna != personaje_seleccionado->get_columna()) break;
                if (cuadricula[personaje_seleccionado->get_fila()][columna]->get_equipo() != p->get_equipo()) set_MovimientosPosibles(true, personaje_seleccionado->get_fila(), columna);
            }
            if (columna == 0) break;
        }
    }
}

// Dibujar resaltado azul/morado en las casillas donde la pieza seleccionada puede moverse
void Tablero::DrawCasillas() {
    for (int i = 0; i < 9; i++) {
        for (int j = 0; j < 9; j++) {
            if (movimientosPosibles[i][j]) {
                Color col = (i + j) % 2 == 0 ? DARKBLUE : DARKPURPLE;
                int posX = (int)(GetScreenWidth() / 2 - 4.5 * tamanoCasilla) + j * tamanoCasilla;
                int posY = (int)(GetScreenHeight() / 2 - 4.5 * tamanoCasilla) + i * tamanoCasilla;
                DrawRectangle(posX, posY, tamanoCasilla, tamanoCasilla, col);
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
        std::cout << "Gana Luz" << std::endl;
    }
    if (ganaOscuridad || contadorOscuridad == 5 || contadorPersonajesLuz == 0) {
        std::cout << "Gana Oscuridad" << std::endl;
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
    if (IsKeyPressed(KEY_A)) { personaje_usando_magia = nullptr; modoJuegoactual = ModoJuego::NORMAL; }
    if (IsKeyPressed(KEY_S)) magiaTablero.Shift_Time(personaje_usando_magia, *this);
    if (IsKeyPressed(KEY_H)) { personaje_seleccionado = nullptr; modoJuegoactual = ModoJuego::HEAL; }
    if (IsKeyPressed(KEY_T)) { personaje_seleccionado = nullptr; modoJuegoactual = ModoJuego::TELEPORT; }
    if (IsKeyPressed(KEY_E)) { personaje_seleccionado = nullptr; modoJuegoactual = ModoJuego::EXCHANGE; }
    if (IsKeyPressed(KEY_I)) { personaje_seleccionado = nullptr; modoJuegoactual = ModoJuego::IMPRISON; }
    if (IsKeyPressed(KEY_R)) { personaje_seleccionado = nullptr; modoJuegoactual = ModoJuego::REVIVE; }
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
