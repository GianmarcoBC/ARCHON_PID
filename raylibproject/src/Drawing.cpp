#include "Drawing.h"
#include "rlgl.h"
#include "raylib.h"

void Drawing::setupProjection() {
    // Defensive check: skip rlgl calls if window/context not ready or screen size is zero.
    // Calling rlDrawRenderBatchActive() with an uninitialized rlgl can crash (access violation).
    if (!IsWindowReady() || GetScreenWidth() <= 0 || GetScreenHeight() <= 0) {
        TraceLog(LOG_WARNING, "Drawing::setupProjection skipped: window/context not ready or zero size");
        return;
    }

    rlDrawRenderBatchActive();
    rlMatrixMode(RL_PROJECTION);
    rlLoadIdentity();
    rlOrtho(0, 800, 0, 600, -1, 1);
    rlMatrixMode(RL_MODELVIEW);
    rlLoadIdentity();
}

void Drawing::texto12(float x, float y, const std::string& t, Color color) {
    rlPushMatrix();
    rlTranslatef(x, y + 12, 0);
    rlScalef(1.0f, -1.0f, 1.0f);
    DrawText(t.c_str(), 0, 0, 12, color);
    rlPopMatrix();
}

void Drawing::texto18(float x, float y, const std::string& t, Color color) {
    rlPushMatrix();
    rlTranslatef(x, y + 18, 0);
    rlScalef(1.0f, -1.0f, 1.0f);
    DrawText(t.c_str(), 0, 0, 18, color);
    rlPopMatrix();
}

void Drawing::texto24(float x, float y, const std::string& t, Color color) {
    rlPushMatrix();
    rlTranslatef(x, y + 24, 0);
    rlScalef(1.0f, -1.0f, 1.0f);
    DrawText(t.c_str(), 0, 0, 24, color);
    rlPopMatrix();
}

void Drawing::instrucciones(float x, float y, const std::string& t, float tiempo) {
    float al = 0.5f + 0.5f * sinf(tiempo * 0.007f);
    texto12(x, y, t, CFloat(al * 0.7f, al * 0.55f, al * 0.3f));
}

void Drawing::circulo(float x, float y, float r, int seg, Color color) {
    rlBegin(RL_TRIANGLES);
    rlColor4ub(color.r, color.g, color.b, color.a);
    for (int i = 0; i < seg; i++) {
        float a1 = i * 2.0f * M_PI / seg;
        float a2 = (i + 1) * 2.0f * M_PI / seg;
        rlVertex2f(x, y);
        rlVertex2f(x + cosf(a1) * r, y + sinf(a1) * r);
        rlVertex2f(x + cosf(a2) * r, y + sinf(a2) * r);
    }
    rlEnd();
}

void Drawing::circuloLinea(float x, float y, float r, int seg, Color color) {
    rlBegin(RL_LINES);
    rlColor4ub(color.r, color.g, color.b, color.a);
    for (int i = 0; i < seg; i++) {
        float a1 = i * 2.0f * M_PI / seg;
        float a2 = (i + 1) * 2.0f * M_PI / seg;
        rlVertex2f(x + cosf(a1) * r, y + sinf(a1) * r);
        rlVertex2f(x + cosf(a2) * r, y + sinf(a2) * r);
    }
    rlEnd();
}

void Drawing::antorcha(float x, float y, float tiempo) {
    float fuego = sinf(tiempo * 0.03f) * 6;
    rlBegin(RL_QUADS);
    rlColor4f(0.3f, 0.15f, 0.0f, 1.0f);
    rlVertex2f(x - 4, y); rlVertex2f(x + 4, y);
    rlVertex2f(x + 4, y + 25); rlVertex2f(x - 4, y + 25);
    rlEnd();
    rlBegin(RL_TRIANGLES);
    rlColor4f(1, 0.5f, 0, 1);
    rlVertex2f(x, y + 40 + fuego); rlVertex2f(x - 8, y + 25); rlVertex2f(x + 8, y + 25);
    rlColor4f(1, 1, 0, 1);
    rlVertex2f(x, y + 35 + fuego); rlVertex2f(x - 5, y + 25); rlVertex2f(x + 5, y + 25);
    rlEnd();
    for (int i = 0; i < 6; i++) {
        circulo(x + (rand() % 12) - 6.0f, y + 25 + fuego + (rand() % 12) - 6.0f,
                1.0f, 6, CFloat(1, 0.7f, 0.3f));
    }
}

