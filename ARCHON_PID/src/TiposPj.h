#pragma once

/*
 * TiposPj.h - Enumeraciones compartidas entre el sistema de tablero y el de combate
 *
 * Define los dos equipos (LUZ y OSCURIDAD) y los 16 tipos de personaje.
 * Usado por PiezaTablero (tablero) y Pj_info (combate) como identificador comun.
 */

// Equipos del juego: LUZ juega en el lado izquierdo, OSCURIDAD en el derecho
enum equipo { LUZ, OSCURIDAD };

// Identificador unico de cada tipo de personaje (compartido entre tablero y combate)
enum class tipo_pj {
    // Equipo LUZ (8 tipos)
    MH,             // Mago principal - lider del equipo Luz
    Phoenix,        // Ave fenix - volador rapido
    Golem,          // Golem - terrestre resistente
    Djinni,         // Genio - volador con rango
    Unicorn,        // Unicornio - terrestre con buen rango
    Valkyrie,       // Valkiria - voladora guerrera
    Archer,         // Arquero - terrestre a distancia
    Knight,         // Caballero - infanteria basica (peones)

    // Equipo OSCURIDAD (8 tipos)
    Platero,        // Hechicero oscuro - lider del equipo Oscuridad
    ShapeShifter,   // Metamorfo - copia al enemigo en combate
    Troll,          // Troll - terrestre resistente
    Dragon,         // Dragon - volador con rango
    Basilisk,       // Basilisco - terrestre venenoso
    Banshee,        // Banshee - voladora espectral
    Manticore,      // Manticora - terrestre fuerte
    Goblin          // Goblin - infanteria basica (peones)
};
