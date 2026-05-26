#include "Pj.h"

void Pj::drawshadow(Vector3 shadowpos)
{
    BeginBlendMode(BLEND_MULTIPLIED);
    for (int i = 0; i < 3; i++)
        DrawModel(shadow[i], shadowpos, 1.0f, WHITE);
    EndBlendMode();
}

void Pj::UnloadPj()
{
    for (int i = 0; i < 3; i++)
    {
        UnloadModel(shadow[i]);
        UnloadTexture(Frames_shadow[i]);
        UnloadTexture(Frames[i]);
    }
}