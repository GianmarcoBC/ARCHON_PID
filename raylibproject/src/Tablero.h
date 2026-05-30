#pragma once
#include "raylib.h"
#include "PiezaTablero.h"
#include "Magia.h"
#include "Pj.h"
#include <vector>
#include <cmath>


/*
 * Tablero.h - Tablero de juego 9x9 estilo Archon
 *
 * Gestiona toda la logica del juego de tablero por turnos:
 * - Cuadricula 9x9 con piezas de dos equipos (LUZ y OSCURIDAD)
 * - Turnos alternados con movimiento segun rango y tipo (vuelo/terrestre)
 * - Casillas de 3 colores: blancas, negras y cambiantes (ciclo de oscilacion)
 * - Sistema de hechizos (Teleport, Heal, Shift, Exchange, Imprison, Revive)
 * - Deteccion de victoria (5 puntos de poder o eliminacion total)
 * - Interfaz de combate: cuando una pieza ataca a otra, se lanza el combate 3D
 *
 * Los puntos de poder estan en las posiciones: (0,4), (4,0), (4,4), (4,8), (8,4)
 * formando una cruz en el centro del tablero.
 */

// Color de cada casilla del tablero
enum class ColorCasilla { BLANCO, NEGRO, CAMBIANTE };

// Estados del juego - determinan que logica y que pantalla se muestra
enum class ModoJuego {
    NORMAL,     // Turno normal: seleccionar pieza y moverla
    ANIMANDO_MOVIMIENTO, // Pieza moviendose con animacion Mov
    COMBATE,    // Combate 3D en curso (se delega a ControladorCombate)
    HECHIZOS,   // Menu de seleccion de hechizo (teclas S/H/T/E/I/R)
    SHIFT,      // Ejecutando hechizo Shift Time
    HEAL,       // Ejecutando hechizo Heal (seleccionar aliado)
    TELEPORT,   // Ejecutando hechizo Teleport (mover aliado a cualquier casilla vacia)
    EXCHANGE,   // Ejecutando hechizo Exchange (intercambiar posicion de dos piezas)
    IMPRISON,   // Ejecutando hechizo Imprison (inmovilizar pieza enemiga)
    REVIVE,     // Ejecutando hechizo Revive (resucitar pieza del cementerio)
    SUMMON,     // Ejecutando hechizo Summon (invocar elemental)
    GAME_OVER   // Fin del juego
};

class Magia;

class Tablero
{
    int casillasxlado;                              // Dimension del tablero (9)
    int tamanoCasilla;                              // Tamanio en pixeles de cada casilla (64) - usado en cementerio 2D
    PiezaTablero* cuadricula[9][9];                 // Matriz del tablero: nullptr = casilla vacia
    PiezaTablero* personaje_seleccionado{ nullptr }; // Pieza actualmente seleccionada por el jugador
    PiezaTablero* personaje_usando_magia{ nullptr }; // Mago que esta lanzando un hechizo (MH o Platero)
    PiezaTablero* personaje_auxiliar{ nullptr };     // Pieza auxiliar para Exchange (primera seleccion)
    std::vector<PiezaTablero*> cementerio_Luz;       // Piezas muertas del equipo Luz (para Revive)
    std::vector<PiezaTablero*> cementerio_Oscuridad; // Piezas muertas del equipo Oscuridad
    PiezaTablero* personaje_muerto_seleccionado{ nullptr }; // Pieza seleccionada del cementerio (Revive)
    int fila_seleccionada{ -1 };                    // Fila clickeada (-1 = ninguna)
    int columna_seleccionada{ -1 };                 // Columna clickeada (-1 = ninguna)
    bool turno{ LUZ };                              // Turno actual: LUZ o OSCURIDAD
    bool movimientosPosibles[9][9]{ false };        // Casillas a las que puede moverse la pieza seleccionada
    ColorCasilla colorCasilla[9][9];                // Color de cada casilla (blanco/negro/cambiante)
    int Ciclo{};                                    // Fase del ciclo de oscilacion (0-4) para casillas cambiantes
    bool avance{ false };                           // Direccion del ciclo: false=subiendo, true=bajando
    ModoJuego modoJuegoactual{ ModoJuego::NORMAL }; // Estado actual del juego
    Magia magiaTablero;                             // Sistema de hechizos
    std::string ganador{};                          // Ganador de la partida
    bool summonPendiente_{ false };                 // El combate es de un elemental invocado
    PiezaTablero* elemental_{ nullptr };            // Pieza temporal del elemental