void Drawing::antorchaGrande(float x, float y, float tiempo) {
    float fuego = sinf(tiempo * 0.04f) * 10;
    rlBegin(RL_QUADS);
    rlColor4f(0.4f, 0.2f, 0.05f, 1);
    rlVertex2f(x - 6, y); rlVertex2f(x + 6, y);
    rlVertex2f(x + 6, y + 35); rlVertex2f(x - 6, y + 35);
    rlColor4f(0.4f, 0.2f, 0.05f, 1);
    rlVertex2f(x - 14, y + 30); rlVertex2f(x + 14, y + 30);
    rlVertex2f(x + 14, y + 36); rlVertex2f(x - 14, y + 36);
    rlEnd();
    // Outer flame
    rlBegin(RL_TRIANGLES);
    rlColor4f(1, 0.4f, 0, 1);
    for (int i = 0; i < 12; i++) {
        float a1 = i * 2.0f * M_PI / 12;
        float a2 = (i + 1) * 2.0f * M_PI / 12;
        rlVertex2f(x, y + 55 + fuego);
        rlVertex2f(x + cosf(a1) * 12, y + 35 + sinf(a1) * 5);
        rlVertex2f(x + cosf(a2) * 12, y + 35 + sinf(a2) * 5);
    }
    // Inner flame
    rlColor4f(1, 1, 0.3f, 1);
    for (int i = 0; i < 12; i++) {
        float a1 = i * 2.0f * M_PI / 12;
        float a2 = (i + 1) * 2.0f * M_PI / 12;
        rlVertex2f(x, y + 48 + fuego);
        rlVertex2f(x + cosf(a1) * 7, y + 35 + sinf(a1) * 4);
        rlVertex2f(x + cosf(a2) * 7, y + 35 + sinf(a2) * 4);
    }
    rlEnd();
    for (int i = 0; i < 10; i++) {
        circulo(x + (rand() % 20) - 10.0f, y + 35 + fuego + (float)(rand() % 20),
                1.2f, 6, CFloat(1, 0.6f + (rand() % 4) / 10.0f, 0));
    }
}

void Drawing::logo(float tiempo) {
    float esc = 1.0f + sinf(tiempo * 0.008f) * 0.04f;
    rlPushMatrix();
    rlTranslatef(400 + sinf(tiempo * 0.003f) * 15, 520 + sinf(tiempo * 0.004f) * 5, 0);
    rlScalef(esc, esc, 1);
    rlBegin(RL_QUADS);
    rlColor4f(0.9f, 0.8f, 0.2f, 1);
    rlVertex2f(-120, -20); rlVertex2f(120, -20);
    rlVertex2f(120, 20); rlVertex2f(-120, 20);
    rlEnd();
    rlSetLineWidth(3);
    rlBegin(RL_LINES);
    rlColor4f(0.2f, 0.1f, 0.0f, 1);
    rlVertex2f(-120, -20); rlVertex2f(120, -20);
    rlVertex2f(120, -20); rlVertex2f(120, 20);
    rlVertex2f(120, 20); rlVertex2f(-120, 20);
    rlVertex2f(-120, 20); rlVertex2f(-120, -20);
    rlEnd();
    texto24(-55, -8, "ARCHON", CFloat(0.1f, 0.05f, 0.0f));
    rlPopMatrix();
}

void Drawing::cursorAnimado(float x, float y, float tiempo) {
    float s = 10 + sinf(tiempo * 0.008f) * 3;
    float rot = sinf(tiempo * 0.008f) * 15;
    rlPushMatrix();
    rlTranslatef(x, y, 0);
    rlRotatef(rot, 0, 0, 1);
    rlBegin(RL_TRIANGLES);
    rlColor4f(1, 1, 1, 1);
    rlVertex2f(0, 0); rlVertex2f(-s, s); rlVertex2f(-s, -s);
    rlEnd();
    rlPopMatrix();
}

