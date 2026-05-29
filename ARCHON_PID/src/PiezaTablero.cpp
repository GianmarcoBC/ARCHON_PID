#include "PiezaTablero.h"

void PiezaTablero::UpdateAnimacion(float dt)
{
    if (Frames_STILL.empty()) return;
    frameTimer += dt;
    if (frameTimer >= frameSpeed) {
        frameTimer = 0.0f;
        frameActual = (frameActual + 1) % (int)Frames_STILL.size();
    }
}

void PiezaTablero::iniciarMovimiento(int filaDestino, int colDestino, float cellSize)
{
    enMovimiento = true;
    movProgreso = 0.0f;
    movOrigenX = (columna - 4.0f) * cellSize;
    movOrigenZ = (fila - 4.0f) * cellSize;
    movDestinoX = (colDestino - 4.0f) * cellSize;
    movDestinoZ = (filaDestino - 4.0f) * cellSize;
    movX = movOrigenX;
    movZ = movOrigenZ;
    frameActual_Mov = 0;
    frameTimer_Mov = 0.0f;
}

bool PiezaTablero::UpdateMovimiento(float dt)
{
    if (!enMovimiento) return true;

    // Calcular distancia total y avanzar progreso
    float dx = movDestinoX - movOrigenX;
    float dz = movDestinoZ - movOrigenZ;
    float dist = sqrtf(dx * dx + dz * dz);
    if (dist < 0.001f) { enMovimiento = false; return true; }

    movProgreso += (movVelocidad / dist) * dt;
    if (movProgreso >= 1.0f) {
        movProgreso = 1.0f;
        enMovimiento = false;
    }

    movX = movOrigenX + dx * movProgreso;
    movZ = movOrigenZ + dz * movProgreso;

    // Avanzar frame Mov
    if (!Frames_MOV.empty()) {
        frameTimer_Mov += dt;
        if (frameTimer_Mov >= frameSpeed_Mov) {
            frameTimer_Mov = 0.0f;
            frameActual_Mov = (frameActual_Mov + 1) % (int)Frames_MOV.size();
        }
    }

    return !enMovimiento; // true si termino
}

/*
 * DrawT - Dibuja la pieza en 2D (usado para el cementerio en Revive)
 * Usa el primer frame Still escalado a 64x64
 */
void PiezaTablero::DrawT(const int paramx, const int paramy)
{
    if (Frames_STILL.empty()) return;
    const Texture2D& tex = Frames_STILL[0];
    float w = (float)tex.width;
    float h = (float)tex.height;
    float size = 64.0f;

    pos.x = (float)(paramx + columna * 64);
    pos.y = (float)(paramy + fila * 64);

    DrawTexturePro(
        tex,
        { 0, 0, w, h },
        { pos.x + size / 2, pos.y + size / 2, size, size },
        { size / 2, size / 2 },
        0.0f, WHITE);
}

/*
 * Draw3D - Dibuja la pieza como billboard animado
 * Replica exactamente Personaje::drawAnimation
 */
void PiezaTablero::Draw3D(Camera camera, float cellSize)
{
    if (Frames_STILL.empty()) return;

    float worldX, worldZ;
    const Texture2D* tex;

    if (enMovimiento && !Frames_MOV.empty()) {
        // Durante movimiento: usar posicion interpolada y frames Mov
        worldX = movX;
        worldZ = movZ;
        tex = &Frames_MOV[frameActual_Mov];
    } else {
        // Estatico: usar posicion de la cuadricula y frames Still/Res
        worldX = (columna - 4.0f) * cellSize;
        worldZ = (fila - 4.0f) * cellSize;
        tex = (seleccionado && !Frames_RES.empty())
            ? &Frames_RES[frameActual] : &Frames_STILL[frameActual];
    }

    Vector3 worldPos = { worldX, pieceSize3D / 2.0f + 0.08f, worldZ };

    float w = (float)tex->width;
    float h = (float)tex->height;
    // Invertir sprite horizontalmente para el equipo oscuro (que mire hacia dentro)
    float srcW = team ? -w : w;

    DrawBillboardPro(camera, *tex,
        { 0, 0, srcW, h }, worldPos,
        { 0, 1, 0 }, { pieceSize3D, pieceSize3D }, { pieceSize3D / 2, pieceSize3D / 2 },
        0.0f, WHITE);
}

/*
 * DrawShadow3D - Dibuja la sombra animada en el suelo
 * Replica exactamente Personaje::drawshadow
 */
void PiezaTablero::DrawShadow3D(float cellSize)
{
    float worldX, worldZ;
    Model* shadowModel;

    if (enMovimiento && !shadow_MOV.empty()) {
        worldX = movX;
        worldZ = movZ;
        shadowModel = &shadow_MOV[frameActual_Mov];
    } else {
        if (shadow_STILL.empty()) return;
        worldX = (columna - 4.0f) * cellSize;
        worldZ = (fila - 4.0f) * cellSize;
        shadowModel = &shadow_STILL[frameActual];
    }

    Vector3 shadowPos = { worldX, 0.10f, worldZ - pieceSize3D / 2.0f };

    BeginBlendMode(BLEND_MULTIPLIED);
    DrawModel(*shadowModel, shadowPos, 1.0f, WHITE);
    EndBlendMode();
}
