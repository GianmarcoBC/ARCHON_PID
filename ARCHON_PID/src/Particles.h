#pragma once
#include "GameState.h"

class Particles {
public:
    static void init(GameState& gs);
    static void update(GameState& gs);
    static void drawStars(const GameState& gs);
};
