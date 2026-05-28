#include "Magia.h"
#include "Tablero.h"
#include "iostream"



void Magia::Shift_Time(Personaje* personaje, Tablero &t) {

    if ((personaje->get_ID() == tipo_pj::MH && hechizosLuz[2] == true) || (personaje->get_ID() == tipo_pj::Platero && hechizosOscuridad[2] == true)) {

        std::cout << "El hechizo Shift ya se ha utilizado" << std::endl;
        t.modoJuegoactual = ModoJuego::HECHIZOS;

    }

    if ((personaje->get_ID() == tipo_pj::MH && hechizosLuz[2] == false) || (personaje->get_ID() == tipo_pj::Platero && hechizosOscuridad[2] == false)) {
        if (t.Ciclo != 0 && t.Ciclo != 4) {
            t.avance = !t.avance;
        }
        t.turno = t.turno == LUZ ? OSCURIDAD : LUZ;
        t.personaje_seleccionado = nullptr;
        t.personaje_usando_magia = nullptr;
        if (personaje->get_ID() == tipo_pj::MH) hechizosLuz[2] = true;
        if (personaje->get_ID() == tipo_pj::Platero) hechizosOscuridad[2] = true;
        t.modoJuegoactual = ModoJuego::NORMAL;

    }
}


void Magia::Teleport(Personaje* personaje, Tablero& t) {

    if ((personaje->get_ID() == tipo_pj::MH && hechizosLuz[0] == true) || (personaje->get_ID() == tipo_pj::Platero && hechizosOscuridad[0] == true)) {

        std::cout << "El hechizo Teleport ya se ha utilizado" << std::endl;
       t.modoJuegoactual = ModoJuego::HECHIZOS;

    }

    t.moverPieza();

    if (t.personaje_seleccionado == nullptr)  t.reset_MovimientosPosibles();


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

                for (int i = 0; i < 9; i++) {
                    for (int j = 0; j < 9; j++) {

                        if (t.cuadricula[i][j] == nullptr) t.movimientosPosibles[i][j] = true;
                    }
                }
            }
        }
    }

    if (personaje->get_ID() == tipo_pj::Platero && hechizosOscuridad[0] == false) {
        if (t.personaje_seleccionado != nullptr) {
            if (t.personaje_seleccionado->get_equipo() == OSCURIDAD) {
                for (int i = 0; i < 9; i++) {
                    for (int j = 0; j < 9; j++) {

                        if (t.cuadricula[i][j] == nullptr) t.movimientosPosibles[i][j] = true;
                    }
                }
            }
        }
    }


}