    // --- Renderizado 3D del tablero ---
    static constexpr float cellSize3D = 3.0f;       // Tamanio de cada casilla en unidades 3D
    static constexpr float cellGap = 0.15f;          // Separacion entre casillas
    Camera3D camera3D = {
        { 0.0f, 25.0f, 32.0f },                     // Posicion: elevada y detras (similar a combate)
        { 0.0f, 0.0f, -3.0f },                      // Objetivo: centro del tablero
        { 0.0f, 1.0f, 0.0f },                       // Vector arriba
        45.0f, CAMERA_PERSPECTIVE                    // FOV y proyeccion
    };
    Shader alphaDiscard{};                           // Shader para transparencia de sprites

	// --- Posicion y dimension del cementerio 2D ---
    static constexpr int cementerioLuzX = 50;   // mismo valor que DrawT(50, 36)
    static constexpr int cementerioOscX = 850;  // mismo valor que DrawT(850, 36)
    static constexpr int cementerioY = 36;
    static constexpr int cementerioCelda = 64;    // tamaño de cada pieza en el cementerio

    // --- Fondo animado del tablero (frames extraidos de GIF) ---
    std::vector<Texture2D> fondoFrames{};
    int   fondoFrameActual{ 0 };
    float fondoTimer{ 0.0f };
    static constexpr float fondoFrameSpeed = 0.03f;  // 30ms por frame (26 frames)

    // --- Datos del combate pendiente ---
    // Se rellenan cuando una pieza intenta moverse a una casilla ocupada por el enemigo
    bool combatePendiente_{ false };                // Hay un combate por resolver
    PiezaTablero* atacante_{ nullptr };             // Pieza que inicio el ataque (se movio)
    PiezaTablero* defensor_{ nullptr };             // Pieza que fue atacada (estaba en la casilla)
    int filaOrigenAtacante_{ -1 };                  // Posicion original del atacante (para devolver si pierde)
    int colOrigenAtacante_{ -1 };

    // --- Animacion de movimiento pendiente ---
    PiezaTablero* piezaAnimando_{ nullptr };    // Pieza que se esta moviendo con animacion
    int filaDestinoAnim_{ -1 };                 // Destino de la animacion
    int colDestinoAnim_{ -1 };

    // --- Audio del tablero ---
    Music musicaInicio{};
    Music musicaFin{};
    bool reproduciendoInicio{ true };  // true = suena Inicio, false = suena Fin
    Sound sfxSelectPiece{};

    friend class Magia;     // Magia necesita acceso directo al tablero para ejecutar hechizos
    friend class Controlador_Tablero;

public:
    Tablero();
    ~Tablero();

    void LogicaTablero();                           // Ejecutar logica segun el modo de juego actual
    void Draw();                                    // Dibujar tablero, piezas e indicadores
    void inicializarTablero();                      // Colocar piezas en posicion inicial y asignar colores
    void seleccionaCasilla();                       // Detectar click del raton y convertir a fila/columna
    void cambioPosicionPieza(PiezaTablero* personaje, int fila, int columna);   // Mover pieza en la cuadricula
    void reset_seleccion();                         // Limpiar fila/columna seleccionada
    void moverPieza();                              // Logica de seleccion y movimiento de piezas
    void casillasPosibles(PiezaTablero* p);         // Calcular casillas validas de movimiento
    void DrawCasillas();                            // Dibujar resaltado de casillas de movimiento posible
    void set_MovimientosPosibles(bool set, int fila, int columna) { movimientosPosibles[fila][columna] = set; }
    bool get_MovimientosPosibles(int fila, int columna) { return movimientosPosibles[fila][columna]; }
    void reset_MovimientosPosibles();               // Limpiar todas las casillas de movimiento posible
    void set_colorCasilla(ColorCasilla color, int fil, int col) { colorCasilla[fil][col] = color; }
    void set_avance(bool aux) { avance = aux; }
    ColorCasilla get_colorCasilla(int fil, int col) { return colorCasilla[fil][col]; }
    void detectaGanador();                          // Comprobar condiciones de victoria
    void hechizos();                                // Menu de seleccion de hechizo (input por teclado)
    void avanceCiclo();                             // Avanzar el ciclo de oscilacion de casillas cambiantes
    ModoJuego get_modoJuegoActual() { return modoJuegoactual; }
    void iniciaEstadoHechizos();                    // Detectar tecla M para entrar al modo hechizos
  

    // --- Interfaz de combate (usada por raylibproject.cpp) ---
    bool combatePendiente() const { return combatePendiente_; }
    PiezaTablero* getAtacante() { return atacante_; }
    PiezaTablero* getDefensor() { return defensor_; }


    // Resuelve el combate: ganaAtacante=true si gana el que se movio
    // El perdedor va al cementerio, el ganador ocupa la casilla
    void resolverCombate(bool ganaAtacante, float vidaGanador);

    // Serialización del estado del tablero para guardado/carga
    std::string serializarEstado() const;
    void deserializarEstado(const std::string& data);
};
