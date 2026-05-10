#include "Background.h"
#include "Drawing.h"
#include "rlgl.h"

// Helper: draw triangle fan from point array (first point is center)
static void drawFan(float pts[][2], int count, float r, float g, float b, float a = 1.0f) {
    rlBegin(RL_TRIANGLES);
    rlColor4f(r, g, b, a);
    for (int i = 1; i < count - 1; i++) {
        rlVertex2f(pts[0][0], pts[0][1]);
        rlVertex2f(pts[i][0], pts[i][1]);
        rlVertex2f(pts[i + 1][0], pts[i + 1][1]);
    }
    rlEnd();
}

// Helper: draw cloud shape
static void drawCloud(float x, float y, float esc, float alpha, float cr, float cg, float cb) {
    float radios[] = { 18, 14, 20, 16, 12 };
    float offx[] = { 0, 22, 38, 58, 75 };
    float offy[] = { 0, 6, -2, 4, -5 };
    for (int b = 0; b < 5; b++) {
        Drawing::circulo(x + offx[b] * esc, y + offy[b] * esc,
            radios[b] * esc * 0.7f, 24,
            CFloat(cr, cg, cb, alpha * (0.7f + b * 0.06f)));
    }
}

// Helper: draw tower
static void drawTower(float x, float base, float ancho, float alto,
    float bodyR, float bodyG, float bodyB,
    float stoneR, float stoneG, float stoneB,
    float winR, float winG, float winB, float tiempo) {
    // Shadow
    rlBegin(RL_QUADS);
    rlColor4f(bodyR * 0.3f, bodyG * 0.3f, bodyB * 0.3f, 1);
    rlVertex2f(x + 4, base); rlVertex2f(x + ancho + 4, base);
    rlVertex2f(x + ancho + 4, base + alto); rlVertex2f(x + 4, base + alto);
    // Body
    rlColor4f(bodyR, bodyG, bodyB, 1);
    rlVertex2f(x, base); rlVertex2f(x + ancho, base);
    rlVertex2f(x + ancho, base + alto); rlVertex2f(x, base + alto);
    rlEnd();
    // Stones
    rlSetLineWidth(0.7f);
    rlBegin(RL_LINES);
    rlColor4f(stoneR, stoneG, stoneB, 1);
    for (int r = 0; r < 8; r++) {
        float ry = base + r * 32.0f;
        rlVertex2f(x, ry); rlVertex2f(x + ancho, ry);
        if (r % 2 == 0) { rlVertex2f(x + ancho / 2, ry); rlVertex2f(x + ancho / 2, ry + 32); }
    }
    rlEnd();
    // Battlements
    int nAlm = (int)(ancho / 18);
    rlBegin(RL_QUADS);
    rlColor4f(bodyR, bodyG, bodyB, 1);
    for (int a = 0; a < nAlm; a++) {
        float ax = x + a * 18.0f;
        rlVertex2f(ax, base + alto); rlVertex2f(ax + 12, base + alto);
        rlVertex2f(ax + 12, base + alto + 22); rlVertex2f(ax, base + alto + 22);
    }
    rlEnd();
    // Window
    float wx = x + ancho / 2 - 8, wy = base + alto * 0.55f;
    rlBegin(RL_QUADS);
    rlColor4f(stoneR * 0.5f, stoneG * 0.5f, stoneB * 0.5f, 1);
    rlVertex2f(wx, wy); rlVertex2f(wx + 16, wy); rlVertex2f(wx + 16, wy + 22); rlVertex2f(wx, wy + 22);
    rlEnd();
    // Window arch
    rlBegin(RL_TRIANGLES);
    rlColor4f(stoneR * 0.6f, stoneG * 0.6f, stoneB * 0.6f, 1);
    for (int j = 0; j < 12; j++) {
        float a1 = j * M_PI / 12;
        float a2 = (j + 1) * M_PI / 12;
        rlVertex2f(wx + 8, wy + 22);
        rlVertex2f(wx + 8 + cosf(a1) * 8, wy + 22 + sinf(a1) * 8);
        rlVertex2f(wx + 8 + cosf(a2) * 8, wy + 22 + sinf(a2) * 8);
    }
    rlEnd();
    // Window glow
    float br = 0.4f + 0.3f * sinf(tiempo * 0.02f + x * 0.01f);
    rlBegin(RL_QUADS);
    rlColor4f(winR * br, winG * br, winB * br, 0.7f);
    rlVertex2f(wx + 2, wy + 2); rlVertex2f(wx + 14, wy + 2);
    rlVertex2f(wx + 14, wy + 20); rlVertex2f(wx + 2, wy + 20);
    rlEnd();
}

