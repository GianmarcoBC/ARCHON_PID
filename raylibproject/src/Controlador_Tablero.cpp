#include "Controlador_Tablero.h"
#include "Personaje.h"
#include <sstream>



Controlador_Tablero::Controlador_Tablero(bool vsAI, int dificultad, equipo equipoAI):tablero(), equipoAI(equipoAI) {

    tablero.inicializarTablero();

    if (vsAI) {

        switch (dificultad) {
        case 0:  ia_Tablero = new AI_Facil_Tablero(equipoAI); break;
        case 1:  ia_Tablero = new AI_Defensiva_Tablero(equipoAI); break;
        case 2:  ia_Tablero = new AI_Agresiva_Tablero(equipoAI); break;
        default: ia_Tablero = new AI_Facil_Tablero(equipoAI); break;
        }

    }
    
}

void Controlador_Tablero::Logica_controlador(bool vsAI) {
    // Bloquear input del jugador si es turno de la IA
    tablero.bloquearInputJugador = vsAI && (tablero.turno == (bool)equipoAI);

    tablero.LogicaTablero();

    if (tablero.get_modoJuegoActual() != ModoJuego::NORMAL) return;
    if (!vsAI || tablero.turno != (bool)equipoAI) return;

    // Lanzar el calculo en un hilo aparte si no esta ya en curso
    if (!calculandoIA_) {
        calculandoIA_ = true;
        futuroMovIA_ = std::async(std::launch::async,
            [this]() {
                return ia_Tablero->decidirMovimiento(tablero.cuadricula);
            });
        return;
    }

    // Comprobar si el hilo ya termin� (sin bloquear)
    if (futuroMovIA_.wait_for(std::chrono::seconds(0)) != std::future_status::ready) {
        return;
    }

    // El resultado est� listo: recogerlo y aplicarlo
    MovTablero movimiento = futuroMovIA_.get();
    calculandoIA_ = false;

    PiezaTablero* personaje = tablero.cuadricula[movimiento.filaOrigen][movimiento.colOrigen];

    if (tablero.cuadricula[movimiento.filaDestino][movimiento.colDestino] != nullptr) {
        tablero.combatePendiente_ = true;
        tablero.atacante_ = personaje;
        tablero.defensor_ = tablero.cuadricula[movimiento.filaDestino][movimiento.colDestino];
        tablero.filaOrigenAtacante_ = movimiento.filaOrigen;
        tablero.colOrigenAtacante_ = movimiento.colOrigen;
    }

    tablero.piezaAnimando_ = personaje;
    tablero.filaDestinoAnim_ = movimiento.filaDestino;
    tablero.colDestinoAnim_ = movimiento.colDestino;
    personaje->iniciarMovimiento(movimiento.filaDestino, movimiento.colDestino, tablero.cellSize3D);
    tablero.modoJuegoactual = ModoJuego::ANIMANDO_MOVIMIENTO;
}

std::string Controlador_Tablero::GuardarEstado() const {
    std::string s = tablero.serializarEstado();
    // Añadir estado del combate 3D si hay uno activo
    if (combateVidaP1 >= 0) {
        s += "combateVidaP1=" + std::to_string(combateVidaP1) + "\n";
        s += "combatePosP1x=" + std::to_string(combatePosP1x) + "\n";
        s += "combatePosP1y=" + std::to_string(combatePosP1y) + "\n";
        s += "combateVidaP2=" + std::to_string(combateVidaP2) + "\n";
        s += "combatePosP2x=" + std::to_string(combatePosP2x) + "\n";
        s += "combatePosP2y=" + std::to_string(combatePosP2y) + "\n";
    }
    return s;
}

void Controlador_Tablero::CargarEstado(const std::string& data) {
    tablero.deserializarEstado(data);
    // Extraer estado del combate 3D si existe
    clearCombateState();
    std::istringstream stream(data);
    std::string linea;
    float v1 = -1, px1 = 0, py1 = 0, v2 = -1, px2 = 0, py2 = 0;
    while (std::getline(stream, linea)) {
        auto eq = linea.find('=');
        if (eq == std::string::npos) continue;
        std::string clave = linea.substr(0, eq);
        std::string valor = linea.substr(eq + 1);
        if (clave == "combateVidaP1")  v1  = std::stof(valor);
        else if (clave == "combatePosP1x") px1 = std::stof(valor);
        else if (clave == "combatePosP1y") py1 = std::stof(valor);
        else if (clave == "combateVidaP2") v2  = std::stof(valor);
        else if (clave == "combatePosP2x") px2 = std::stof(valor);
        else if (clave == "combatePosP2y") py2 = std::stof(valor);
    }
    if (v1 >= 0 && v2 >= 0) setCombateState(v1, px1, py1, v2, px2, py2);
}