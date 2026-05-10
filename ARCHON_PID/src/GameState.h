#pragma once
#include "raylib.h"
#include <vector>
#include <string>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <algorithm>

#ifndef M_PI
#define M_PI 3.14159265358979323846f
#endif

enum Estado {
    MENU,
    JUGAR, JUGAR_PVP, JUGAR_IA,
    OPCIONES,
    MUSICA,
    ENCICLOPEDIA
};

struct OpcionControl {
    std::string nombre;
    int valor, minV, maxV;
    bool esBool;
};

struct Cancion {
    std::string titulo, artista, duracion, contexto;
    float r, g, b;
};

struct DatosTropa {
    std::string nombre, bando, stat1, stat2, stat3;
    std::vector<std::string> descripcion;
};

struct Particula { float x, y, radio, angulo, velocidadAngular; };
struct ParticulaFuego { float x, y, vy, vida, vidaMax, size; };
struct ParticulaTeatro { float x, y, vx, vy, vida, vidaMax, size, r, g, b; };
struct ParticulaPolvo { float x, y, vx, vy, vida, vidaMax, size; };
struct ParticulaNota { float x, y, vy, vx, vida, vidaMax, size, rot; int tipo; };

static const int NUM_BARRAS = 48;

inline Color CFloat(float r, float g, float b, float a = 1.0f) {
    return {
        (unsigned char)(std::fmin(std::fmax(r, 0.0f), 1.0f) * 255),
        (unsigned char)(std::fmin(std::fmax(g, 0.0f), 1.0f) * 255),
        (unsigned char)(std::fmin(std::fmax(b, 0.0f), 1.0f) * 255),
        (unsigned char)(std::fmin(std::fmax(a, 0.0f), 1.0f) * 255)
    };
}

struct GameState {
    Estado estadoActual = MENU;
    Estado siguienteEstado = MENU;
    float tiempo = 0.0f;
    bool pantallaCompleta = false;

    float menuOffset = -600.0f;
    float jugarOffset = -600.0f;
    float encicloOffset = -800.0f;
    float opcionesOffset = -800.0f;
    float musicaOffset = -800.0f;

    bool slashActivo = false;
    float slashX = -1000.0f;

    std::vector<std::string> opcionesMenu = { "JUGAR", "OPCIONES", "ENCICLOPEDIA" };
    int opcionMenuSel = 0;

    std::vector<std::string> opcionesJugar = { "PVP", "CONTRA IA" };
    int opcionJugarSel = 0;
    std::vector<std::string> descJugar = {
        "Desafia a otro jugador. Domina\nel tablero y demuestra tu maestria.",
        "Enfrenta a la maquina. Pon a\nprueba tu estrategia contra la IA."
    };

    int paginaLibro = 0;
    static const int TOTAL_PAGINAS = 4;
    std::vector<DatosTropa> tropas;

    std::vector<OpcionControl> controlesOpciones;
    int opcionOpcionesSel = 0;

    std::vector<Cancion> canciones;
    int cancionActual = 0;
    int cancionHover = 0;
    bool reproduciendo = true;
    bool repetir = false;
    bool aleatorio = false;
    int volumenMusica = 8;
    float progresoCancion = 0.0f;
    float velocidadProg = 0.0004f;

    float barrasViz[NUM_BARRAS]{};
    float barraTarget[NUM_BARRAS]{};

    std::vector<Particula> particulas;
    std::vector<ParticulaFuego> particulasFuego;
    std::vector<ParticulaTeatro> particulasTeatro;
    std::vector<ParticulaPolvo> particulasPolvo;
    std::vector<ParticulaNota> particulasNotas;

    void init() {
        tropas = {
            { "CABALLERO", "Fuerzas de Luz", "ATQ: Alto", "DEF: Alto", "MOV: Medio",
              { "Guerrero de elite blindado con", "armadura sagrada. Lidera la carga",
                "en nombre del rey de la Luz.", "Efectivo contra unidades pesadas." } },
            { "ARQUERO", "Fuerzas de Luz", "ATQ: Medio", "DEF: Bajo", "MOV: Alto",
              { "Explorador agil de ojos certeros.", "Sus flechas besan el viento antes",
                "de encontrar su blanco.", "Efectivo contra unidades voladoras." } },
            { "ORCO", "Fuerzas Oscuras", "ATQ: Muy Alto", "DEF: Medio", "MOV: Bajo",
              { "Bestia de la oscuridad. Su furia", "es legendaria en el campo de batalla.",
                "Arremete sin miedo ni piedad.", "Efectivo contra estructuras." } },
            { "NIGROMANTE", "Fuerzas Oscuras", "ATQ: Bajo", "DEF: Bajo", "MOV: Medio",
              { "Maestro de la magia sombria.", "Invoca espiritus y corroe la",
                "voluntad del enemigo.", "Efectivo contra grupos numerosos." } }
        };
        controlesOpciones = {
            { "Volumen Musica",      8, 0, 10, false },
            { "Volumen Efectos",     7, 0, 10, false },
            { "Pantalla Completa",   0, 0,  1, true  },
            { "Particulas",          1, 0,  1, true  },
            { "Velocidad Animacion", 5, 1, 10, false },
            { "Idioma",              0, 0,  1, true  },
        };
        canciones = {
            { "Skill issue bro",      "Archon OST", "4:12", "Menu Principal",     0.6f, 0.5f, 0.9f },
            { "Espabila Julian",      "Archon OST", "3:47", "Batalla PvP",        0.9f, 0.2f, 0.2f },
            { "March of the Fallen",  "Archon OST", "5:03", "Batalla vs IA",      0.3f, 0.7f, 0.4f },
            { "Sacred Grounds",       "Archon OST", "2:58", "Victoria Luz",       0.9f, 0.85f,0.3f },
            { "The Abyss Calls",      "Archon OST", "4:31", "Victoria Oscuridad", 0.5f, 0.1f, 0.8f },
            { "Eternal Siege",        "Archon OST", "6:15", "Modo Clasico",       0.8f, 0.4f, 0.1f },
            { "Lament of the Knight", "Archon OST", "3:22", "Derrota",            0.4f, 0.4f, 0.6f },
        };
        for (int i = 0; i < NUM_BARRAS; i++) {
            barrasViz[i] = 2.0f + rand() % 8;
            barraTarget[i] = 2.0f + rand() % 30;
        }
    }
};
