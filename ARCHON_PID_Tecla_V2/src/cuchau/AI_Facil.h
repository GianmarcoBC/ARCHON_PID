#pragma once
#include "CombatAI.h"

class AI_Facil : public CombatAI {
    Accion decide(const std::vector<Disparo>& disparos, float dt) override;
public:
    AI_Facil(Personaje& IA, Personaje& Jugador, Rectangle arena);
};


