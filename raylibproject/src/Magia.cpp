#include "Magia.h"
#include "Tablero.h"
#include <iostream>

void Magia::Shift_Time(PiezaTablero* personaje, Tablero &t) {
    if ((personaje->get_ID() == tipo_pj::MH && hechizosLuz[2] == true) || (personaje->get_ID() == tipo_pj::Platero && hechizosOscuridad[2] == true)) {
        std::cout << "El hechizo Shift ya se ha utilizado" << std::endl;
        t.modoJuegoactual = ModoJuego::HECHIZOS;
    }
    if ((personaje->get_ID() == tipo_pj::MH && hechizosLuz[2] == false) || (personaje->get_ID() == tipo_pj::Platero && hechizosOscuridad[2] == false)) {
        if (t.Ciclo != 0 && t.Ciclo != 4) t.avance = !t.avance;
        t.turno = t.turno == LUZ ? OSCURIDAD : LUZ;
        t.personaje_seleccionado = nullptr;
        personaje->set_seleccionado(false);
        t.personaje_usando_magia = nullptr;
        if (personaje->get_ID() == tipo_pj::MH) hechizosLuz[2] = true;
        if (personaje->get_ID() == tipo_pj::Platero) hechizosOscuridad[2] = true;
        t.modoJuegoactual = ModoJuego::NORMAL;
    }
}

void Magia::Teleport(PiezaTablero* personaje, Tablero& t) {
    if ((personaje->get_ID() == tipo_pj::MH && hechizosLuz[0] == true) || (personaje->get_ID() == tipo_pj::Platero && hechizosOscuridad[0] == true)) {
        std::cout << "El hechizo Teleport ya se ha utilizado" << std::endl;
        t.modoJuegoactual = ModoJuego::HECHIZOS;
    }

    t.moverPieza();

    if (t.personaje_seleccionado == nullptr) t.reset_MovimientosPosibles();

    if (personaje->get_ID() == tipo_pj::MH && t.turno == OSCURIDAD) {
        t.personaje_seleccionado = nullptr;
        hechizosLuz[0] = true;
        t.modoJuegoactual = ModoJuego::NORMAL;
    }
    if (personaje->get_ID() == tipo_pj::Platero && t.turno == LUZ) {
        t.personaje_seleccionado = nullptr;
        hechizosOscuridad[0] = true;
        t.modoJuegoactual = ModoJuego::NORMAL;
    }

    if (personaje->get_ID() == tipo_pj::MH && hechizosLuz[0] == false) {
        if (t.personaje_seleccionado != nullptr) {
            if (t.personaje_seleccionado->get_equipo() == LUZ) {
                for (int i = 0; i < 9; i++)
                    for (int j = 0; j < 9; j++)
                        if (t.cuadricula[i][j] == nullptr) t.movimientosPosibles[i][j] = true;
            }
        }
    }

    if (personaje->get_ID() == tipo_pj::Platero && hechizosOscuridad[0] == false) {
        if (t.personaje_seleccionado != nullptr) {
            if (t.personaje_seleccionado->get_equipo() == OSCURIDAD) {
                for (int i = 0; i < 9; i++)
                    for (int j = 0; j < 9; j++)
                        if (t.cuadricula[i][j] == nullptr) t.movimientosPosibles[i][j] = true;
            }
        }
    }
}

