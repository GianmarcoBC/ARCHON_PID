#pragma once
#include "GameState.h"

namespace Archon_PID {

    class Background {
    public:
        static void castle(GameState& gs);
        static void battle(GameState& gs);
        static void library(GameState& gs);
        static void theater(GameState& gs);
        static void organ(GameState& gs);
    };

}