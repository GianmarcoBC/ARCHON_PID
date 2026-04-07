#include "Menu_Combate.h"

bool Menu_Combate::Update() {
    // P1 navega con A/D, confirma con ESPACIO
    if (!P1Listo) {
        if (IsKeyPressed(KEY_D))     selP1 = (selP1 + 1) % 2;
        if (IsKeyPressed(KEY_A))     selP1 = (selP1 - 1 + 2) % 2;
        if (IsKeyPressed(KEY_SPACE)) P1Listo = true;
    }

    // P2 navega con izquierda/derecha, confirma con CTRL
    if (!P2Listo) {
        if (IsKeyPressed(KEY_RIGHT))         selP2 = (selP2 + 1) % 2;
        if (IsKeyPressed(KEY_LEFT))          selP2 = (selP2 - 1 + 2) % 2;
        if (IsKeyPressed(KEY_RIGHT_CONTROL)) P2Listo = true;
    }

    return P1Listo && P2Listo; // true = ambos listos, empieza el combate
}

void Menu_Combate::Draw() {
    ClearBackground(BLACK);
    DrawText("SELECCIONA TU PERSONAJE", 250, 50, 30, WHITE);

    // --- P1 ---
    DrawText("JUGADOR 1  (A/D + ESPACIO)", 100, 120, 20, BLUE);
    DrawTexture(*imagenes[selP1], 150, 200, WHITE);
    DrawText(nombres[selP1], 170, 350, 25, P1Listo ? GREEN : WHITE);
    if (P1Listo) DrawText("LISTO!", 170, 380, 20, GREEN);

    // --- P2 ---
    DrawText("JUGADOR 2  (FLECHAS + CTRL)", 550, 120, 20, RED);
    DrawTexture(*imagenes[selP2], 600, 200, WHITE);
    DrawText(nombres[selP2], 620, 350, 25, P2Listo ? GREEN : WHITE);
    if (P2Listo) DrawText("LISTO!", 620, 380, 20, GREEN);
}