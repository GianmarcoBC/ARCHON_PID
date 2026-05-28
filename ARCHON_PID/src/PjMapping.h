#pragma once
#include "TiposPj.h"
#include "cuchau/Pj_info.h"

/*
 * PjMapping.h - Puente entre el sistema de tablero y el de combate
 *
 * Cuando dos piezas se enfrentan en el tablero, necesitamos obtener
 * los datos de combate (sprites 3D, stats, IA) a partir del tipo_pj
 * de cada pieza. Esta funcion realiza ese mapeo.
 *
 * Las constantes PjC_* estan definidas en cuchau/Pj_info.h con el
 * prefijo "PjC_" para evitar conflictos con las constantes del tablero.
 */

// Mapea un tipo de pieza del tablero a su Pj_info de combate 3D
inline const Pj_info& getCombatInfo(tipo_pj tipo) {
    switch (tipo) {
    case tipo_pj::MH:           return PjC_MH;
    case tipo_pj::Phoenix:      return PjC_Phoenix;
    case tipo_pj::Golem:        return PjC_Golem;
    case tipo_pj::Djinni:       return PjC_Djinni;
    case tipo_pj::Unicorn:      return PjC_Unicorn;
    case tipo_pj::Valkyrie:     return PjC_Valkyrie;
    case tipo_pj::Archer:       return PjC_Archer;
    case tipo_pj::Knight:       return PjC_Knight;
    case tipo_pj::Platero:      return PjC_Platero;
    case tipo_pj::ShapeShifter: return PjC_ShapeShifter;
    case tipo_pj::Troll:        return PjC_Troll;
    case tipo_pj::Dragon:       return PjC_Dragon;
    case tipo_pj::Basilisk:     return PjC_Basilisk;
    case tipo_pj::Banshee:      return PjC_Banshee;
    case tipo_pj::Manticore:    return PjC_Manticore;
    case tipo_pj::Goblin:       return PjC_Goblin;
    default:                    return PjC_Knight;
    }
}