// ============================================================
// CASTLE BACKGROUND (Main Menu)
// ============================================================
void Background::castle(GameState& gs) {
    float t = gs.tiempo;
    // Sky gradient
    rlBegin(RL_QUADS);
    rlColor4f(0.01f, 0.01f, 0.04f, 1); rlVertex2f(0, 0);
    rlColor4f(0.01f, 0.01f, 0.04f, 1); rlVertex2f(800, 0);
    rlColor4f(0.06f, 0.04f, 0.14f, 1); rlVertex2f(800, 300);
    rlColor4f(0.06f, 0.04f, 0.14f, 1); rlVertex2f(0, 300);
    rlColor4f(0.06f, 0.04f, 0.14f, 1); rlVertex2f(0, 300);
    rlColor4f(0.06f, 0.04f, 0.14f, 1); rlVertex2f(800, 300);
    rlColor4f(0.12f, 0.08f, 0.28f, 1); rlVertex2f(800, 600);
    rlColor4f(0.12f, 0.08f, 0.28f, 1); rlVertex2f(0, 600);
    rlEnd();

    // Moon halos
    float lx = 680, ly = 520, lr = 38;
    for (int h = 0; h < 5; h++) {
        float hr = lr + 20 + h * 14.0f;
        float ha = 0.06f - h * 0.012f;
        Drawing::circulo(lx, ly, hr, 40, CFloat(0.88f, 0.88f, 0.70f, ha));
    }
    // Moon rays
    rlBegin(RL_TRIANGLES);
    rlColor4f(0.85f, 0.85f, 0.65f, 0.06f);
    for (int r = 0; r < 12; r++) {
        float a = r * 2.0f * M_PI / 12 + t * 0.0005f;
        rlVertex2f(lx, ly);
        rlVertex2f(lx + cosf(a - 0.04f) * 120, ly + sinf(a - 0.04f) * 120);
        rlVertex2f(lx + cosf(a + 0.04f) * 120, ly + sinf(a + 0.04f) * 120);
    }
    rlEnd();
    // Moon disc + craters
    Drawing::circulo(lx, ly, lr, 50, CFloat(0.95f, 0.94f, 0.82f));
    Drawing::circulo(lx - 12, ly + 8, 7, 20, CFloat(0.82f, 0.80f, 0.70f));
    Drawing::circulo(lx + 10, ly - 10, 5, 20, CFloat(0.82f, 0.80f, 0.70f));
    Drawing::circulo(lx - 4, ly - 14, 4, 20, CFloat(0.82f, 0.80f, 0.70f));
    Drawing::circulo(lx - 12, ly + 8, 5, 20, CFloat(0.78f, 0.76f, 0.66f));
    Drawing::circulo(lx + 10, ly - 10, 3.5f, 20, CFloat(0.78f, 0.76f, 0.66f));

    // Stars
    srand(42);
    for (int i = 0; i < 120; i++) {
        float sx = (float)(rand() % 800);
        float sy = 250 + (float)(rand() % 350);
        float sb = 0.4f + 0.6f * sinf(t * 0.003f * (1 + i % 7) + (float)i);
        Drawing::circulo(sx, sy, 0.5f + (i % 3) * 0.18f, 4, CFloat(sb, sb, sb * 0.9f + 0.1f));
    }
    srand((unsigned)time(nullptr));

    // Clouds
    drawCloud(30 + sinf(t * 0.0007f) * 120, 490, 1.3f, 0.55f, 0.22f, 0.18f, 0.32f);
    drawCloud(280 + sinf(t * 0.0011f) * 100, 470, 1.6f, 0.50f, 0.22f, 0.18f, 0.32f);
    drawCloud(550 + sinf(t * 0.0008f) * 90, 505, 1.1f, 0.45f, 0.22f, 0.18f, 0.32f);
    drawCloud(700 + sinf(t * 0.0013f) * 70, 480, 0.9f, 0.40f, 0.22f, 0.18f, 0.32f);

    // Mountains (3 layers)
    float m1[][2] = {{0,220},{80,295},{160,240},{240,310},{320,255},{400,320},{480,260},{560,315},{640,250},{720,300},{800,230},{800,220}};
    drawFan(m1, 12, 0.07f, 0.05f, 0.12f);
    float m2[][2] = {{0,180},{60,240},{140,200},{220,260},{300,210},{400,275},{500,205},{600,265},{700,215},{800,255},{800,180}};
    drawFan(m2, 11, 0.05f, 0.03f, 0.09f);
    float m3[][2] = {{0,155},{50,195},{120,165},{200,215},{290,170},{380,220},{460,175},{550,230},{650,168},{750,210},{800,160},{800,155}};
    drawFan(m3, 12, 0.04f, 0.02f, 0.07f);

    // Stone floor + joints
    rlBegin(RL_QUADS);
    rlColor4f(0.08f, 0.07f, 0.09f, 1); rlVertex2f(0, 0);
    rlColor4f(0.08f, 0.07f, 0.09f, 1); rlVertex2f(800, 0);
    rlColor4f(0.11f, 0.10f, 0.13f, 1); rlVertex2f(800, 155);
    rlColor4f(0.11f, 0.10f, 0.13f, 1); rlVertex2f(0, 155);
    rlEnd();
    rlBegin(RL_LINES);
    rlColor4f(0.05f, 0.04f, 0.06f, 1);
    for (int row = 0; row < 5; row++) {
        float jy = 30.0f + row * 25.0f;
        rlVertex2f(0, jy); rlVertex2f(800, jy);
        int off = (row % 2) * 40;
        for (int col = 0; col < 13; col++) {
            float jx = off + col * 65.0f;
            rlVertex2f(jx, jy); rlVertex2f(jx, jy + 25);
        }
    }
    rlEnd();

    // Castle shadow
    rlBegin(RL_QUADS);
    rlColor4f(0.03f, 0.02f, 0.05f, 1);
    rlVertex2f(90, 155); rlVertex2f(718, 155); rlVertex2f(710, 420); rlVertex2f(98, 420);
    // Main wall
    rlColor4f(0.10f, 0.09f, 0.12f, 1);
    rlVertex2f(95, 155); rlVertex2f(710, 155); rlVertex2f(700, 415); rlVertex2f(100, 415);
    rlEnd();

    // Wall stones
    rlBegin(RL_LINES);
    rlColor4f(0.06f, 0.05f, 0.08f, 1);
    for (int row = 0; row < 7; row++) {
        float by = 175 + row * 35.0f;
        rlVertex2f(100, by); rlVertex2f(700, by);
        int off = (row % 2) * 45;
        for (int col = 0; col < 12; col++) {
            float bx = off + 100.0f + col * 55.0f;
            rlVertex2f(bx, by); rlVertex2f(bx, by + 35);
        }
    }
    rlEnd();

    // Wall battlements
    rlBegin(RL_QUADS);
    rlColor4f(0.10f, 0.09f, 0.12f, 1);
    for (int i = 0; i < 11; i++) {
        float ax = 110.0f + i * 54.0f;
        rlVertex2f(ax, 415); rlVertex2f(ax + 32, 415);
        rlVertex2f(ax + 32, 445); rlVertex2f(ax, 445);
    }
    rlEnd();

    // Lateral towers
    drawTower(60, 155, 80, 280, 0.09f, 0.08f, 0.11f, 0.05f, 0.04f, 0.07f, 0.9f, 0.7f, 0.2f, t);
    drawTower(155, 155, 65, 240, 0.09f, 0.08f, 0.11f, 0.05f, 0.04f, 0.07f, 0.9f, 0.7f, 0.2f, t);
    drawTower(580, 155, 65, 240, 0.09f, 0.08f, 0.11f, 0.05f, 0.04f, 0.07f, 0.9f, 0.7f, 0.2f, t);
    drawTower(660, 155, 80, 280, 0.09f, 0.08f, 0.11f, 0.05f, 0.04f, 0.07f, 0.9f, 0.7f, 0.2f, t);

    // Central tower
    rlBegin(RL_QUADS);
    rlColor4f(0.03f, 0.02f, 0.05f, 1);
    rlVertex2f(334, 155); rlVertex2f(474, 155); rlVertex2f(474, 490); rlVertex2f(334, 490);
    rlColor4f(0.11f, 0.10f, 0.14f, 1);
    rlVertex2f(330, 155); rlVertex2f(470, 155); rlVertex2f(470, 485); rlVertex2f(330, 485);
    rlEnd();
    rlBegin(RL_LINES);
    rlColor4f(0.06f, 0.05f, 0.08f, 1);
    for (int r = 0; r < 10; r++) {
        float ry = 175 + r * 32.0f;
        rlVertex2f(330, ry); rlVertex2f(470, ry);
        int oc = (r % 2) * 35;
        for (int c = 0; c < 5; c++) {
            float cx = 330.0f + oc + c * 35.0f;
            rlVertex2f(cx, ry); rlVertex2f(cx, ry + 32);
        }
    }
    rlEnd();
    rlBegin(RL_QUADS);
    for (int i = 0; i < 6; i++) {
        float ax = 332.0f + i * 23.0f;
        rlColor4f(0.11f, 0.10f, 0.14f, 1);
        rlVertex2f(ax, 485); rlVertex2f(ax + 16, 485);
        rlVertex2f(ax + 16, 512); rlVertex2f(ax, 512);
    }
    rlEnd();

    // Central window
    float twx = 382, twy = 350;
    rlBegin(RL_QUADS);
    rlColor4f(0.02f, 0.01f, 0.03f, 1);
    rlVertex2f(twx, twy); rlVertex2f(twx + 36, twy); rlVertex2f(twx + 36, twy + 40); rlVertex2f(twx, twy + 40);
    rlEnd();
    rlBegin(RL_TRIANGLES);
    rlColor4f(0.02f, 0.01f, 0.03f, 1);
    for (int j = 0; j < 16; j++) {
        float a1 = j * M_PI / 16; float a2 = (j + 1) * M_PI / 16;
        rlVertex2f(twx + 18, twy + 40);
        rlVertex2f(twx + 18 + cosf(a1) * 18, twy + 40 + sinf(a1) * 18);
        rlVertex2f(twx + 18 + cosf(a2) * 18, twy + 40 + sinf(a2) * 18);
    }
    rlEnd();
    float tbr = 0.5f + 0.35f * sinf(t * 0.015f);
    rlBegin(RL_QUADS);
    rlColor4f(0.95f * tbr, 0.75f * tbr, 0.25f * tbr, 0.85f);
    rlVertex2f(twx + 3, twy + 3); rlVertex2f(twx + 33, twy + 3);
    rlVertex2f(twx + 33, twy + 37); rlVertex2f(twx + 3, twy + 37);
    rlEnd();

    // Banner
    float banX = 400, banY = 512;
    rlSetLineWidth(2);
    rlBegin(RL_LINES);
    rlColor4f(0.25f, 0.18f, 0.08f, 1);
    rlVertex2f(banX, banY); rlVertex2f(banX, banY + 40);
    rlEnd();
    float wave = sinf(t * 0.018f) * 4;
    rlBegin(RL_TRIANGLES);
    rlColor4f(0.7f, 0.05f, 0.05f, 1);
    rlVertex2f(banX, banY + 40); rlVertex2f(banX + 28 + wave, banY + 32); rlVertex2f(banX, banY + 24);
    rlEnd();
    rlBegin(RL_LINES);
    rlColor4f(0.95f, 0.85f, 0.2f, 1);
    rlVertex2f(banX + 5, banY + 36); rlVertex2f(banX + 22, banY + 36);
    rlVertex2f(banX + 12, banY + 42); rlVertex2f(banX + 12, banY + 28);
    rlEnd();

    // Gate
    float gx = 360, gy = 155, gw = 80, gh = 120;
    rlBegin(RL_QUADS);
    rlColor4f(0.02f, 0.01f, 0.02f, 1);
    rlVertex2f(gx, gy); rlVertex2f(gx + gw, gy); rlVertex2f(gx + gw, gy + gh); rlVertex2f(gx, gy + gh);
    rlEnd();
    rlBegin(RL_TRIANGLES);
    rlColor4f(0.02f, 0.01f, 0.02f, 1);
    for (int j = 0; j < 18; j++) {
        float a1 = j * M_PI / 18; float a2 = (j + 1) * M_PI / 18;
        rlVertex2f(gx + gw / 2, gy + gh);
        rlVertex2f(gx + gw / 2 + cosf(a1) * gw / 2, gy + gh + sinf(a1) * gw / 2);
        rlVertex2f(gx + gw / 2 + cosf(a2) * gw / 2, gy + gh + sinf(a2) * gw / 2);
    }
    rlEnd();
    // Portcullis
    rlSetLineWidth(3);
    rlBegin(RL_LINES);
    rlColor4f(0.12f, 0.10f, 0.04f, 1);
    for (int r = 0; r < 5; r++) {
        float rx = gx + 8.0f + r * 16.0f;
        rlVertex2f(rx, gy); rlVertex2f(rx, gy + gh);
    }
    rlEnd();
    rlSetLineWidth(2);
    rlBegin(RL_LINES);
    rlColor4f(0.12f, 0.10f, 0.04f, 1);
    for (int r = 0; r < 4; r++) {
        float ry = gy + 20.0f + r * 28.0f;
        rlVertex2f(gx + 4, ry); rlVertex2f(gx + gw - 4, ry);
    }
    rlEnd();

    // Bridge
    rlBegin(RL_QUADS);
    rlColor4f(0.08f, 0.07f, 0.09f, 1);
    rlVertex2f(350, 0); rlVertex2f(450, 0); rlVertex2f(gx + gw, gy); rlVertex2f(gx, gy);
    rlEnd();
    // Chain
    rlSetLineWidth(1.5f);
    rlBegin(RL_LINES);
    rlColor4f(0.18f, 0.15f, 0.08f, 1);
    for (int c = 0; c < 8; c++) {
        float ct = (float)c / 8; float ct2 = (float)(c + 1) / 8;
        rlVertex2f(gx + ct * gw, gy - 25 + sinf(ct * M_PI) * 15);
        rlVertex2f(gx + ct2 * gw, gy - 25 + sinf(ct2 * M_PI) * 15);
    }
    rlEnd();

    // Fog
    for (int f = 0; f < 8; f++) {
        float fy = f * 19.0f;
        float fa = 0.18f - f * 0.02f;
        rlBegin(RL_QUADS);
        rlColor4f(0.08f, 0.06f, 0.12f, fa);
        rlVertex2f(0, fy); rlVertex2f(800, fy);
        rlVertex2f(800, fy + 20); rlVertex2f(0, fy + 20);
        rlEnd();
    }
}

