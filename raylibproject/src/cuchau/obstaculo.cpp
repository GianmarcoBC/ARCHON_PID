#include "obstaculo.h"
#include "raylib.h"

namespace Archon_PID {

    //  obstaculo::drawshadow — Dibuja la sombra del obstaculo en el suelo
    //
    //  Usa BLEND_MULTIPLIED para que la sombra oscurezca el suelo sin tapar
    //  completamente lo que hay debajo (efecto de transparencia multiplicativa).

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

} // namespace Archon_PID