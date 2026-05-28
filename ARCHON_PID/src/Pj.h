#pragma once
#include "TiposPj.h"

/*
 * Pj.h - Plantillas de piezas del tablero
 *
 * Define los datos estaticos de cada tipo de pieza para el tablero:
 * sprite, tipo de movimiento (vuela/terrestre), equipo, rango.
 *
 * Estas constantes se usan en Tablero::inicializarTablero() para
 * crear las PiezaTablero con sus datos correctos.
 *
 * NOTA: Los sprites usan los frames "Still" del sistema de combate 3D
 * (ARCHON_PID_TECLA), ya que son los assets mas recientes.
 * Se escalan de 640x640 a 64x64 al dibujarse en el tablero.
 *
 * Las constantes estan en namespace pjboard para evitar conflictos
 * con las constantes PjC_* de combate definidas en cuchau/Pj_info.h.
 */

// Plantilla de pieza de tablero (solo datos de tablero, no de combate)
struct PjBoard
{
    const char* Sprite;             // Ruta al sprite para el tablero
    bool rango;                     // true = ataque a distancia
    bool vuela;                     // true = movimiento libre en area (vuelo)
    bool team;                      // LUZ (0) u OSCURIDAD (1)
    tipo_pj ID;                     // Identificador del tipo de personaje
    unsigned int rangoTablero;      // Casillas de alcance en el tablero
};

// ============================================================================
//  Constantes de piezas del tablero (namespace para evitar conflictos)
// ============================================================================
namespace pjboard {

// === EQUIPO LUZ ===
// Disposicion inicial: columna 0 (primera fila) y columna 1 (segunda fila)

inline constexpr PjBoard MH = {
    "Resources/MH/SpriteMHStill_0001.png",
    true, true, LUZ, tipo_pj::MH, 3                // Mago: vuela, rango 3
};

inline constexpr PjBoard Phoenix = {
    "Resources/Phoenix/SpritePhoenixStill_0001.png",
    false, true, LUZ, tipo_pj::Phoenix, 5           // Phoenix: vuela, rango 5 (mayor movilidad)
};

inline constexpr PjBoard Golem = {
    "Resources/Golem/SpriteGolemStill_0001.png",
    true, false, LUZ, tipo_pj::Golem, 3             // Golem: terrestre, rango 3
};

inline constexpr PjBoard Djinni = {
    "Resources/Djinni/SpriteDjinniStill_0001.png",
    true, true, LUZ, tipo_pj::Djinni, 4             // Djinni: vuela, rango 4
};

inline constexpr PjBoard Unicorn = {
    "Resources/Unicorn/SpriteUnicornStill_0001.png",
    true, false, LUZ, tipo_pj::Unicorn, 4           // Unicornio: terrestre, rango 4
};

inline constexpr PjBoard Valkyrie = {
    "Resources/Valkyrie/SpriteValkyrieStill_0001.png",
    true, true, LUZ, tipo_pj::Valkyrie, 3           // Valkiria: vuela, rango 3
};

inline constexpr PjBoard Archer = {
    "Resources/Archer/SpriteArcherStill_0001.png",
    true, false, LUZ, tipo_pj::Archer, 3            // Arquero: terrestre, rango 3
};

inline constexpr PjBoard Knight = {
    "Resources/Knight/SpriteKnightStill_0001.png",
    false, false, LUZ, tipo_pj::Knight, 3           // Caballero (peon): terrestre, rango 3
};

// === EQUIPO OSCURIDAD ===
// Disposicion inicial: columna 8 (ultima fila) y columna 7 (penultima)

inline constexpr PjBoard Platero = {
    "Resources/Platero/SpritePlateroStill_0001.png",
    true, true, OSCURIDAD, tipo_pj::Platero, 3      // Hechicero oscuro: vuela, rango 3
};

inline constexpr PjBoard ShapeShifter = {
    "Resources/ShapeShifter/SpriteShapeShifterStill_0001.png",
    false, true, OSCURIDAD, tipo_pj::ShapeShifter, 5 // Metamorfo: vuela, rango 5
};

inline constexpr PjBoard Troll = {
    "Resources/Troll/SpriteTrollStill_0001.png",
    true, false, OSCURIDAD, tipo_pj::Troll, 3       // Troll: terrestre, rango 3
};

inline constexpr PjBoard Dragon = {
    "Resources/Dragon/SpriteDragonStill_0001.png",
    true, true, OSCURIDAD, tipo_pj::Dragon, 4       // Dragon: vuela, rango 4
};

inline constexpr PjBoard Basilisk = {
    "Resources/Basilisk/SpriteBasiliskStill_0001.png",
    true, false, OSCURIDAD, tipo_pj::Basilisk, 3    // Basilisco: terrestre, rango 3
};

inline constexpr PjBoard Banshee = {
    "Resources/Banshee/SpriteBansheeStill_0001.png",
    true, true, OSCURIDAD, tipo_pj::Banshee, 3      // Banshee: vuela, rango 3
};

inline constexpr PjBoard Manticore = {
    "Resources/Manticore/SpriteManticoreStill_0001.png",
    true, false, OSCURIDAD, tipo_pj::Manticore, 3   // Manticora: terrestre, rango 3
};

inline constexpr PjBoard Goblin = {
    "Resources/Goblin/SpriteGoblinStill_0001.png",
    false, false, OSCURIDAD, tipo_pj::Goblin, 3     // Goblin (peon): terrestre, rango 3
};

} // namespace pjboard
