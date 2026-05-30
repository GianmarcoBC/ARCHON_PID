#pragma once
#include "CombatAI.h"

namespace Archon_PID {

    class AI_Agresiva : public CombatAI {
        Accion decide(const std::vector<Disparo>& disparos, float dt) override;
    public:
        AI_Agresiva(Personaje& IA, Personaje& Jugador, Rectangle arena);
    };

} // namespace Archon_PID