void Magia::Heal(PiezaTablero* personaje, Tablero& t) {
    if ((personaje->get_ID() == tipo_pj::MH && hechizosLuz[1] == true) || (personaje->get_ID() == tipo_pj::Platero && hechizosOscuridad[1] == true)) {
        std::cout << "El hechizo Heal ya se ha utilizado" << std::endl;
        t.modoJuegoactual = ModoJuego::HECHIZOS;
    }

    t.seleccionaCasilla();

    if (t.fila_seleccionada != -1 && t.columna_seleccionada != -1 && t.cuadricula[t.fila_seleccionada][t.columna_seleccionada] != nullptr) {
        t.personaje_seleccionado = t.cuadricula[t.fila_seleccionada][t.columna_seleccionada];
        t.reset_seleccion();
    }

    if ((personaje->get_ID() == tipo_pj::MH && hechizosLuz[1] == false)) {
        if (t.personaje_seleccionado != nullptr) {
            if (t.personaje_seleccionado->get_equipo() == LUZ) {
                t.personaje_seleccionado->heal();
                t.turno = t.turno == LUZ ? OSCURIDAD : LUZ;
                t.personaje_seleccionado = nullptr;
                hechizosLuz[1] = true;
                t.modoJuegoactual = ModoJuego::NORMAL;
            }
        }
    }

    if ((personaje->get_ID() == tipo_pj::Platero && hechizosOscuridad[1] == false)) {
        if (t.personaje_seleccionado != nullptr) {
            if (t.personaje_seleccionado->get_equipo() == OSCURIDAD) {
                t.personaje_seleccionado->heal();
                t.turno = t.turno == LUZ ? OSCURIDAD : LUZ;
                t.personaje_seleccionado = nullptr;
                hechizosOscuridad[1] = true;
                t.modoJuegoactual = ModoJuego::NORMAL;
            }
        }
    }
}

void Magia::Exchange(PiezaTablero* personaje, Tablero& t) {
    if ((personaje->get_ID() == tipo_pj::MH && hechizosLuz[3] == true) || (personaje->get_ID() == tipo_pj::Platero && hechizosOscuridad[3] == true)) {
        std::cout << "El hechizo Exchange ya se ha utilizado" << std::endl;
        t.modoJuegoactual = ModoJuego::HECHIZOS;
    }

    t.seleccionaCasilla();

    if (t.fila_seleccionada != -1 && t.columna_seleccionada != -1 && t.cuadricula[t.fila_seleccionada][t.columna_seleccionada] != nullptr && t.personaje_auxiliar == nullptr) {
        t.personaje_auxiliar = t.cuadricula[t.fila_seleccionada][t.columna_seleccionada];
        t.reset_seleccion();
    }
    if (t.fila_seleccionada != -1 && t.columna_seleccionada != -1 && t.cuadricula[t.fila_seleccionada][t.columna_seleccionada] != nullptr && t.personaje_auxiliar != nullptr) {
        t.personaje_seleccionado = t.cuadricula[t.fila_seleccionada][t.columna_seleccionada];
        t.reset_seleccion();
    }
    if (t.personaje_auxiliar != nullptr && t.personaje_seleccionado != nullptr) {
        PiezaTablero* auxiliar = t.personaje_auxiliar;
        int auxfila = t.personaje_auxiliar->get_fila();
        int auxcolumna = t.personaje_auxiliar->get_columna();
        int selfila = t.personaje_seleccionado->get_fila();
        int selcolumna = t.personaje_seleccionado->get_columna();

        t.cuadricula[auxfila][auxcolumna] = t.personaje_seleccionado;
        t.cuadricula[selfila][selcolumna] = auxiliar;
        t.cuadricula[auxfila][auxcolumna]->set_fila_columna(auxfila, auxcolumna);
        t.cuadricula[selfila][selcolumna]->set_fila_columna(selfila, selcolumna);

        t.turno = t.turno == LUZ ? OSCURIDAD : LUZ;
        if (personaje->get_ID() == tipo_pj::MH) hechizosLuz[3] = true;
        if (personaje->get_ID() == tipo_pj::Platero) hechizosOscuridad[3] = true;
        t.personaje_seleccionado = nullptr;
        t.personaje_auxiliar = nullptr;
        t.modoJuegoactual = ModoJuego::NORMAL;
    }
}