void Drawing::barraSlider(float x, float y, float w, int valor, int vmax, bool esBool) {
    rlBegin(RL_QUADS);
    rlColor4f(0.1f, 0.05f, 0.05f, 1);
    rlVertex2f(x, y - 5); rlVertex2f(x + w, y - 5);
    rlVertex2f(x + w, y + 5); rlVertex2f(x, y + 5);
    rlEnd();
    float fill = (float)valor / vmax * w;
    float r2, g2, b2;
    if (esBool) { r2 = valor ? 0.1f : 0.4f; g2 = valor ? 0.6f : 0.1f; b2 = 0.05f; }
    else { float t = (float)valor / vmax; r2 = 0.6f + t * 0.3f; g2 = 0.2f + t * 0.3f; b2 = 0.05f; }
    rlBegin(RL_QUADS);
    rlColor4f(r2, g2, b2, 1);
    rlVertex2f(x, y - 4); rlVertex2f(x + fill, y - 4);
    rlVertex2f(x + fill, y + 4); rlVertex2f(x, y + 4);
    rlEnd();
    rlBegin(RL_LINES);
    rlColor4f(0.6f, 0.45f, 0.08f, 1);
    rlVertex2f(x, y - 5); rlVertex2f(x + w, y - 5);
    rlVertex2f(x + w, y - 5); rlVertex2f(x + w, y + 5);
    rlVertex2f(x + w, y + 5); rlVertex2f(x, y + 5);
    rlVertex2f(x, y + 5); rlVertex2f(x, y - 5);
    rlEnd();
    rlBegin(RL_QUADS);
    rlColor4f(0.9f, 0.8f, 0.3f, 1);
    rlVertex2f(x + fill - 3, y - 7); rlVertex2f(x + fill + 3, y - 7);
    rlVertex2f(x + fill + 3, y + 7); rlVertex2f(x + fill - 3, y + 7);
    rlEnd();
}

void Drawing::botonControl(float cx, float cy, float r, bool activo, float pr, float pg, float pb) {
    circulo(cx, cy, r, 20, CFloat(pr * 0.3f, pg * 0.3f, pb * 0.3f, 0.7f));
    Color lc = activo ? CFloat(pr, pg, pb) : CFloat(pr * 0.6f, pg * 0.6f, pb * 0.6f);
    rlSetLineWidth(activo ? 2.5f : 1.5f);
    circuloLinea(cx, cy, r, 20, lc);
}

void Drawing::panelMenu(float y) {
    rlBegin(RL_QUADS);
    rlColor4f(0.8f, 0.0f, 0.0f, 1);
    rlVertex2f(250, y + 25); rlVertex2f(600, y + 25);
    rlVertex2f(640, y - 25); rlVertex2f(290, y - 25);
    rlEnd();
}

void Drawing::panelBatalla(float y, bool sel) {
    if (sel) {
        rlBegin(RL_QUADS);
        rlColor4f(0.4f, 0.0f, 0.0f, 1);
        rlVertex2f(155, y + 30); rlVertex2f(660, y + 30);
        rlVertex2f(700, y - 30); rlVertex2f(195, y - 30);
        rlColor4f(0.85f, 0.05f, 0.05f, 1);
        rlVertex2f(150, y + 28); rlVertex2f(655, y + 28);
        rlVertex2f(695, y - 28); rlVertex2f(190, y - 28);
        rlColor4f(1.0f, 0.3f, 0.3f, 1);
        rlVertex2f(150, y + 20); rlVertex2f(655, y + 20);
        rlVertex2f(655, y + 28); rlVertex2f(150, y + 28);
        rlEnd();
    } else {
        rlBegin(RL_QUADS);
        rlColor4f(0.15f, 0.05f, 0.05f, 1);
        rlVertex2f(150, y + 25); rlVertex2f(655, y + 25);
        rlVertex2f(695, y - 25); rlVertex2f(190, y - 25);
        rlEnd();
    }
}

