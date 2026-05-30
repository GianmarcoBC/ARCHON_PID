#pragma once

// ============================================================
// Archon_PID — manifiesto global del namespace
// Incluye este fichero cuando solo necesites saber QUÉ existe
// sin tirar de toda la cadena de includes.
// ============================================================
namespace Archon_PID {

    // --- Clases principales ---
    class App;
    class Screen;

    // --- Pantallas (legacy) ---
    class Screens;

    // --- Pantallas nuevas (heredan de Screen) ---
    class ModoJuegoScreen;
    class SeleccionModoScreen;
    class ConfigJuegoCompletoScreen;
    class SeleccionEquipoScreen;
    class ConfigDificultadScreen;
    class CargarPartidaScreen;
    class PausaScreen;

    // --- Subsistemas ---
    class Background;
    class Drawing;
    class Particles;
    class SaveSystem;
    class Tablero;
    class TableroAI;
    class TableroScreen;
    class CuchauScreen;
    class Controlador_Tablero;
    class Magia;
    class PiezaTablero;

    // --- IAs tablero ---
    class AI_Facil_Tablero;
    class AI_Agresiva_Tablero;
    class AI_Defensiva_Tablero;

    // --- Structs de datos (no necesitan forward decl, pero sí visibilidad) ---
    struct GameState;
    struct DatosTropa;
    struct DatosEquipo;
    struct Cancion;
    struct SaveData;
    struct PartidaGuardada;
    struct Particula;
    struct ParticulaFuego;
    struct ParticulaTeatro;
    struct ParticulaPolvo;
    struct ParticulaNota;

    // --- Enums ---
    enum Estado;
    enum ModoPartida;
    enum Bando;
    enum Dificultad;

} // namespace Archon_PID