// ============================================================
// BATTLE BACKGROUND (Play Menu) - Vampire Castle
// ============================================================
void Background::battle(GameState& gs) {
    float t = gs.tiempo;
    // Blood-red sky
    rlBegin(RL_QUADS);
    rlColor4f(0.02f, 0.00f, 0.01f, 1); rlVertex2f(0, 0);
    rlColor4f(0.02f, 0.00f, 0.01f, 1); rlVertex2f(800, 0);
    rlColor4f(0.18f, 0.02f, 0.04f, 1); rlVertex2f(800, 600);
    rlColor4f(0.14f, 0.01f, 0.03f, 1); rlVertex2f(0, 600);
    rlEnd();

    // Red moon
    float lx = 680, ly = 520, lr = 38;
    for (int h = 0; h < 6; h++) {
        float hr = lr + 18 + h * 16.0f;
        float hp = (0.07f - h * 0.01f) + 0.025f * sinf(t * 0.006f + h * 0.5f);
        Drawing::circulo(lx, ly, hr, 40, CFloat(0.75f, 0.05f + h * 0.01f, 0.05f, hp));
    }
    rlBegin(RL_TRIANGLES);
    rlColor4f(0.7f, 0.04f, 0.04f, 0.05f);
    for (int r = 0; r < 12; r++) {
        float a = r * 2.0f * M_PI / 12 + t * 0.0004f;
        rlVertex2f(lx, ly);
        rlVertex2f(lx + cosf(a - 0.04f) * 130, ly + sinf(a - 0.04f) * 130);
        rlVertex2f(lx + cosf(a + 0.04f) * 130, ly + sinf(a + 0.04f) * 130);
    }
    rlEnd();
    float lunaBr = 0.75f + 0.1f * sinf(t * 0.008f);
    Drawing::circulo(lx, ly, lr, 50, CFloat(lunaBr * 0.88f, lunaBr * 0.06f, lunaBr * 0.06f));
    Drawing::circulo(lx - 12, ly + 8, 7, 20, CFloat(lunaBr * 0.6f, 0.02f, 0.02f));
    Drawing::circulo(lx + 10, ly - 10, 5, 20, CFloat(lunaBr * 0.6f, 0.02f, 0.02f));

    // Stars (reddish)
    srand(99);
    for (int i = 0; i < 100; i++) {
        float sx = (float)(rand() % 800), sy = 250 + (float)(rand() % 350);
        float sb = 0.3f + 0.5f * sinf(t * 0.003f * (1 + i % 7) + (float)i);
        Drawing::circulo(sx, sy, 0.5f + (i % 3) * 0.15f, 4,
            CFloat(sb, sb * (0.3f + (i % 5) * 0.08f), sb * (0.3f + (i % 4) * 0.05f)));
    }
    srand((unsigned)time(nullptr));

    // Vampire clouds
    drawCloud(20 + sinf(t * 0.0007f) * 110, 490, 1.4f, 0.70f, 0.18f, 0.04f, 0.06f);
    drawCloud(270 + sinf(t * 0.0011f) * 95, 472, 1.7f, 0.65f, 0.18f, 0.04f, 0.06f);
    drawCloud(540 + sinf(t * 0.0008f) * 85, 508, 1.2f, 0.60f, 0.18f, 0.04f, 0.06f);
    drawCloud(710 + sinf(t * 0.0013f) * 65, 482, 1.0f, 0.55f, 0.18f, 0.04f, 0.06f);

    // Mountains
    float m1[][2] = {{0,220},{70,302},{155,238},{235,315},{315,252},{400,318},{485,258},{565,312},{645,248},{725,298},{800,228},{800,220}};
    drawFan(m1, 12, 0.06f, 0.01f, 0.02f);
    float m2[][2] = {{0,178},{55,242},{138,198},{218,262},{298,208},{400,272},{502,202},{602,262},{702,212},{800,252},{800,178}};
    drawFan(m2, 11, 0.04f, 0.01f, 0.015f);
    float m3[][2] = {{0,153},{48,198},{118,163},{198,218},{288,168},{382,222},{462,172},{552,228},{652,165},{752,208},{800,158},{800,153}};
    drawFan(m3, 12, 0.03f, 0.005f, 0.01f);

    // Dark stone floor
    rlBegin(RL_QUADS);
    rlColor4f(0.06f, 0.02f, 0.02f, 1); rlVertex2f(0, 0);
    rlColor4f(0.06f, 0.02f, 0.02f, 1); rlVertex2f(800, 0);
    rlColor4f(0.09f, 0.03f, 0.03f, 1); rlVertex2f(800, 153);
    rlColor4f(0.09f, 0.03f, 0.03f, 1); rlVertex2f(0, 153);
    rlEnd();
    rlBegin(RL_LINES);
    rlColor4f(0.03f, 0.01f, 0.01f, 1);
    for (int row = 0; row < 5; row++) {
        float jy = 28 + row * 25.0f;
        rlVertex2f(0, jy); rlVertex2f(800, jy);
        int off = (row % 2) * 40;
        for (int col = 0; col < 13; col++) {
            float jx = off + col * 65.0f;
            rlVertex2f(jx, jy); rlVertex2f(jx, jy + 25);
        }
    }
    rlEnd();

    // Red veins on floor
    rlBegin(RL_LINES);
    rlColor4f(0.55f, 0.0f, 0.0f, 0.25f);
    for (int c = 0; c < 5; c++) {
        float cx = 80.0f + c * 140.0f + sinf(c * 1.7f) * 30;
        rlVertex2f(cx, 0); rlVertex2f(cx + 20, 40);
        rlVertex2f(cx + 20, 40); rlVertex2f(cx + 8, 80);
        rlVertex2f(cx + 8, 80); rlVertex2f(cx + 28, 130);
    }
    rlEnd();

    // Castle structure
    rlBegin(RL_QUADS);
    rlColor4f(0.02f, 0.01f, 0.01f, 1);
    rlVertex2f(90, 153); rlVertex2f(718, 153); rlVertex2f(710, 420); rlVertex2f(98, 420);
    rlColor4f(0.07f, 0.03f, 0.03f, 1);
    rlVertex2f(95, 153); rlVertex2f(710, 153); rlVertex2f(700, 415); rlVertex2f(100, 415);
    rlEnd();

    // Wall stones
    rlBegin(RL_LINES);
    rlColor4f(0.04f, 0.015f, 0.015f, 1);
    for (int row = 0; row < 7; row++) {
        float by = 173 + row * 35.0f;
        rlVertex2f(100, by); rlVertex2f(700, by);
        int off = (row % 2) * 45;
        for (int col = 0; col < 12; col++) {
            float bx = off + 100.0f + col * 55.0f;
            rlVertex2f(bx, by); rlVertex2f(bx, by + 35);
        }
    }
    rlEnd();

    // Wall cracks
    rlBegin(RL_LINES);
    rlColor4f(0.5f, 0.0f, 0.0f, 0.20f);
    for (int g = 0; g < 6; g++) {
        float gx2 = 130.0f + g * 95.0f;
        float gy2 = 200.0f + sinf(g * 1.3f) * 40;
        rlVertex2f(gx2, gy2); rlVertex2f(gx2 + 12, gy2 + 30);
        rlVertex2f(gx2 + 12, gy2 + 30); rlVertex2f(gx2 + 5, gy2 + 55);
        rlVertex2f(gx2 + 5, gy2 + 55); rlVertex2f(gx2 + 18, gy2 + 80);
    }
    rlEnd();

    // Battlements with spikes
    rlBegin(RL_QUADS);
    for (int i = 0; i < 11; i++) {
        float ax = 110.0f + i * 54.0f;
        rlColor4f(0.07f, 0.03f, 0.03f, 1);
        rlVertex2f(ax, 415); rlVertex2f(ax + 32, 415);
        rlVertex2f(ax + 32, 445); rlVertex2f(ax, 445);
    }
    rlEnd();
    rlBegin(RL_TRIANGLES);
    rlColor4f(0.05f, 0.02f, 0.02f, 1);
    for (int i = 0; i < 11; i++) {
        float ax = 110.0f + i * 54.0f;
        rlVertex2f(ax + 16, 465); rlVertex2f(ax + 10, 445); rlVertex2f(ax + 22, 445);
    }
    rlEnd();

    // Vampire towers
    drawTower(60, 153, 80, 280, 0.06f, 0.02f, 0.02f, 0.035f, 0.012f, 0.012f, 0.5f, 0.0f, 1.2f, t);
    drawTower(155, 153, 65, 240, 0.06f, 0.02f, 0.02f, 0.035f, 0.012f, 0.012f, 0.5f, 0.0f, 1.2f, t);
    drawTower(580, 153, 65, 240, 0.06f, 0.02f, 0.02f, 0.035f, 0.012f, 0.012f, 0.5f, 0.0f, 1.2f, t);
    drawTower(660, 153, 80, 280, 0.06f, 0.02f, 0.02f, 0.035f, 0.012f, 0.012f, 0.5f, 0.0f, 1.2f, t);

    // Central tower
    rlBegin(RL_QUADS);
    rlColor4f(0.02f, 0.01f, 0.01f, 1);
    rlVertex2f(334, 153); rlVertex2f(474, 153); rlVertex2f(474, 490); rlVertex2f(334, 490);
    rlColor4f(0.07f, 0.025f, 0.025f, 1);
    rlVertex2f(330, 153); rlVertex2f(470, 153); rlVertex2f(470, 485); rlVertex2f(330, 485);
    rlEnd();

    // Central battlements with spikes
    rlBegin(RL_QUADS);
    for (int i = 0; i < 6; i++) {
        float ax = 332.0f + i * 23.0f;
        rlColor4f(0.07f, 0.025f, 0.025f, 1);
        rlVertex2f(ax, 485); rlVertex2f(ax + 16, 485); rlVertex2f(ax + 16, 510); rlVertex2f(ax, 510);
    }
    rlEnd();
    rlBegin(RL_TRIANGLES);
    rlColor4f(0.07f, 0.025f, 0.025f, 1);
    for (int i = 0; i < 6; i++) {
        float ax = 332.0f + i * 23.0f;
        rlVertex2f(ax + 8, 528); rlVertex2f(ax + 2, 510); rlVertex2f(ax + 14, 510);
    }
    rlEnd();

    // Gate
    float gx = 360, gy = 153, gw = 80, gh = 120;
    rlBegin(RL_QUADS);
    rlColor4f(0.01f, 0.005f, 0.005f, 1);
    rlVertex2f(gx, gy); rlVertex2f(gx + gw, gy); rlVertex2f(gx + gw, gy + gh); rlVertex2f(gx, gy + gh);
    rlEnd();
    rlBegin(RL_TRIANGLES);
    rlColor4f(0.01f, 0.005f, 0.005f, 1);
    for (int j = 0; j < 18; j++) {
        float a1 = j * M_PI / 18; float a2 = (j + 1) * M_PI / 18;
        rlVertex2f(gx + gw / 2, gy + gh);
        rlVertex2f(gx + gw / 2 + cosf(a1) * gw / 2, gy + gh + sinf(a1) * gw / 2);
        rlVertex2f(gx + gw / 2 + cosf(a2) * gw / 2, gy + gh + sinf(a2) * gw / 2);
    }
    rlEnd();
    // Evil eye
    float eyBr = 0.5f + 0.4f * sinf(t * 0.022f);
    Drawing::circulo(gx + gw / 2, gy + gh + gw / 2, 5, 12, CFloat(eyBr * 0.9f, 0.0f, 0.0f, 0.7f));
    Drawing::circulo(gx + gw / 2, gy + gh + gw / 2, 12, 16, CFloat(eyBr * 0.4f, 0.0f, 0.0f, 0.25f));

    // Bridge
    rlBegin(RL_QUADS);
    rlColor4f(0.05f, 0.02f, 0.02f, 1);
    rlVertex2f(350, 0); rlVertex2f(450, 0); rlVertex2f(gx + gw, gy); rlVertex2f(gx, gy);
    rlEnd();

    // Red fog
    for (int f = 0; f < 10; f++) {
        float fy = f * 15.0f;
        float fa = 0.22f - f * 0.02f;
        float fp = fa + 0.04f * sinf(t * 0.005f + f * 0.4f);
        rlBegin(RL_QUADS);
        rlColor4f(0.18f, 0.01f, 0.03f, fp);
        rlVertex2f(0, fy); rlVertex2f(800, fy); rlVertex2f(800, fy + 16); rlVertex2f(0, fy + 16);
        rlEnd();
    }

    // Bats
    for (int b = 0; b < 5; b++) {
        float bt = fmodf(t * 0.004f + b * 0.4f, 1.0f);
        float bx = fmodf(bt * 900 + b * 175.0f, 900.0f) - 50;
        float by = 420 + sinf(t * 0.015f + b * 1.2f) * 30 + sinf(t * 0.008f + b * 0.7f) * 20;
        float bs = 0.7f + b * 0.12f;
        float wf = sinf(t * 0.08f + b * 0.9f);
        rlBegin(RL_TRIANGLES);
        rlColor4f(0.04f, 0.01f, 0.01f, 1);
        // Left wing
        rlVertex2f(bx, by); rlVertex2f(bx - 18 * bs, by + wf * 6 * bs); rlVertex2f(bx - 6 * bs, by + 3 * bs);
        // Right wing
        rlVertex2f(bx, by); rlVertex2f(bx + 18 * bs, by + wf * 6 * bs); rlVertex2f(bx + 6 * bs, by + 3 * bs);
        rlEnd();
        Drawing::circulo(bx, by, 3 * bs, 8, CFloat(0.04f, 0.01f, 0.01f));
    }

    // Fire particles
    if (gs.controlesOpciones[3].valor) {
        for (auto& p : gs.particulasFuego) {
            float vn = p.vida / p.vidaMax;
            float al = (vn < 0.3f) ? vn / 0.3f : 1.0f - (vn - 0.3f) / 0.7f;
            Drawing::circulo(p.x, p.y, p.size * 0.5f, 6,
                CFloat(0.7f + vn * 0.2f, 0.0f, 0.2f + vn * 0.3f, al * 0.7f));
        }
    }
}