void Drawing::iconoEspada(float x, float y, Color color) {
    rlSetLineWidth(1.5f);
    rlBegin(RL_LINES);
    rlColor4ub(color.r, color.g, color.b, color.a);
    rlVertex2f(x, y + 10); rlVertex2f(x, y - 10);
    rlVertex2f(x - 5, y - 5); rlVertex2f(x + 5, y - 5);
    rlEnd();
}

void Drawing::iconoOpciones(float x, float y, Color color) {
    circuloLinea(x, y, 8, 20, color);
    rlBegin(RL_LINES);
    rlColor4ub(color.r, color.g, color.b, color.a);
    for (int i = 0; i < 8; i++) {
        float a = i * 2.0f * M_PI / 8;
        rlVertex2f(x + cosf(a) * 8, y + sinf(a) * 8);
        rlVertex2f(x + cosf(a) * 11, y + sinf(a) * 11);
    }
    rlEnd();
}

void Drawing::iconoLibro(float x, float y, Color color) {
    rlBegin(RL_LINES);
    rlColor4ub(color.r, color.g, color.b, color.a);
    rlVertex2f(x - 8, y + 8); rlVertex2f(x + 8, y + 8);
    rlVertex2f(x + 8, y + 8); rlVertex2f(x + 8, y - 8);
    rlVertex2f(x + 8, y - 8); rlVertex2f(x - 8, y - 8);
    rlVertex2f(x - 8, y - 8); rlVertex2f(x - 8, y + 8);
    rlVertex2f(x, y + 8); rlVertex2f(x, y - 8);
    rlEnd();
}

void Drawing::iconoEscudos(float x, float y, Color color) {
    rlBegin(RL_LINES);
    rlColor4ub(color.r, color.g, color.b, color.a);
    // Left shield
    rlVertex2f(x - 10, y + 8); rlVertex2f(x - 4, y + 8);
    rlVertex2f(x - 4, y + 8); rlVertex2f(x - 4, y);
    rlVertex2f(x - 4, y); rlVertex2f(x - 7, y - 6);
    rlVertex2f(x - 7, y - 6); rlVertex2f(x - 10, y);
    rlVertex2f(x - 10, y); rlVertex2f(x - 10, y + 8);
    // Right shield
    rlVertex2f(x + 4, y + 8); rlVertex2f(x + 10, y + 8);
    rlVertex2f(x + 10, y + 8); rlVertex2f(x + 10, y);
    rlVertex2f(x + 10, y); rlVertex2f(x + 7, y - 6);
    rlVertex2f(x + 7, y - 6); rlVertex2f(x + 4, y);
    rlVertex2f(x + 4, y); rlVertex2f(x + 4, y + 8);
    rlEnd();
}

void Drawing::iconoIA(float x, float y, Color color) {
    circuloLinea(x, y, 8, 20, color);
    rlBegin(RL_LINES);
    rlColor4ub(color.r, color.g, color.b, color.a);
    rlVertex2f(x - 4, y); rlVertex2f(x + 4, y);
    rlVertex2f(x, y - 4); rlVertex2f(x, y + 4);
    for (int i = 0; i < 8; i++) {
        float a = i * 2.0f * M_PI / 8;
        rlVertex2f(x + cosf(a) * 8, y + sinf(a) * 8);
        rlVertex2f(x + cosf(a) * 11, y + sinf(a) * 11);
    }
    rlEnd();
}

void Drawing::iconoNota(float x, float y, Color color) {
    circulo(x, y, 4, 8, color);
    rlSetLineWidth(1.5f);
    rlBegin(RL_LINES);
    rlColor4ub(color.r, color.g, color.b, color.a);
    rlVertex2f(x + 3.2f, y); rlVertex2f(x + 3.2f, y + 10);
    rlVertex2f(x + 3.2f, y + 10); rlVertex2f(x + 9, y + 7);
    rlEnd();
}

