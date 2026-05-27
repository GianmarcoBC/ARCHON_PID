#pragma once
#include "raylib.h"
#include <vector>
#include <string>
#include <cmath>
#include <cstdlib>
#include "SaveData.h"
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
    CUCHAU_COMBATE,        // Modo Combate 3D (Cuchau)
};

enum ModoJuego  { MODO_NINGUNO, MODO_COMBATE, MODO_COMPLETO };
enum Bando      { BANDO_NINGUNO, BANDO_LUZ, BANDO_OSCURIDAD, BANDO_RANDOM };
enum Dificultad { DIFI_FACIL, DIFI_NORMAL, DIFI_DIFICIL };

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
struct Cancion        { std::string titulo, artista, duracion, contexto, archivo; float r,g,b; };
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
        "JUGAR", "CARGAR PARTIDA", "OPCIONES", "SALA DE ROLITAS", "ENCICLOPEDIA", "SALIR"
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

    int opcionSlotSel = 0;   

    // Enciclopedia
    int paginaLibro = 0;
    static const int TOTAL_PAGINAS = 16;
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
        // Tropas para la enciclopedia — Equipo Claro (0-7) + Equipo Oscuro (8-15)
        tropas = {
            // --- EQUIPO CLARO ---
            {"MH","Fuerzas de Luz","ATQ: Alto","DEF: Medio","MOV: Medio",
             {"Mago principal del equipo de Luz.","Domina las artes arcanas y ataca",
              "a distancia con proyectiles magicos.","Vuela sobre el campo de batalla."}},
            {"PHOENIX","Fuerzas de Luz","ATQ: Alto","DEF: Alto","MOV: Medio",
             {"Ave fenix renacida del fuego.","Vida y fuerza altas pero ataque lento.",
              "Sus llamas arrasan en area.","Vuela majestuosamente sobre el campo."}},
            {"GOLEM","Fuerzas de Luz","ATQ: Alto","DEF: Alto","MOV: Bajo",
             {"Tanque de piedra imparable.","Lento pero resistente, golpea con",
              "fuerza devastadora.","Su presencia intimida al enemigo."}},
            {"DJINNI","Fuerzas de Luz","ATQ: Medio","DEF: Alto","MOV: Medio",
             {"Genio mistico de los vientos.","Equilibrado en todas sus estadisticas.",
              "Flota por el aire lanzando","hechizos a sus rivales."}},
            {"UNICORN","Fuerzas de Luz","ATQ: Medio","DEF: Medio","MOV: Medio",
             {"Criatura magica de gran agilidad.","Dispara rapido y con precision.",
              "Su cooldown bajo le permite","mantener presion constante."}},
            {"VALKYRIE","Fuerzas de Luz","ATQ: Medio","DEF: Medio","MOV: Medio",
             {"Guerrera alada del Valhalla.","Combate cuerpo a cuerpo con su lanza.",
              "Vuela sobre las lineas enemigas","y ataca sin piedad."}},
            {"ARCHER","Fuerzas de Luz","ATQ: Bajo","DEF: Bajo","MOV: Medio",
             {"Explorador agil de ojos certeros.","Sus flechas besan el viento antes",
              "de encontrar su blanco.","Ataca a distancia con precision."}},
            {"KNIGHT","Fuerzas de Luz","ATQ: Bajo","DEF: Bajo","MOV: Medio",
             {"Caballero de elite con armadura","sagrada. Sin tiempo de espera entre",
              "ataques, golpea sin cesar.","Rapido y letal en combate cercano."}},
            // --- EQUIPO OSCURO ---
            {"PLATERO","Fuerzas Oscuras","ATQ: Medio","DEF: Medio","MOV: Medio",
             {"Mago oscuro de mirada siniestra.","Lanza proyectiles rapidos desde",
              "las sombras. Vuela envuelto","en oscuridad arcana."}},
            {"SHAPESHIFTER","Fuerzas Oscuras","ATQ: Alto","DEF: Alto","MOV: Medio",
             {"Cambiaformas bestial y feroz.","Vida y fuerza altas, ataca cuerpo",
              "a cuerpo con garras letales.","Vuela adoptando formas terrorificas."}},
            {"TROLL","Fuerzas Oscuras","ATQ: Alto","DEF: Alto","MOV: Bajo",
             {"Tanque del equipo oscuro.","Lento pero devastador en combate",
              "cercano. Su piel gruesa","resiste los golpes mas duros."}},
            {"DRAGON","Fuerzas Oscuras","ATQ: Muy Alto","DEF: Muy Alto","MOV: Medio",
             {"La criatura mas poderosa.","Vida y fuerza descomunales. Su aliento",
              "arrasa todo a su paso.","El terror alado del campo de batalla."}},
            {"BASILISK","Fuerzas Oscuras","ATQ: Alto","DEF: Bajo","MOV: Medio",
             {"Serpiente mitica de mirada mortal.","Fuerza alta con cooldown bajo,",
              "dispara rapido y certero.","Fragil pero extremadamente letal."}},
            {"BANSHEE","Fuerzas Oscuras","ATQ: Medio","DEF: Medio","MOV: Medio",
             {"Fantasma aullador de ultratumba.","Cooldown alto pero proyectil",
              "muy rapido. Ataca cuerpo a cuerpo","con gritos que hielan la sangre."}},
            {"MANTICORE","Fuerzas Oscuras","ATQ: Bajo","DEF: Medio","MOV: Medio",
             {"Bestia mitica con cuerpo de leon.","Fuerza baja pero ataca a distancia",
              "con proyectiles. Su presencia","impone respeto en el campo."}},
            {"GOBLIN","Fuerzas Oscuras","ATQ: Bajo","DEF: Bajo","MOV: Medio",
             {"Duende veloz y escurridizo.","Sin tiempo de espera entre ataques,",
              "golpea sin cesar cuerpo a cuerpo.","Pequeno pero implacable."}},
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
            {"Skill Issue Bro",          "Archon OST","---","Menu Principal",     "bin/Resources/AAAudio/Musica/Menugnomo.mp3",           0.6f,0.5f,0.9f},
            {"Espabila Julian",          "Archon OST","---","Batalla Epica",      "bin/Resources/AAAudio/Musica/ageofwar.mp3",            0.9f,0.2f,0.2f},
            {"The Peak",                 "Archon OST","---","Duelo Estrategico",  "bin/Resources/AAAudio/Musica/blackjack.mp3",           0.3f,0.7f,0.4f},
            {"No Cap On God Bro",        "Archon OST","---","Creditos",           "bin/Resources/AAAudio/Musica/credits.mp3",             0.9f,0.85f,0.3f},
            {"Bombardilo Coccodrillo",   "Archon OST","---","Victoria",           "bin/Resources/AAAudio/Musica/ifrith.mp3",              0.5f,0.1f,0.8f},
            {"TIA Portal Contraataca",   "Archon OST","---","Tablero Inicio",     "bin/Resources/AAAudio/Musica/MusciaTablero Inicio.mp3",0.8f,0.4f,0.1f},
            {"Oh Mi Dios",               "Archon OST","---","Tablero Final",      "bin/Resources/AAAudio/Musica/slimshady.mp3",           0.4f,0.4f,0.6f},
            {"Santa Vaca",               "Archon OST","---","Batalla PvP",        "bin/Resources/AAAudio/Musica/MusicaBatalla1V1.mp3",    0.7f,0.3f,0.3f},
            {"Clase Padre",              "Archon OST","---","MH vs Platero",      "bin/Resources/AAAudio/Musica/MusicaMagoMago.mp3",      0.4f,0.6f,0.9f},
            {"Esto es Cine Senores",     "Archon OST","---","Peon vs Mago",       "bin/Resources/AAAudio/Musica/MusicaPeonMago.mp3",      0.6f,0.8f,0.4f},
        };
        for (int i = 0; i < NUM_BARRAS; i++) {
            barrasViz[i]   = 2.f + rand() % 8;
            barraTarget[i] = 2.f + rand() % 30;
        }
    }
};