void Magia::Imprison(PiezaTablero* personaje, Tablero& t) {
    if ((personaje->get_ID() == tipo_pj::MH && hechizosLuz[6] == true) || (personaje->get_ID() == tipo_pj::Platero && hechizosOscuridad[6] == true)) {
        std::cout << "El hechizo Imprison ya se ha utilizado" << std::endl;
        t.modoJuegoactual = ModoJuego::HECHIZOS;
    }

    t.seleccionaCasilla();

    if (t.fila_seleccionada != -1 && t.columna_seleccionada != -1 && t.cuadricula[t.fila_seleccionada][t.columna_seleccionada] != nullptr) {
        t.personaje_seleccionado = t.cuadricula[t.fila_seleccionada][t.columna_seleccionada];
        t.reset_seleccion();
    }

    if ((personaje->get_ID() == tipo_pj::MH)) {
        if (t.personaje_seleccionado != nullptr) {
            if (t.personaje_seleccionado->get_equipo() != LUZ) {
                t.personaje_seleccionado->set_imprison(true);
                t.turno = t.turno == LUZ ? OSCURIDAD : LUZ;
                hechizosLuz[6] = true;
                t.personaje_seleccionado = nullptr;
                t.modoJuegoactual = ModoJuego::NORMAL;
            }
        }
    }

    if ((personaje->get_ID() == tipo_pj::Platero)) {
        if (t.personaje_seleccionado != nullptr) {
            if (t.personaje_seleccionado->get_equipo() != OSCURIDAD) {
                t.personaje_seleccionado->set_imprison(true);
                t.turno = t.turno == LUZ ? OSCURIDAD : LUZ;
                hechizosOscuridad[6] = true;
                t.personaje_seleccionado = nullptr;
                t.modoJuegoactual = ModoJuego::NORMAL;
            }
        }
    }
}

void Magia::Revive(PiezaTablero* personaje, Tablero& t) {
    if ((personaje->get_ID() == tipo_pj::MH && hechizosLuz[5] == true) || (personaje->get_ID() == tipo_pj::Platero && hechizosOscuridad[5] == true)) {
        std::cout << "El hechizo Revive ya se ha utilizado" << std::endl;
        t.modoJuegoactual = ModoJuego::HECHIZOS;
    }

    if ((personaje->get_ID() == tipo_pj::MH && t.cementerio_Luz.size() == 0) || (personaje->get_ID() == tipo_pj::Platero && t.cementerio_Oscuridad.size() == 0)) {
        std::cout << "No tienes personajes muertos que revivir" << std::endl;
        t.modoJuegoactual = ModoJuego::HECHIZOS;
    }

    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        int mouseX = GetMouseX();
        int mouseY = GetMouseY();

        if (personaje->get_equipo() == LUZ) {
            // Columna 1 del cementerio Luz (filas 0-8)
            if (mouseX > Tablero::cementerioLuzX
                && mouseX < Tablero::cementerioLuzX + Tablero::cementerioCelda) {
                for (int i = 0; i < 9; i++) {
                    int posY = Tablero::cementerioY + i * Tablero::cementerioCelda;
                    if (mouseY >= posY && mouseY < posY + Tablero::cementerioCelda) {
                        if (i < (int)t.cementerio_Luz.size())
                            t.personaje_muerto_seleccionado = t.cementerio_Luz[i];
                    }
                }
            }
            // Columna 2 del cementerio Luz (filas 9-17)
            if (mouseX > Tablero::cementerioLuzX + Tablero::cementerioCelda
                && mouseX < Tablero::cementerioLuzX + Tablero::cementerioCelda * 2) {
                for (int i = 9; i < 18; i++) {
                    int posY = Tablero::cementerioY + (i - 9) * Tablero::cementerioCelda;
                    if (mouseY >= posY && mouseY < posY + Tablero::cementerioCelda) {
                        if (i < (int)t.cementerio_Luz.size())
                            t.personaje_muerto_seleccionado = t.cementerio_Luz[i];
                    }
                }
            }
        }

        if (personaje->get_equipo() == OSCURIDAD) {
            // Columna 1 del cementerio Oscuridad (filas 0-8)
            if (mouseX > Tablero::cementerioOscX
                && mouseX < Tablero::cementerioOscX + Tablero::cementerioCelda) {
                for (int i = 0; i < 9; i++) {
                    int posY = Tablero::cementerioY + i * Tablero::cementerioCelda;
                    if (mouseY >= posY && mouseY < posY + Tablero::cementerioCelda) {
                        if (i < (int)t.cementerio_Oscuridad.size())
                            t.personaje_muerto_seleccionado = t.cementerio_Oscuridad[i];
                    }
                }
            }
            // Columna 2 del cementerio Oscuridad (filas 9-17)
            if (mouseX > Tablero::cementerioOscX + Tablero::cementerioCelda
                && mouseX < Tablero::cementerioOscX + Tablero::cementerioCelda * 2) {
                for (int i = 9; i < 18; i++) {
                    int posY = Tablero::cementerioY + (i - 9) * Tablero::cementerioCelda;
                    if (mouseY >= posY && mouseY < posY + Tablero::cementerioCelda) {
                        if (i < (int)t.cementerio_Oscuridad.size())
                            t.personaje_muerto_seleccionado = t.cementerio_Oscuridad[i];
                    }
                }
            }
        }

        if (t.personaje_muerto_seleccionado != nullptr) {
            t.seleccionaCasilla();
            if (t.cuadricula[t.fila_seleccionada][t.columna_seleccionada] == nullptr && t.fila_seleccionada != -1 && t.columna_seleccionada != -1) {
                t.cuadricula[t.fila_seleccionada][t.columna_seleccionada] = t.personaje_muerto_seleccionado;
                t.cuadricula[t.fila_seleccionada][t.columna_seleccionada]->set_fila_columna(t.fila_seleccionada, t.columna_seleccionada);

                if (t.personaje_muerto_seleccionado->get_equipo() == LUZ) EliminaMuerto(t.personaje_muerto_seleccionado, t.cementerio_Luz);
                else EliminaMuerto(t.personaje_muerto_seleccionado, t.cementerio_Oscuridad);

                t.turno = t.turno == LUZ ? OSCURIDAD : LUZ;
                if (personaje->get_ID() == tipo_pj::MH) hechizosLuz[5] = true;
                if (personaje->get_ID() == tipo_pj::Platero) hechizosOscuridad[5] = true;
                t.personaje_seleccionado = nullptr;
                t.personaje_muerto_seleccionado = nullptr;
                t.modoJuegoactual = ModoJuego::NORMAL;
            }
        }
    }
}