// ============================================================
// SPRITES
// ============================================================
void Drawing::spriteCaballero(float x, float y, float esc, float tiempo) {
    rlPushMatrix(); rlTranslatef(x, y, 0); rlScalef(esc, esc, 1);
    // Body
    rlBegin(RL_QUADS);
    rlColor4f(0.7f, 0.75f, 0.8f, 1);
    rlVertex2f(-12, -20); rlVertex2f(12, -20); rlVertex2f(12, 10); rlVertex2f(-12, 10);
    rlEnd();
    circulo(0, 18, 10, 16, CFloat(0.6f, 0.65f, 0.7f));
    // Visor
    rlBegin(RL_QUADS);
    rlColor4f(0.4f, 0.4f, 0.45f, 1);
    rlVertex2f(-7, 14); rlVertex2f(7, 14); rlVertex2f(6, 10); rlVertex2f(-6, 10);
    // Shield
    rlColor4f(0.8f, 0.1f, 0.1f, 1);
    rlVertex2f(-18, -5); rlVertex2f(-10, -5); rlVertex2f(-10, 8); rlVertex2f(-18, 8);
    rlEnd();
    rlBegin(RL_LINES);
    rlColor4f(1, 0.9f, 0.1f, 1);
    rlVertex2f(-18, 1); rlVertex2f(-10, 1);
    rlEnd();
    // Sword
    rlBegin(RL_QUADS);
    rlColor4f(0.85f, 0.85f, 0.9f, 1);
    rlVertex2f(10, -5); rlVertex2f(13, -5); rlVertex2f(13, 20); rlVertex2f(10, 20);
    // Legs
    rlColor4f(0.55f, 0.6f, 0.65f, 1);
    rlVertex2f(-10, -35); rlVertex2f(-2, -35); rlVertex2f(-2, -20); rlVertex2f(-10, -20);
    rlVertex2f(2, -35); rlVertex2f(10, -35); rlVertex2f(10, -20); rlVertex2f(2, -20);
    rlEnd();
    rlPopMatrix();
}

void Drawing::spriteArquero(float x, float y, float esc, float tiempo) {
    rlPushMatrix(); rlTranslatef(x, y, 0); rlScalef(esc, esc, 1);
    rlBegin(RL_QUADS);
    rlColor4f(0.4f, 0.25f, 0.1f, 1);
    rlVertex2f(-10, -20); rlVertex2f(10, -20); rlVertex2f(8, 8); rlVertex2f(-8, 8);
    rlEnd();
    circulo(0, 16, 9, 16, CFloat(0.75f, 0.6f, 0.45f));
    rlBegin(RL_TRIANGLES);
    rlColor4f(0.25f, 0.15f, 0.05f, 1);
    rlVertex2f(0, 28); rlVertex2f(-9, 14); rlVertex2f(9, 14);
    rlEnd();
    // Bow
    rlSetLineWidth(2);
    rlBegin(RL_LINES);
    rlColor4f(0.55f, 0.35f, 0.1f, 1);
    for (int i = 0; i < 12; i++) {
        float a1 = (float)i / 12 * M_PI - M_PI / 2;
        float a2 = (float)(i + 1) / 12 * M_PI - M_PI / 2;
        rlVertex2f(16 + cosf(a1) * 14, sinf(a1) * 18);
        rlVertex2f(16 + cosf(a2) * 14, sinf(a2) * 18);
    }
    // String
    rlColor4f(0.9f, 0.85f, 0.7f, 1);
    rlVertex2f(16, -18); rlVertex2f(16, 18);
    // Arrow
    rlColor4f(0.6f, 0.4f, 0.15f, 1);
    rlVertex2f(-10, 2); rlVertex2f(16, 2);
    rlEnd();
    rlBegin(RL_TRIANGLES);
    rlColor4f(0.7f, 0.1f, 0.1f, 1);
    rlVertex2f(-10, 2); rlVertex2f(-5, 5); rlVertex2f(-5, -1);
    rlEnd();
    rlBegin(RL_QUADS);
    rlColor4f(0.3f, 0.2f, 0.08f, 1);
    rlVertex2f(-8, -35); rlVertex2f(-2, -35); rlVertex2f(-2, -20); rlVertex2f(-8, -20);
    rlVertex2f(2, -35); rlVertex2f(8, -35); rlVertex2f(8, -20); rlVertex2f(2, -20);
    rlEnd();
    rlPopMatrix();
}

