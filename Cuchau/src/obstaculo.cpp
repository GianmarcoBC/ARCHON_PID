#include "obstaculo.h"
#include "raylib.h"

// ============================================================================
//  obstaculo::drawshadow — Dibuja la sombra del obstaculo en el suelo
//
//  Usa BLEND_MULTIPLIED para que la sombra oscurezca el suelo sin tapar
//  completamente lo que hay debajo (efecto de transparencia multiplicativa).
// ============================================================================

void obstaculo::drawshadow()
{
    BeginBlendMode(BLEND_MULTIPLIED);
    DrawModel(shadow, shadowpos, 1.0f, WHITE);
    EndBlendMode();
}

// ============================================================================
//  obstaculo::UnloadObstaculo — Libera recursos de GPU
//
//  Descarga el modelo de sombra y las texturas del sprite y la sombra.
//  Se llama en el destructor de arena al terminar un combate.
// ============================================================================

void obstaculo::UnloadObstaculo()
{
    UnloadModel(shadow);
    UnloadTexture(sprite);
    UnloadTexture(spriteShadow);
}
