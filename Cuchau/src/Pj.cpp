#include "Pj.h"

void Pj::drawshadow(Vector3 shadowpos)
{
    BeginBlendMode(BLEND_MULTIPLIED);
    DrawModel(shadow, shadowpos, 1.0f, WHITE);
    EndBlendMode();
}

void Pj::UnloadPj()
{
    UnloadModel(shadow);

    UnloadTexture(spriteShadow);
    UnloadTexture(spriteChar);
}