void Drawing::spriteOrco(float x, float y, float esc, float tiempo) {
    rlPushMatrix(); rlTranslatef(x, y, 0); rlScalef(esc, esc, 1);
    rlBegin(RL_QUADS);
    rlColor4f(0.2f, 0.35f, 0.1f, 1);
    rlVertex2f(-16, -22); rlVertex2f(16, -22); rlVertex2f(14, 12); rlVertex2f(-14, 12);
    rlEnd();
    circulo(0, 22, 13, 16, CFloat(0.25f, 0.4f, 0.12f));
    // Tusks
    rlBegin(RL_TRIANGLES);
    rlColor4f(0.9f, 0.85f, 0.7f, 1);
    rlVertex2f(-5, 15); rlVertex2f(-3, 15); rlVertex2f(-4, 9);
    rlVertex2f(3, 15); rlVertex2f(5, 15); rlVertex2f(4, 9);
    rlEnd();
    // Eyes
    circulo(-5, 25, 3, 8, CFloat(0.9f, 0.1f, 0.05f));
    circulo(5, 25, 3, 8, CFloat(0.9f, 0.1f, 0.05f));
    // Weapon
    rlBegin(RL_QUADS);
    rlColor4f(0.45f, 0.3f, 0.1f, 1);
    rlVertex2f(14, -10); rlVertex2f(17, -10); rlVertex2f(17, 25); rlVertex2f(14, 25);
    rlEnd();
    circulo(15.5f, 27, 6, 10, CFloat(0.5f, 0.5f, 0.5f));
    // Legs
    rlBegin(RL_QUADS);
    rlColor4f(0.18f, 0.28f, 0.08f, 1);
    rlVertex2f(-14, -38); rlVertex2f(-4, -38); rlVertex2f(-4, -22); rlVertex2f(-14, -22);
    rlVertex2f(4, -38); rlVertex2f(14, -38); rlVertex2f(14, -22); rlVertex2f(4, -22);
    rlEnd();
    rlPopMatrix();
}

void Drawing::spriteNigromante(float x, float y, float esc, float tiempo) {
    rlPushMatrix(); rlTranslatef(x, y, 0); rlScalef(esc, esc, 1);
    rlBegin(RL_TRIANGLES);
    rlColor4f(0.1f, 0.05f, 0.15f, 1);
    rlVertex2f(-14, -28); rlVertex2f(14, -28); rlVertex2f(0, 14);
    rlEnd();
    rlBegin(RL_QUADS);
    rlColor4f(0.1f, 0.05f, 0.15f, 1);
    rlVertex2f(-8, -28); rlVertex2f(8, -28); rlVertex2f(8, 14); rlVertex2f(-8, 14);
    rlEnd();
    circulo(0, 20, 10, 16, CFloat(0.12f, 0.06f, 0.18f));
    rlBegin(RL_TRIANGLES);
    rlColor4f(0.12f, 0.06f, 0.18f, 1);
    rlVertex2f(0, 36); rlVertex2f(-10, 18); rlVertex2f(10, 18);
    rlEnd();
    // Eyes
    circulo(-4, 21, 2.5f, 8, CFloat(0.5f, 0.0f, 0.8f));
    circulo(4, 21, 2.5f, 8, CFloat(0.5f, 0.0f, 0.8f));
    // Staff
    rlBegin(RL_QUADS);
    rlColor4f(0.35f, 0.25f, 0.1f, 1);
    rlVertex2f(10, -26); rlVertex2f(13, -26); rlVertex2f(13, 28); rlVertex2f(10, 28);
    rlEnd();
    circulo(11.5f, 33, 7, 14, CFloat(0.4f, 0.0f, 0.9f, 0.8f));
    circulo(11.5f, 33, 11, 14, CFloat(0.7f, 0.3f, 1.0f, 0.4f));
    // Magic ring
    float rr = 0.4f + 0.4f * sinf(tiempo * 0.02f);
    circuloLinea(11.5f, 33, 15, 6, CFloat(rr, 0, rr * 1.5f));
    rlPopMatrix();
}