void Magia::Heal(Personaje* personaje, Tablero& t) {

    if ((personaje->get_ID() == tipo_pj::MH && hechizosLuz[1] == true) || (personaje->get_ID() == tipo_pj::Platero && hechizosOscuridad[1] == true)) {

        std::cout << "El hechizo Heal ya se ha utilizado" << std::endl;
        t.modoJuegoactual = ModoJuego::HECHIZOS;

    }

    t.seleccionaCasilla();

    if (t.cuadricula[t.fila_seleccionada][t.columna_seleccionada] != nullptr && t.fila_seleccionada != -1 && t.columna_seleccionada != -1) {
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
                std::cout << "Personaje curado" << std::endl;

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



void Magia::Exchange(Personaje* personaje, Tablero& t) {

    if ((personaje->get_ID() == tipo_pj::MH && hechizosLuz[3] == true) || (personaje->get_ID() == tipo_pj::Platero && hechizosOscuridad[3] == true)) {

        std::cout << "El hechizo Shift ya se ha utilizado" << std::endl;
        t.modoJuegoactual = ModoJuego::HECHIZOS;

    }

    t.seleccionaCasilla();

    if (t.cuadricula[t.fila_seleccionada][t.columna_seleccionada] != nullptr && t.fila_seleccionada != -1 && t.columna_seleccionada != -1 && t.personaje_auxiliar == nullptr) {
        t.personaje_auxiliar = t.cuadricula[t.fila_seleccionada][t.columna_seleccionada];
        std::cout << "El hechizo Shift ya se ha utilizado" << std::endl;
        t.reset_seleccion();
    }
    if (t.cuadricula[t.fila_seleccionada][t.columna_seleccionada] != nullptr && t.fila_seleccionada != -1 && t.columna_seleccionada != -1 && t.personaje_auxiliar != nullptr) {
        t.personaje_seleccionado = t.cuadricula[t.fila_seleccionada][t.columna_seleccionada];
        std::cout << "El hechizo Shift ya se ha utilizado" << std::endl;
        t.reset_seleccion();
    }
    if (t.personaje_auxiliar != nullptr && t.personaje_seleccionado != nullptr) {

        Personaje* auxiliar = t.personaje_auxiliar;
        int auxfila, auxcolumna;

        auxfila = t.personaje_auxiliar->get_fila();
        auxcolumna = t.personaje_auxiliar->get_columna();

        int selfila, selcolumna;

        selfila = t.personaje_seleccionado->get_fila();
        selcolumna = t.personaje_seleccionado->get_columna();

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


void Magia::Imprison(Personaje* personaje, Tablero& t) {

    if ((personaje->get_ID() == tipo_pj::MH && hechizosLuz[6] == true) || (personaje->get_ID() == tipo_pj::Platero && hechizosOscuridad[6] == true)) {

        std::cout << "El hechizo Imprison ya se ha utilizado" << std::endl;
        t.modoJuegoactual = ModoJuego::HECHIZOS;

    }

    t.seleccionaCasilla();

    if (t.cuadricula[t.fila_seleccionada][t.columna_seleccionada] != nullptr && t.fila_seleccionada != -1 && t.columna_seleccionada != -1) {
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





void Magia::Revive(Personaje* personaje, Tablero& t) {

    
    if ((personaje->get_ID() == tipo_pj::MH && hechizosLuz[5] == true) || (personaje->get_ID() == tipo_pj::Platero && hechizosOscuridad[5] == true)) {

        std::cout << "El hechizo Revive ya se ha utilizado" << std::endl;
        t.modoJuegoactual = ModoJuego::HECHIZOS;

    }

    std::cout << t.cementerio_Luz.size() << std::endl;

    if ((personaje->get_ID() == tipo_pj::MH && t.cementerio_Luz.size() == 0) || (personaje->get_ID() == tipo_pj::Platero && t.cementerio_Oscuridad.size() == 0)) {

        std::cout << "No tienes personajes muertos que revivir" << std::endl;
        t.modoJuegoactual = ModoJuego::HECHIZOS;

    }

    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {

        int mouseX = GetMouseX();
        int mouseY = GetMouseY();

        if (personaje->get_equipo() == LUZ) {
            if (mouseX > 18 && mouseX < 82) {

                for (int i = 0; i < 9; i++) {

                    int posY = 4 + i * t.tamanoCasilla;
                    if (mouseY <= (posY + t.tamanoCasilla) && mouseY > posY) {
                        if (i < t.cementerio_Luz.size()) {
                            t.personaje_muerto_seleccionado = t.cementerio_Luz[i];
                        }
                    }

                }
            }

            if (mouseX > 82 && mouseX < 146) {

                for (int i = 10; i < 18; i++) {

                    int posY = 4 + (i - 9) * t.tamanoCasilla;
                    if (mouseY <= (posY + t.tamanoCasilla) && mouseY > posY) {
                        if (i < t.cementerio_Luz.size()) {
                            t.personaje_muerto_seleccionado = t.cementerio_Luz[i];
                        }
                    }

                }
            }
        }

        if (personaje->get_equipo() == OSCURIDAD) {
            if (mouseX > 818 && mouseX < 882) {

                for (int i = 0; i < 9; i++) {

                    int posY = 4 + i * t.tamanoCasilla;
                    if (mouseY <= (posY + t.tamanoCasilla) && mouseY > posY) {
                        if (i < t.cementerio_Oscuridad.size()) {
                            t.personaje_muerto_seleccionado = t.cementerio_Oscuridad[i];
                        }
                    }

                }
            }

            if (mouseX > 882 && mouseX < 946) {

                for (int i = 10; i < 18; i++) {

                    int posY = 4 + (i - 9) * t.tamanoCasilla;
                    if (mouseY <= (posY + t.tamanoCasilla) && mouseY > posY) {
                        if (i < t.cementerio_Oscuridad.size()) {
                            t.personaje_muerto_seleccionado = t.cementerio_Oscuridad[i];
                        }
                    }

                }
            }
        }


        if (t.personaje_muerto_seleccionado != nullptr) {

            t.seleccionaCasilla();
            std::cout << t.fila_seleccionada << " " << t.columna_seleccionada << std::endl;
            if (t.cuadricula[t.fila_seleccionada][t.columna_seleccionada] == nullptr && t.fila_seleccionada != -1 && t.columna_seleccionada != -1) {

                t.cuadricula[t.fila_seleccionada][t.columna_seleccionada] = t.personaje_muerto_seleccionado;
                t.cuadricula[t.fila_seleccionada][t.columna_seleccionada]->set_fila_columna(t.fila_seleccionada, t.columna_seleccionada);

                
                //Borramos el personaje de la lista

                if (t.personaje_muerto_seleccionado->get_equipo() == LUZ) EliminaMuerto(t.personaje_muerto_seleccionado, t.cementerio_Luz);
                else  if (t.personaje_muerto_seleccionado->get_equipo() == OSCURIDAD) EliminaMuerto(t.personaje_muerto_seleccionado, t.cementerio_Oscuridad);




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



void Magia::EliminaMuerto(Personaje* personaje_muerto_seleccionado, std::vector<Personaje*>& Vector) {

  
    for (int i = 0; i < Vector.size(); i++) {

        if (personaje_muerto_seleccionado == Vector[i]) {

            int index = i;  
            Vector.erase(Vector.begin() + index);
            break;       
        }
    }

}