// ============================================================
// LIBRARY BACKGROUND (Encyclopedia)
// ============================================================
void Background::library(GameState& gs) {
    float t = gs.tiempo;
    rlBegin(RL_QUADS);
    rlColor4f(0.06f, 0.04f, 0.02f, 1); rlVertex2f(0, 0);
    rlColor4f(0.10f, 0.07f, 0.03f, 1); rlVertex2f(800, 0);
    rlColor4f(0.12f, 0.08f, 0.03f, 1); rlVertex2f(800, 600);
    rlColor4f(0.07f, 0.05f, 0.02f, 1); rlVertex2f(0, 600);
    rlEnd();

    // Bookshelves
    for (int e = 0; e < 3; e++) {
        float ey = 80.0f + e * 170;
        rlBegin(RL_QUADS);
        rlColor4f(0.08f, 0.05f, 0.02f, 1);
        rlVertex2f(0, ey); rlVertex2f(800, ey); rlVertex2f(800, ey + 12); rlVertex2f(0, ey + 12);
        rlEnd();
        for (int b = 0; b < 30; b++) {
            float bx = b * 28.0f + sinf(b * 13.0f) * 3;
            float bh = 60.0f + (b * 17) % 50;
            rlBegin(RL_QUADS);
            rlColor4f(0.05f + (b * 7 % 5) * 0.03f, 0.03f + (b * 3 % 4) * 0.02f, 0.01f + (b * 11 % 3) * 0.015f, 1);
            rlVertex2f(bx, ey + 12); rlVertex2f(bx + 24, ey + 12);
            rlVertex2f(bx + 24, ey + 12 + bh); rlVertex2f(bx, ey + 12 + bh);
            rlEnd();
            rlBegin(RL_LINES);
            rlColor4f(0.2f, 0.15f, 0.05f, 1);
            rlVertex2f(bx, ey + 12); rlVertex2f(bx, ey + 12 + bh);
            rlEnd();
        }
    }

    // Chandelier
    float cx = 400, cy = 575;
    rlBegin(RL_LINES);
    rlColor4f(0.3f, 0.2f, 0.05f, 1);
    rlVertex2f(cx, cy); rlVertex2f(cx, cy - 30);
    rlVertex2f(cx - 60, cy - 30); rlVertex2f(cx + 60, cy - 30);
    rlEnd();
    for (int i = -1; i <= 1; i++) {
        float cvx = cx + i * 60.0f;
        rlBegin(RL_QUADS);
        rlColor4f(0.9f, 0.85f, 0.7f, 1);
        rlVertex2f(cvx - 4, cy - 30); rlVertex2f(cvx + 4, cy - 30);
        rlVertex2f(cvx + 4, cy - 30 + 15); rlVertex2f(cvx - 4, cy - 30 + 15);
        rlEnd();
        float fl = sinf(t * 0.04f + (float)i) * 4;
        rlBegin(RL_TRIANGLES);
        rlColor4f(1, 0.6f, 0, 0.9f);
        rlVertex2f(cvx, cy - 30 + 25 + fl); rlVertex2f(cvx - 5, cy - 30 + 15); rlVertex2f(cvx + 5, cy - 30 + 15);
        rlEnd();
    }

    // Dust particles
    if (gs.controlesOpciones[3].valor) {
        for (auto& p : gs.particulasPolvo) {
            float vn = p.vida / p.vidaMax;
            float al = (vn < 0.2f) ? vn / 0.2f : (vn > 0.8f) ? (1.0f - vn) / 0.2f : 0.5f;
            Drawing::circulo(p.x, p.y, 1.0f, 4, CFloat(0.8f, 0.7f, 0.5f, al * 0.6f));
        }
    }
}