void Drawing::dibujarSprite(int idx, float x, float y, float esc, float tiempo) {
    switch (idx) {
    case 0: spriteCaballero(x, y, esc, tiempo); break;
    case 1: spriteArquero(x, y, esc, tiempo);   break;
    case 2: spriteOrco(x, y, esc, tiempo);      break;
    case 3: spriteNigromante(x, y, esc, tiempo); break;
    }
}

// ============================================================
// SIMBOLO DE LUZ — Sol radiante dorado
// ============================================================
void Drawing::simboloLuz(float x, float y, float esc, float tiempo) {
    rlPushMatrix();
    rlTranslatef(x, y, 0);
    rlScalef(esc, esc, 1);

    // Aura exterior pulsante
    float al = 0.15f + 0.1f * sinf(tiempo * 0.008f);
    circulo(0, 0, 45, 24, CFloat(1.f, 0.85f, 0.2f, al));

    // Rayos del sol (rotando lentamente)
    float rot = tiempo * 0.003f;
    int nRayos = 12;
    rlBegin(RL_TRIANGLES);
    for (int i = 0; i < nRayos; i++) {
        float a = rot + i * 2.f * M_PI / nRayos;
        float inner = 22.f, outer = 38.f + sinf(tiempo * 0.01f + i) * 4.f;
        float hw = 0.18f; // half-width in radians
        rlColor4f(1.f, 0.8f, 0.1f, 0.7f);
        rlVertex2f(cosf(a) * outer, sinf(a) * outer);
        rlColor4f(1.f, 0.7f, 0.0f, 0.9f);
        rlVertex2f(cosf(a - hw) * inner, sinf(a - hw) * inner);
        rlVertex2f(cosf(a + hw) * inner, sinf(a + hw) * inner);
    }
    rlEnd();

    // Disco central dorado
    circulo(0, 0, 20, 20, CFloat(1.f, 0.85f, 0.15f));
    // Brillo interior
    circulo(0, 2, 14, 16, CFloat(1.f, 0.95f, 0.5f));
    // Punto de luz
    circulo(-4, 5, 5, 10, CFloat(1.f, 1.f, 0.8f, 0.6f));

    // Borde del disco
    circuloLinea(0, 0, 20, 20, CFloat(0.8f, 0.6f, 0.05f));

    rlPopMatrix();
}

// ============================================================
// SIMBOLO DE OSCURIDAD — Craneo con aura purpura
// ============================================================
void Drawing::simboloOscuridad(float x, float y, float esc, float tiempo) {
    rlPushMatrix();
    rlTranslatef(x, y, 0);
    rlScalef(esc, esc, 1);

    // Aura purpura pulsante
    float al = 0.12f + 0.08f * sinf(tiempo * 0.009f);
    circulo(0, 0, 45, 24, CFloat(0.6f, 0.1f, 0.9f, al));

    // Llamas oscuras girando
    float rot = tiempo * 0.004f;
    rlBegin(RL_TRIANGLES);
    for (int i = 0; i < 8; i++) {
        float a = rot + i * 2.f * M_PI / 8;
        float outer = 36.f + sinf(tiempo * 0.012f + i * 0.7f) * 5.f;
        rlColor4f(0.4f, 0.0f, 0.7f, 0.5f);
        rlVertex2f(cosf(a) * outer, sinf(a) * outer);
        rlColor4f(0.3f, 0.05f, 0.5f, 0.7f);
        rlVertex2f(cosf(a - 0.2f) * 20, sinf(a - 0.2f) * 20);
        rlVertex2f(cosf(a + 0.2f) * 20, sinf(a + 0.2f) * 20);
    }
    rlEnd();

    // Craneo — forma del craneo (ovalo superior)
    circulo(0, 2, 18, 16, CFloat(0.2f, 0.15f, 0.25f));
    // Mandibula (cuadro inferior)
    rlBegin(RL_QUADS);
    rlColor4f(0.18f, 0.13f, 0.22f, 1);
    rlVertex2f(-12, -2); rlVertex2f(12, -2);
    rlVertex2f(10, -14); rlVertex2f(-10, -14);
    rlEnd();

    // Ojos — cuencas vacias con brillo purpura
    float eyeGlow = 0.6f + 0.4f * sinf(tiempo * 0.015f);
    circulo(-7, 6, 5, 10, CFloat(0, 0, 0));
    circulo(7, 6, 5, 10, CFloat(0, 0, 0));
    circulo(-7, 6, 3, 8, CFloat(0.7f * eyeGlow, 0.1f, 0.9f * eyeGlow, 0.8f));
    circulo(7, 6, 3, 8, CFloat(0.7f * eyeGlow, 0.1f, 0.9f * eyeGlow, 0.8f));

    // Nariz (triangulo invertido)
    rlBegin(RL_TRIANGLES);
    rlColor4f(0.05f, 0.03f, 0.08f, 1);
    rlVertex2f(-2, 2); rlVertex2f(2, 2); rlVertex2f(0, -3);
    rlEnd();

    // Dientes
    rlBegin(RL_QUADS);
    rlColor4f(0.35f, 0.3f, 0.4f, 1);
    for (int i = 0; i < 5; i++) {
        float dx = -9.f + i * 4.5f;
        rlVertex2f(dx, -4); rlVertex2f(dx + 3, -4);
        rlVertex2f(dx + 3, -9); rlVertex2f(dx, -9);
    }
    rlEnd();
    // Lineas entre dientes
    rlSetLineWidth(1);
    rlBegin(RL_LINES);
    rlColor4f(0.05f, 0.03f, 0.08f, 1);
    for (int i = 1; i < 5; i++) {
        float dx = -9.f + i * 4.5f;
        rlVertex2f(dx, -4); rlVertex2f(dx, -9);
    }
    rlEnd();

    // Borde del craneo
    circuloLinea(0, 2, 18, 16, CFloat(0.35f, 0.15f, 0.5f));

    rlPopMatrix();
}

