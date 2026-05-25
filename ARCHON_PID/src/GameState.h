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

// Estados de la aplicación — el flujo es lineal hacia abajo y ESC sube
enum Estado {
    MENU,
    OPCIONES, MUSICA, ENCICLOPEDIA,
    MODO_JUEGO,            // Combate / Juego Completo
    SELECCION_MODO,        // PvP / Contra IA
    CONFIG_JUEGO_COMPLETO, // Bando + dificultad (Juego Completo)
    SELECCION_EQUIPO,      // Elegir Luz u Oscuridad (Combate)
    CONFIG_DIFICULTAD,     // Dificultad IA (Combate)
    CARGAR_PARTIDA,
    PAUSA,
    JUGAR_PVP, JUGAR_IA,
};

enum ModoJuego  { MODO_NINGUNO, MODO_COMBATE, MODO_COMPLETO };
enum Bando      { BANDO_NINGUNO, BANDO_LUZ, BANDO_OSCURIDAD, BANDO_RANDOM };
enum Dificultad { DIFI_FACIL, DIFI_NORMAL, DIFI_DIFICIL, DIFI_MAESTRO };

// Datos de un equipo (Luz / Oscuridad) para la pantalla de selección
struct DatosEquipo {
    std::string nombre;
    std::string descripcion;  // texto corto para la tarjeta
    // Stats del equipo (0-10) que se usan para la barra visual
    int ataque, defensa, velocidad, magia;
    float colorR, colorG, colorB;
    // Qué sprites mostrar en la tarjeta (índices para dibujarSprite)
    int sprite1, sprite2;
};

// Partida guardada — lo que se escribe en disco
struct PartidaGuardada {
    std::string nombre, fecha, modo, bando;
    int   turno      = 0;
    float progreso   = 0.f;
    int   equipo1    = 0;   // índice en gs.equipos
    int   equipo2    = 1;
    int   dificultad = 1;   // valor del enum Dificultad
};

struct OpcionControl { std::string nombre; int valor, minV, maxV; bool esBool; };
struct Cancion        { std::string titulo, artista, duracion, contexto; float r,g,b; };
struct DatosTropa {
    std::string nombre, bando, stat1, stat2, stat3;
    std::vector<std::string> descripcion;
};
struct Particula        { float x,y,radio,angulo,velocidadAngular; };
struct ParticulaFuego   { float x,y,vy,vida,vidaMax,size; };
struct ParticulaTeatro  { float x,y,vx,vy,vida,vidaMax,size,r,g,b; };
struct ParticulaPolvo   { float x,y,vx,vy,vida,vidaMax,size; };
struct ParticulaNota    { float x,y,vy,vx,vida,vidaMax,size,rot; int tipo; };

static const int NUM_BARRAS = 48;

inline Color CFloat(float r, float g, float b, float a = 1.0f) {
    return {
        (unsigned char)(std::fmin(std::fmax(r,0.f),1.f)*255),
        (unsigned char)(std::fmin(std::fmax(g,0.f),1.f)*255),
        (unsigned char)(std::fmin(std::fmax(b,0.f),1.f)*255),
        (unsigned char)(std::fmin(std::fmax(a,0.f),1.f)*255)
    };
}

struct GameState {
    Estado estadoActual    = MENU;
    Estado siguienteEstado = MENU;
    Estado estadoAnterior  = MENU;  // para volver desde PAUSA
    float  tiempo          = 0.f;
    bool   pantallaCompleta= false;

    // Offsets de animación de deslizamiento al entrar en cada pantalla
    float menuOffset       = -600.f;
    float encicloOffset    = -800.f;
    float opcionesOffset   = -800.f;
    float musicaOffset     = -800.f;
    float modoJuegoOffset  = -600.f;
    float selModoOffset    = -600.f;
    float configJCOffset   = -800.f;
    float selEquipoOffset  = -800.f;
    float configDifiOffset = -800.f;
    float cargaOffset      = -800.f;
    float pausaOffset      =  600.f; // entra desde arriba

    bool  slashActivo = false;
    float slashX      = -1000.f;

    // Menú principal — 4 opciones
    std::vector<std::string> opcionesMenu = {
        "JUGAR", "CARGAR PARTIDA", "OPCIONES", "ENCICLOPEDIA"
    };
    int opcionMenuSel = 0;

    // Modo de juego seleccionado
    ModoJuego modoActual    = MODO_NINGUNO;
    int       opcionModoSel = 0;

    // PvP (0) o IA (1)
    int opcionSelModoSel = 0;

    // Configuración Juego Completo
    Bando      bandoSel      = BANDO_NINGUNO;
    int        opcionBandoSel= 0;
    Dificultad dificultadSel = DIFI_NORMAL;
    int        opcionDifiSel = 1;
    int        configJCFoco  = 0;  // 0=panel bando, 1=panel dificultad

    // Selección de equipo (Combate) — 0=Luz, 1=Oscuridad
    int  equipoSel1       = 0;
    int  equipoSel2       = 1;
    int  equipoFoco       = 0;   // qué jugador está eligiendo ahora
    bool seleccionandoJ1  = true;