// ============================================================
// THEATER BACKGROUND (Options)
// ============================================================
void Background::theater(GameState& gs) {
    float t = gs.tiempo;
    // Dark background
    rlBegin(RL_QUADS);
    rlColor4f(0.01f, 0.01f, 0.03f, 1); rlVertex2f(0, 0);
    rlColor4f(0.02f, 0.01f, 0.05f, 1); rlVertex2f(800, 0);
    rlColor4f(0.03f, 0.01f, 0.07f, 1); rlVertex2f(800, 600);
    rlColor4f(0.01f, 0.01f, 0.04f, 1); rlVertex2f(0, 600);
    // Stage floor
    rlColor4f(0.08f, 0.05f, 0.02f, 1); rlVertex2f(0, 0);
    rlColor4f(0.08f, 0.05f, 0.02f, 1); rlVertex2f(800, 0);
    rlColor4f(0.08f, 0.05f, 0.02f, 1); rlVertex2f(800, 120);
    rlColor4f(0.08f, 0.05f, 0.02f, 1); rlVertex2f(0, 120);
    rlEnd();

    rlBegin(RL_LINES);
    rlColor4f(0.4f, 0.3f, 0.1f, 1);
    rlVertex2f(0, 120); rlVertex2f(800, 120);
    rlEnd();

    // Footlights
    for (int i = 0; i < 10; i++) {
        float llx = 40.0f + i * 80;
        float br = 0.7f + 0.3f * sinf(t * 0.015f + i * 0.7f);
        rlBegin(RL_LINES);
        rlColor4f(0.15f, 0.1f, 0.05f, 1);
        rlVertex2f(llx, 600); rlVertex2f(llx, 565);
        rlEnd();
        Drawing::circulo(llx, 562, 7, 12, CFloat(br, br * 0.9f, br * 0.4f));
        Drawing::circulo(llx, 562, 20, 16, CFloat(br, br * 0.7f, 0.1f, 0.15f));
    }

    // Left curtain
    rlBegin(RL_QUADS);
    rlColor4f(0.25f, 0.04f, 0.04f, 1);
    rlVertex2f(0, 120); rlVertex2f(80, 120); rlVertex2f(80, 600); rlVertex2f(0, 600);
    rlEnd();
    for (int i = 0; i < 4; i++) {
        float px = 15.0f + i * 18;
        rlBegin(RL_QUADS);
        rlColor4f(0.18f, 0.02f, 0.02f, 1);
        rlVertex2f(px, 120); rlVertex2f(px + 10, 120); rlVertex2f(px + 6, 600); rlVertex2f(px - 4, 600);
        rlEnd();
    }
    rlBegin(RL_LINES);
    rlColor4f(0.7f, 0.55f, 0.1f, 1); rlSetLineWidth(3);
    rlVertex2f(80, 120); rlVertex2f(80, 600);
    rlEnd();

    // Right curtain
    rlBegin(RL_QUADS);
    rlColor4f(0.25f, 0.04f, 0.04f, 1);
    rlVertex2f(720, 120); rlVertex2f(800, 120); rlVertex2f(800, 600); rlVertex2f(720, 600);
    rlEnd();
    for (int i = 0; i < 4; i++) {
        float px = 725.0f + i * 18;
        rlBegin(RL_QUADS);
        rlColor4f(0.18f, 0.02f, 0.02f, 1);
        rlVertex2f(px, 120); rlVertex2f(px + 10, 120); rlVertex2f(px + 14, 600); rlVertex2f(px + 4, 600);
        rlEnd();
    }
    rlBegin(RL_LINES);
    rlColor4f(0.7f, 0.55f, 0.1f, 1); rlSetLineWidth(3);
    rlVertex2f(720, 120); rlVertex2f(720, 600);
    rlEnd();

    // Top valance
    rlBegin(RL_QUADS);
    rlColor4f(0.22f, 0.03f, 0.03f, 1);
    rlVertex2f(0, 550); rlVertex2f(800, 550); rlVertex2f(800, 600); rlVertex2f(0, 600);
    rlEnd();
    // Fringe
    rlBegin(RL_LINES);
    rlColor4f(0.7f, 0.5f, 0.08f, 1); rlSetLineWidth(1);
    for (int i = 0; i < 40; i++) {
        float fx = 10.0f + i * 20;
        float flen = 12 + sinf(t * 0.02f + i * 0.4f) * 4;
        rlVertex2f(fx, 550); rlVertex2f(fx, 550 - flen);
    }
    rlEnd();
    rlBegin(RL_LINES);
    rlColor4f(0.8f, 0.6f, 0.1f, 1); rlSetLineWidth(2);
    rlVertex2f(0, 550); rlVertex2f(800, 550);
    rlEnd();

    // Skull decoration
    float ckx = 400, cky = 575;
    Drawing::circulo(ckx, cky + 5, 14, 20, CFloat(0.75f, 0.7f, 0.6f));
    rlBegin(RL_QUADS);
    rlColor4f(0.75f, 0.7f, 0.6f, 1);
    rlVertex2f(ckx - 10, cky - 5); rlVertex2f(ckx + 10, cky - 5);
    rlVertex2f(ckx + 8, cky - 14); rlVertex2f(ckx - 8, cky - 14);
    rlEnd();
    Drawing::circulo(ckx - 5, cky + 7, 3, 8, CFloat(0.05f, 0.02f, 0.02f));
    Drawing::circulo(ckx + 5, cky + 7, 3, 8, CFloat(0.05f, 0.02f, 0.02f));

    Drawing::antorchaGrande(100, 140, t);
    Drawing::antorchaGrande(700, 140, t);

    // Theater particles
    if (gs.controlesOpciones[3].valor) {
        for (auto& p : gs.particulasTeatro) {
            float vn = p.vida / p.vidaMax;
            float al = (vn < 0.2f) ? vn / 0.2f : (vn > 0.8f) ? (1.0f - vn) / 0.2f : 1.0f;
            Drawing::circulo(p.x, p.y, 1.0f, 4, CFloat(p.r * al, p.g * al, p.b * al, al * 0.8f));
        }
    }
}

