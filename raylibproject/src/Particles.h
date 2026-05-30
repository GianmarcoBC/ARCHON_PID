#pragma once
#include "GameState.h"

namespace Archon_PID {

    class Particles {
    public:
        static void init(GameState& gs);
        static void update(GameState& gs);
        static void drawStars(const GameState& gs);
    };

}