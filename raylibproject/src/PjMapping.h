#pragma once
#include "TiposPj.h"
#include "Pj.h"
#include "Pj_info.h"

// Mapea un tipo_pj a su PjBoard (datos de tablero)
inline PjBoard getBoardPj(tipo_pj tipo) {
    switch (tipo) {
    case tipo_pj::MH:           return pjboard::MH;
    case tipo_pj::Phoenix:      return pjboard::Phoenix;
    case tipo_pj::Golem:        return pjboard::Golem;
    case tipo_pj::Djinni:       return pjboard::Djinni;
    case tipo_pj::Unicorn:      return pjboard::Unicorn;
    case tipo_pj::Valkyrie:     return pjboard::Valkyrie;
    case tipo_pj::Archer:       return pjboard::Archer;
    case tipo_pj::Knight:       return pjboard::Knight;
    case tipo_pj::Platero:      return pjboard::Platero;
    case tipo_pj::ShapeShifter: return pjboard::ShapeShifter;
    case tipo_pj::Troll:        return pjboard::Troll;
    case tipo_pj::Dragon:       return pjboard::Dragon;
    case tipo_pj::Basilisk:     return pjboard::Basilisk;
    case tipo_pj::Banshee:      return pjboard::Banshee;
    case tipo_pj::Manticore:    return pjboard::Manticore;
    case tipo_pj::Goblin:       return pjboard::Goblin;
    case tipo_pj::Elemental:    return pjboard::Elemental;
    default:                    return pjboard::Knight;
    }
}

// Mapea un tipo de pieza del tablero a su Pj_info de combate 3D
inline const Pj_info& getCombatInfo(tipo_pj tipo) {
    switch (tipo) {
    case tipo_pj::MH:           return MH;
    case tipo_pj::Phoenix:      return Phoenix;
    case tipo_pj::Golem:        return Golem;
    case tipo_pj::Djinni:       return Djinni;
    case tipo_pj::Unicorn:      return Unicorn;
    case tipo_pj::Valkyrie:     return Valkyrie;
    case tipo_pj::Archer:       return Archer;
    case tipo_pj::Knight:       return Knight;
    case tipo_pj::Platero:      return Platero;
    case tipo_pj::ShapeShifter: return ShapeShifter;
    case tipo_pj::Troll:        return Troll;
    case tipo_pj::Dragon:       return Dragon;
    case tipo_pj::Basilisk:     return Basilisk;
    case tipo_pj::Banshee:      return Banshee;
    case tipo_pj::Manticore:    return Manticore;
    case tipo_pj::Goblin:       return Goblin;
    case tipo_pj::Elemental:    return MH;
    default:                    return Knight;
    }
}