void Magia::Summon(PiezaTablero* personaje, Tablero& t) {
    if ((personaje->get_ID() == tipo_pj::MH && hechizosLuz[4]) ||
        (personaje->get_ID() == tipo_pj::Platero && hechizosOscuridad[4])) {
        t.modoJuegoactual = ModoJuego::HECHIZOS;
        return;
    }

    t.seleccionaCasilla();

    if (t.fila_seleccionada == -1 || t.columna_seleccionada == -1) return;
    PiezaTablero* objetivo = t.cuadricula[t.fila_seleccionada][t.columna_seleccionada];
    if (objetivo == nullptr || objetivo->get_equipo() == personaje->get_equipo()) {
        t.reset_seleccion();
        return;
    }

    int filaObj = t.fila_seleccionada;
    int colObj = t.columna_seleccionada;
    t.reset_seleccion();

    equipo equipoLanzador = personaje->get_equipo() == LUZ ? LUZ : OSCURIDAD;
    t.elemental_ = new PiezaTablero(pjboard::Elemental, filaObj, colObj, equipoLanzador);
    t.cuadricula[filaObj][colObj] = t.elemental_;

    t.summonPendiente_ = true;
    t.combatePendiente_ = true;
    t.atacante_ = t.elemental_;
    t.defensor_ = objetivo;
    t.filaOrigenAtacante_ = filaObj;
    t.colOrigenAtacante_ = colObj;
    t.filaDestinoAnim_ = filaObj;
    t.colDestinoAnim_ = colObj;

    if (personaje->get_ID() == tipo_pj::MH)     hechizosLuz[4] = true;
    if (personaje->get_ID() == tipo_pj::Platero) hechizosOscuridad[4] = true;

    personaje->set_seleccionado(false);
    t.personaje_usando_magia = nullptr;
    t.personaje_seleccionado = nullptr;

    t.modoJuegoactual = ModoJuego::COMBATE;
}

void Magia::EliminaMuerto(PiezaTablero* personaje_muerto_seleccionado, std::vector<PiezaTablero*>& Vector) {
    for (int i = 0; i < (int)Vector.size(); i++) {
        if (personaje_muerto_seleccionado == Vector[i]) {
            Vector.erase(Vector.begin() + i);
            break;
        }
    }
}

bool Magia::hechizoBloqueado(tipo_pj lanzador, int indice) const
{
    if (lanzador == tipo_pj::MH)     return hechizosLuz[indice];
    if (lanzador == tipo_pj::Platero) return hechizosOscuridad[indice];
    return false;
}
