#include "obstaculo.h"
#include "raylib.h"

void obstaculo::drawshadow()
{
    BeginBlendMode(BLEND_MULTIPLIED);
    DrawModel(shadow, shadowpos, 1.0f, WHITE);
    EndBlendMode();
}

void obstaculo::UnloadObstaculo()
{
    UnloadModel(shadow);
  
    UnloadTexture(sprite);
    UnloadTexture(spriteShadow);
}