bool Drawing::botonVolver(float x, float y, float tiempo) {
    // Hit test in virtual 800x600 Y-up coords
    float mxv = (float)GetMouseX() * 800.f / GetScreenWidth();
    float myv = 600.f - (float)GetMouseY() * 600.f / GetScreenHeight();
    // Visible box bounds
    float bx0 = x - 10, bx1 = x + 65, by0 = y - 12, by1 = y + 14;
    // Hover/click zone: same centre, 10px bigger on each side
    float pad = 10.f;
    bool hover = (mxv > bx0 - pad && mxv < bx1 + pad &&
                  myv > by0 - pad && myv < by1 + pad);

    float al = hover ? (0.7f + 0.3f * sinf(tiempo * 0.015f)) : 0.45f;
    Color col = CFloat(0.85f * al, 0.7f * al, 0.15f * al);

    // Arrow shape: ◄—
    rlSetLineWidth(hover ? 2.5f : 1.5f);
    rlBegin(RL_LINES);
    rlColor4ub(col.r, col.g, col.b, col.a);
    rlVertex2f(x + 2, y); rlVertex2f(x + 18, y);
    rlVertex2f(x + 2, y); rlVertex2f(x + 10, y + 7);
    rlVertex2f(x + 2, y); rlVertex2f(x + 10, y - 7);
    rlEnd();

    if (hover) {
        rlBegin(RL_TRIANGLES);
        rlColor4ub(col.r, col.g, col.b, 80);
        rlVertex2f(x + 2, y);
        rlVertex2f(x + 10, y + 7);
        rlVertex2f(x + 10, y - 7);
        rlEnd();
    }

    texto12(x + 22, y - 2, "VOLVER", col);

    // Outline box
    rlSetLineWidth(hover ? 1.5f : 1.f);
    rlBegin(RL_LINES);
    rlColor4ub(col.r, col.g, col.b, hover ? (unsigned char)180 : (unsigned char)80);
    rlVertex2f(bx0, by0); rlVertex2f(bx1, by0);
    rlVertex2f(bx1, by0); rlVertex2f(bx1, by1);
    rlVertex2f(bx1, by1); rlVertex2f(bx0, by1);
    rlVertex2f(bx0, by1); rlVertex2f(bx0, by0);
    rlEnd();

    return hover;
}