    // Dificultad IA en Combate
    int opcionDifiCombateSel = 1;

    // Pausa
    int opcionPausaSel = 0;
    // Tiene "GUARDAR" como primera opción
    std::vector<std::string> opcionesPausa = {
        "CONTINUAR", "GUARDAR PARTIDA", "OPCIONES", "MENU PRINCIPAL"
    };
    bool guardadoOk      = false; // para mostrar mensaje de confirmación
    float guardadoTimer  = 0.f;

    // Nombre propuesto para la partida al guardar
    std::string nombreGuardado = "Partida Nueva";

    // Cargar partida
    int opcionCargaSel = 0;
    std::vector<PartidaGuardada> partidas;

    // Enciclopedia
    int paginaLibro = 0;
    static const int TOTAL_PAGINAS = 4;
    std::vector<DatosTropa> tropas;

    std::vector<OpcionControl> controlesOpciones;
    int opcionOpcionesSel = 0;

    std::vector<Cancion> canciones;
    int   cancionActual   = 0;
    int   cancionHover    = 0;
    bool  reproduciendo   = true;
    bool  repetir         = false;
    bool  aleatorio       = false;
    int   volumenMusica   = 8;
    float progresoCancion = 0.f;
    float velocidadProg   = 0.0004f;
    float barrasViz[NUM_BARRAS]{};
    float barraTarget[NUM_BARRAS]{};

    std::vector<Particula>       particulas;
    std::vector<ParticulaFuego>  particulasFuego;
    std::vector<ParticulaTeatro> particulasTeatro;
    std::vector<ParticulaPolvo>  particulasPolvo;
    std::vector<ParticulaNota>   particulasNotas;

    // Los dos equipos del juego
    std::vector<DatosEquipo> equipos;

    void init() {
        // Tropas para la enciclopedia
        tropas = {
            {"CABALLERO","Fuerzas de Luz","ATQ: Alto","DEF: Alto","MOV: Medio",
             {"Guerrero de elite blindado con","armadura sagrada. Lidera la carga",
              "en nombre del rey de la Luz.","Efectivo contra unidades pesadas."}},
            {"ARQUERO","Fuerzas de Luz","ATQ: Medio","DEF: Bajo","MOV: Alto",
             {"Explorador agil de ojos certeros.","Sus flechas besan el viento antes",
              "de encontrar su blanco.","Efectivo contra unidades voladoras."}},
            {"ORCO","Fuerzas Oscuras","ATQ: Muy Alto","DEF: Medio","MOV: Bajo",
             {"Bestia de la oscuridad. Su furia","es legendaria en el campo de batalla.",
              "Arremete sin miedo ni piedad.","Efectivo contra estructuras."}},
            {"NIGROMANTE","Fuerzas Oscuras","ATQ: Bajo","DEF: Bajo","MOV: Medio",
             {"Maestro de la magia sombria.","Invoca espiritus y corroe la",
              "voluntad del enemigo.","Efectivo contra grupos numerosos."}}
        };

        // Los dos únicos equipos del juego
        equipos = {
            {
                "FUERZAS DE LUZ",
                "Caballeros y arqueros al servicio\ndel rey. Atacan en formacion.",
                7, 8, 6, 5,
                0.9f, 0.85f, 0.25f,
                0, 1   // caballero + arquero
            },
            {
                "FUERZAS OSCURAS",
                "Orcos y Nigromantes sin piedad.\nLa fuerza bruta es su estandarte.",
                9, 6, 4, 7,
                0.5f, 0.1f, 0.8f,
                2, 3   // orco + nigromante
            },
        };

        controlesOpciones = {
            {"Volumen Musica",      8, 0, 10, false},
            {"Volumen Efectos",     7, 0, 10, false},
            {"Pantalla Completa",   0, 0,  1, true },
            {"Particulas",          1, 0,  1, true },
            {"Velocidad Animacion", 5, 1, 10, false},
            {"Idioma",              0, 0,  1, true },
        };
        canciones = {
            {"Skill issue bro",     "Archon OST","4:12","Menu Principal",    0.6f,0.5f,0.9f},
            {"Espabila Julian",     "Archon OST","3:47","Batalla PvP",       0.9f,0.2f,0.2f},
            {"March of the Fallen", "Archon OST","5:03","Batalla vs IA",     0.3f,0.7f,0.4f},
            {"Sacred Grounds",      "Archon OST","2:58","Victoria Luz",      0.9f,0.85f,0.3f},
            {"The Abyss Calls",     "Archon OST","4:31","Victoria Oscuridad",0.5f,0.1f,0.8f},
            {"Eternal Siege",       "Archon OST","6:15","Modo Clasico",      0.8f,0.4f,0.1f},
            {"Lament of the Knight","Archon OST","3:22","Derrota",           0.4f,0.4f,0.6f},
        };
        for (int i = 0; i < NUM_BARRAS; i++) {
            barrasViz[i]   = 2.f + rand() % 8;
            barraTarget[i] = 2.f + rand() % 30;
        }
    }
};