// ============================================================
// ORGAN ROOM BACKGROUND (Music)
// ============================================================
void Background::organ(GameState& gs) {
    float t = gs.tiempo;
    rlBegin(RL_QUADS);
    rlColor4f(0.01f, 0.01f, 0.06f, 1); rlVertex2f(0, 0);
    rlColor4f(0.02f, 0.01f, 0.08f, 1); rlVertex2f(800, 0);
    rlColor4f(0.04f, 0.02f, 0.12f, 1); rlVertex2f(800, 600);
    rlColor4f(0.02f, 0.01f, 0.08f, 1); rlVertex2f(0, 600);
    rlEnd();

    // Rose window
    float rx = 400, ry = 555, rr = 55;
    float hp = 0.06f + 0.03f * sinf(t * 0.008f);
    Drawing::circulo(rx, ry, rr + 30, 40, CFloat(0.3f, 0.1f, 0.6f, hp));
    Drawing::circulo(rx, ry, rr, 40, CFloat(0.5f, 0.2f, 0.9f, 0.25f));
    Drawing::circulo(rx, ry, rr - 12, 40, CFloat(0.7f, 0.3f, 1.0f, 0.15f));

    // Rose segments
    rlBegin(RL_TRIANGLES);
    for (int i = 0; i < 12; i++) {
        float a1 = i * 2.0f * M_PI / 12;
        float a2 = (i + 1) * 2.0f * M_PI / 12;
        float r2 = rr - 4;
        float pr = 0.35f + 0.15f * sinf(t * 0.01f + i * 0.5f);
        float pg = 0.1f + 0.08f * sinf(t * 0.013f + i * 0.3f);
        float pb = 0.7f + 0.2f * sinf(t * 0.009f + i * 0.7f);
        rlColor4f(pr, pg, pb, 0.4f);
        rlVertex2f(rx, ry);
        rlVertex2f(rx + cosf(a1) * r2, ry + sinf(a1) * r2);
        rlVertex2f(rx + cosf(a2) * r2, ry + sinf(a2) * r2);
    }
    rlEnd();
    Drawing::circuloLinea(rx, ry, rr, 40, CFloat(0.5f, 0.3f, 0.8f));
    Drawing::circuloLinea(rx, ry, rr - 12, 40, CFloat(0.5f, 0.3f, 0.8f));
    rlBegin(RL_LINES);
    rlColor4f(0.6f, 0.4f, 0.9f, 1);
    rlVertex2f(rx - rr, ry); rlVertex2f(rx + rr, ry);
    rlVertex2f(rx, ry - rr); rlVertex2f(rx, ry + rr);
    rlEnd();

    // Gothic windows
    for (int w = 0; w < 5; w++) {
        float wx = 80.0f + w * 160, wbase = 100, wheight = 320, ww = 60;
        float wr = 0.05f + 0.03f * sinf(t * 0.006f + w * 0.8f);
        float wg = 0.02f + 0.02f * sinf(t * 0.009f + w * 0.5f);
        float wb = 0.12f + 0.06f * sinf(t * 0.007f + w * 1.1f);
        // Window arch
        rlBegin(RL_TRIANGLES);
        rlColor4f(wr, wg, wb, 0.5f);
        for (int j = 0; j < 20; j++) {
            float a1 = j * M_PI / 20; float a2 = (j + 1) * M_PI / 20;
            rlVertex2f(wx + ww / 2, wbase + wheight);
            rlVertex2f(wx + ww / 2 + cosf(a1) * ww / 2, wbase + wheight - 10 + sinf(a1) * ww / 2);
            rlVertex2f(wx + ww / 2 + cosf(a2) * ww / 2, wbase + wheight - 10 + sinf(a2) * ww / 2);
        }
        rlEnd();
        rlBegin(RL_QUADS);
        rlColor4f(wr, wg, wb, 0.5f);
        rlVertex2f(wx, wbase); rlVertex2f(wx + ww, wbase);
        rlVertex2f(wx + ww, wbase + wheight - ww / 2); rlVertex2f(wx, wbase + wheight - ww / 2);
        rlEnd();
        // Window frame
        rlBegin(RL_LINES);
        rlColor4f(0.15f, 0.08f, 0.2f, 1); rlSetLineWidth(2);
        rlVertex2f(wx, wbase); rlVertex2f(wx, wbase + wheight - ww / 2);
        rlVertex2f(wx + ww, wbase); rlVertex2f(wx + ww, wbase + wheight - ww / 2);
        rlVertex2f(wx + ww / 2, wbase); rlVertex2f(wx + ww / 2, wbase + wheight - ww / 2);
        rlVertex2f(wx, wbase + wheight * 0.45f); rlVertex2f(wx + ww, wbase + wheight * 0.45f);
        rlEnd();
    }

    // Organ pipes (left)
    auto tubo = [](float x, float base, float h, float w) {
        rlBegin(RL_QUADS);
        rlColor4f(0.2f, 0.15f, 0.25f, 1);
        rlVertex2f(x, base); rlVertex2f(x + w, base); rlVertex2f(x + w, base + h); rlVertex2f(x, base + h);
        rlColor4f(0.35f, 0.28f, 0.42f, 1);
        rlVertex2f(x + 2, base + h * 0.1f); rlVertex2f(x + w * 0.3f, base + h * 0.1f);
        rlVertex2f(x + w * 0.3f, base + h * 0.9f); rlVertex2f(x + 2, base + h * 0.9f);
        rlEnd();
        rlBegin(RL_LINES);
        rlColor4f(0.45f, 0.35f, 0.55f, 1);
        rlVertex2f(x, base); rlVertex2f(x + w, base);
        rlVertex2f(x + w, base); rlVertex2f(x + w, base + h);
        rlVertex2f(x + w, base + h); rlVertex2f(x, base + h);
        rlVertex2f(x, base + h); rlVertex2f(x, base);
        rlEnd();
        Drawing::circulo(x + w / 2, base + h, w / 2, 12, CFloat(0.3f, 0.22f, 0.38f));
    };
    tubo(15, 100, 420, 28); tubo(47, 100, 370, 24); tubo(75, 100, 310, 20);
    tubo(99, 100, 260, 18); tubo(121, 100, 210, 16); tubo(141, 100, 175, 14);
    tubo(757, 100, 420, 28); tubo(729, 100, 370, 24); tubo(705, 100, 310, 20);
    tubo(683, 100, 260, 18); tubo(663, 100, 210, 16); tubo(645, 100, 175, 14);

    // Floor platform
    rlBegin(RL_QUADS);
    rlColor4f(0.12f, 0.08f, 0.18f, 1);
    rlVertex2f(0, 95); rlVertex2f(800, 95); rlVertex2f(800, 105); rlVertex2f(0, 105);
    rlColor4f(0.06f, 0.04f, 0.09f, 1);
    rlVertex2f(0, 0); rlVertex2f(800, 0); rlVertex2f(800, 100); rlVertex2f(0, 100);
    rlEnd();
    rlBegin(RL_LINES);
    rlColor4f(0.3f, 0.2f, 0.4f, 1); rlSetLineWidth(2);
    rlVertex2f(0, 105); rlVertex2f(800, 105);
    rlEnd();

    // Music note particles
    Cancion& cact = gs.canciones[gs.cancionActual];
    if (gs.controlesOpciones[3].valor) {
        for (auto& p : gs.particulasNotas) {
            float vn = p.vida / p.vidaMax;
            float al = (vn < 0.15f) ? vn / 0.15f : (vn > 0.75f) ? (1.0f - vn) / 0.25f : 1.0f;
            rlPushMatrix();
            rlTranslatef(p.x, p.y, 0);
            rlRotatef(p.rot, 0, 0, 1);
            if (p.tipo == 0) {
                Drawing::circulo(0, 0, p.size, 8, CFloat(cact.r, cact.g, cact.b, al * 0.8f));
                rlBegin(RL_LINES);
                rlColor4f(cact.r, cact.g, cact.b, al * 0.8f);
                rlVertex2f(p.size * 0.8f, 0); rlVertex2f(p.size * 0.8f, p.size * 3);
                rlVertex2f(p.size * 0.8f, p.size * 3); rlVertex2f(p.size * 2.2f, p.size * 2);
                rlEnd();
            } else if (p.tipo == 1) {
                Drawing::circuloLinea(0, 0, p.size, 10, CFloat(cact.r, cact.g, cact.b, al * 0.6f));
            } else {
                rlBegin(RL_QUADS);
                rlColor4f(cact.r * 0.7f, cact.g * 0.7f, cact.b * 0.7f, al * 0.5f);
                rlVertex2f(-p.size, -p.size * 0.4f); rlVertex2f(p.size, -p.size * 0.4f);
                rlVertex2f(p.size, p.size * 0.4f); rlVertex2f(-p.size, p.size * 0.4f);
                rlEnd();
            }
            rlPopMatrix();
        }
    }
}
