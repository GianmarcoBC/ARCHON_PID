#include "obstaculo.h"

void obstaculo::drawshadow() const
{
    BeginBlendMode(BLEND_MULTIPLIED);
    DrawModel(shadow, shadowpos, 1.0f, WHITE);
    EndBlendMode();
}

void obstaculo::Draw(Camera camera) const
{
    drawshadow();
    DrawBillboard(camera, sprite, pos, size, WHITE);
}
