#include "Menu_Combate.h"
#include <cstdlib>
#include <cmath>
#include <string>
#include "rlgl.h"


namespace Archon_PID {

    // ====================================================================
    //  Helpers de dibujo — replican el estilo de ScreensNew (drawPanel)
    //  adaptados a coordenadas de pantalla (Y-down, top-left origin).
    // ====================================================================

    // Panel con sombra, borde doble y esquinas decorativas.
    static void drawPanelSC(float x, float y, float w, float h,
        float br, float bg, float bb,
        float fr, float fg, float fb,
        float alpha = 0.85f)
    {
        // Sombra desplazada
        rlBegin(RL_QUADS);
        rlColor4f(0, 0, 0, 0.5f);
        rlVertex2f(x + 4, y + 4); rlVertex2f(x + w + 4, y + 4);
        rlVertex2f(x + w + 4, y + h + 4); rlVertex2f(x + 4, y + h + 4);
        rlEnd();
        // Fondo
        rlBegin(RL_QUADS);
        rlColor4f(fr, fg, fb, alpha);
        rlVertex2f(x, y); rlVertex2f(x + w, y);
        rlVertex2f(x + w, y + h); rlVertex2f(x, y + h);
        rlEnd();
        // Borde exterior
        rlSetLineWidth(2.5f);
        rlBegin(RL_LINES);
        rlColor4f(br, bg, bb, 1);
        rlVertex2f(x, y);     rlVertex2f(x + w, y);
        rlVertex2f(x + w, y);   rlVertex2f(x + w, y + h);
        rlVertex2f(x + w, y + h); rlVertex2f(x, y + h);
        rlVertex2f(x, y + h);   rlVertex2f(x, y);
        rlEnd();
        // Borde interior (mas fino, medio transparente)
        rlSetLineWidth(1.f);
        rlBegin(RL_LINES);
        rlColor4f(br * 0.6f, bg * 0.6f, bb * 0.6f, 0.5f);
        rlVertex2f(x + 5, y + 5);     rlVertex2f(x + w - 5, y + 5);
        rlVertex2f(x + w - 5, y + 5);   rlVertex2f(x + w - 5, y + h - 5);
        rlVertex2f(x + w - 5, y + h - 5); rlVertex2f(x + 5, y + h - 5);
        rlVertex2f(x + 5, y + h - 5);   rlVertex2f(x + 5, y + 5);
        rlEnd();
        // Triangulos en las esquinas
        float cs = 12.f;
        rlBegin(RL_TRIANGLES);
        rlColor4f(br, bg, bb, 0.8f);
        rlVertex2f(x, y);       rlVertex2f(x + cs, y);     rlVertex2f(x, y + cs);
        rlVertex2f(x + w, y);     rlVertex2f(x + w - cs, y);   rlVertex2f(x + w, y + cs);
        rlVertex2f(x, y + h);     rlVertex2f(x + cs, y + h);   rlVertex2f(x, y + h - cs);
        rlVertex2f(x + w, y + h);   rlVertex2f(x + w - cs, y + h); rlVertex2f(x + w, y + h - cs);
        rlEnd();
    }

    // Separador horizontal con diamante central.
    static void drawSeparadorSC(float x1, float y, float x2, float r, float g, float b)
    {
        rlSetLineWidth(1.5f);
        rlBegin(RL_LINES);
        rlColor4f(r, g, b, 1);
        rlVertex2f(x1, y); rlVertex2f(x2, y);
        rlEnd();
        float cx = (x1 + x2) / 2;
        rlBegin(RL_LINES);
        rlColor4f(r, g, b, 1);
        rlVertex2f(cx, y - 6); rlVertex2f(cx + 7, y);
        rlVertex2f(cx + 7, y); rlVertex2f(cx, y + 6);
        rlVertex2f(cx, y + 6); rlVertex2f(cx - 7, y);
        rlVertex2f(cx - 7, y); rlVertex2f(cx, y - 6);
        rlEnd();
    }

    // Colores del bando
    static void bandoColor(int bando, float& r, float& g, float& b) {
        if (bando == 0) { r = 0.9f; g = 0.85f; b = 0.25f; }
        else { r = 0.5f; g = 0.1f;  b = 0.8f; }
    }

    // ====================================================================
    //  Fondo medieval procedural — muro de piedra con antorchas y banderines
    // ====================================================================
    static void drawFondoMedieval(int W, int H, float tiempo)
    {
        // --- Muro de piedra ---
        // Base oscura
        rlBegin(RL_QUADS);
        rlColor4f(0.08f, 0.06f, 0.04f, 1);
        rlVertex2f(0, 0); rlVertex2f((float)W, 0);
        rlVertex2f((float)W, (float)H); rlVertex2f(0, (float)H);
        rlEnd();

        // Bloques de piedra (patron pseudo-aleatorio basado en posicion)
        for (int row = 0; row < H + 40; row += 40) {
            int offset = ((row / 40) % 2) * 35;
            for (int col = -40; col < W + 40; col += 70) {
                float bx = (float)(col + offset);
                float by = (float)row;
                // Variacion de brillo por bloque
                unsigned int seed = (unsigned int)(bx * 73 + by * 137);
                float v = 0.06f + (seed % 100) * 0.0004f;
                rlBegin(RL_QUADS);
                rlColor4f(v + 0.02f, v, v - 0.01f, 1);
                rlVertex2f(bx + 2, by + 2);
                rlVertex2f(bx + 66, by + 2);
                rlVertex2f(bx + 66, by + 36);
                rlVertex2f(bx + 2, by + 36);
                rlEnd();
                // Juntas (lineas entre bloques)
                rlSetLineWidth(1);
                rlBegin(RL_LINES);
                rlColor4f(0.03f, 0.02f, 0.01f, 0.8f);
                rlVertex2f(bx, by); rlVertex2f(bx + 68, by);
                rlVertex2f(bx, by); rlVertex2f(bx, by + 38);
                rlEnd();
            }
        }

        // --- Vigas de madera horizontales ---
        float vigaY1 = 10.f, vigaY2 = H - 16.f;
        rlBegin(RL_QUADS);
        rlColor4f(0.18f, 0.1f, 0.04f, 1);
        rlVertex2f(0, vigaY1); rlVertex2f((float)W, vigaY1);
        rlVertex2f((float)W, vigaY1 + 8); rlVertex2f(0, vigaY1 + 8);
        rlVertex2f(0, vigaY2); rlVertex2f((float)W, vigaY2);
        rlVertex2f((float)W, vigaY2 + 8); rlVertex2f(0, vigaY2 + 8);
        rlEnd();
        // Brillo superior de la viga
        rlBegin(RL_QUADS);
        rlColor4f(0.25f, 0.15f, 0.06f, 0.5f);
        rlVertex2f(0, vigaY1); rlVertex2f((float)W, vigaY1);
        rlVertex2f((float)W, vigaY1 + 2); rlVertex2f(0, vigaY1 + 2);
        rlVertex2f(0, vigaY2); rlVertex2f((float)W, vigaY2);
        rlVertex2f((float)W, vigaY2 + 2); rlVertex2f(0, vigaY2 + 2);
        rlEnd();

        // --- Antorchas en las paredes ---
        float antorchaX[] = { 50.f, W - 50.f, W / 2.f };
        for (int a = 0; a < 3; a++) {
            float ax = antorchaX[a], ay = 70.f;
            // Soporte de la antorcha
            rlBegin(RL_QUADS);
            rlColor4f(0.22f, 0.12f, 0.04f, 1);
            rlVertex2f(ax - 3, ay); rlVertex2f(ax + 3, ay);
            rlVertex2f(ax + 3, ay + 30); rlVertex2f(ax - 3, ay + 30);
            // Base del soporte
            rlColor4f(0.3f, 0.2f, 0.08f, 1);
            rlVertex2f(ax - 8, ay + 28); rlVertex2f(ax + 8, ay + 28);
            rlVertex2f(ax + 8, ay + 34); rlVertex2f(ax - 8, ay + 34);
            rlEnd();

            // Llama exterior
            float fuego = sinf(tiempo * 0.04f + a * 2.1f) * 4;
            float fuego2 = sinf(tiempo * 0.06f + a * 1.7f) * 3;
            rlBegin(RL_TRIANGLES);
            rlColor4f(1.f, 0.4f, 0.0f, 0.9f);
            rlVertex2f(ax, ay - 20 + fuego);
            rlVertex2f(ax - 8, ay); rlVertex2f(ax + 8, ay);
            // Llama interior
            rlColor4f(1.f, 0.85f, 0.2f, 0.95f);
            rlVertex2f(ax, ay - 14 + fuego2);
            rlVertex2f(ax - 5, ay); rlVertex2f(ax + 5, ay);
            rlEnd();

            // Brillo de la antorcha (circulo de luz)
            for (int r = 3; r >= 0; r--) {
                float rad = 30.f + r * 20.f;
                float alpha = 0.04f - r * 0.008f;
                rlBegin(RL_TRIANGLES);
                for (int s = 0; s < 16; s++) {
                    float a1 = s * 2.f * 3.14159f / 16;
                    float a2 = (s + 1) * 2.f * 3.14159f / 16;
                    rlColor4f(1.f, 0.6f, 0.1f, alpha);
                    rlVertex2f(ax, ay - 5);
                    rlVertex2f(ax + cosf(a1) * rad, ay - 5 + sinf(a1) * rad);
                    rlVertex2f(ax + cosf(a2) * rad, ay - 5 + sinf(a2) * rad);
                }
                rlEnd();
            }
        }

        // --- Banderines colgantes ---
        float bannerX[] = { W * 0.25f, W * 0.75f };
        float bannerColors[][3] = { {0.7f, 0.1f, 0.05f}, {0.1f, 0.1f, 0.6f} };
        for (int b = 0; b < 2; b++) {
            float bx = bannerX[b], by = 22.f;
            float sway = sinf(tiempo * 0.005f + b * 3.f) * 4;
            float bw = 30, bh = 55;
            // Tela del banner
            rlBegin(RL_QUADS);
            rlColor4f(bannerColors[b][0], bannerColors[b][1], bannerColors[b][2], 0.9f);
            rlVertex2f(bx - bw / 2, by);
            rlVertex2f(bx + bw / 2, by);
            rlVertex2f(bx + bw / 2 + sway, by + bh);
            rlVertex2f(bx - bw / 2 + sway, by + bh);
            rlEnd();
            // Punta triangular
            rlBegin(RL_TRIANGLES);
            rlColor4f(bannerColors[b][0], bannerColors[b][1], bannerColors[b][2], 0.9f);
            rlVertex2f(bx - bw / 2 + sway, by + bh);
            rlVertex2f(bx + bw / 2 + sway, by + bh);
            rlVertex2f(bx + sway, by + bh + 18);
            rlEnd();
            // Borde dorado del banner
            rlSetLineWidth(1.5f);
            rlBegin(RL_LINES);
            rlColor4f(0.7f, 0.55f, 0.1f, 0.8f);
            rlVertex2f(bx - bw / 2, by); rlVertex2f(bx - bw / 2 + sway, by + bh);
            rlVertex2f(bx + bw / 2, by); rlVertex2f(bx + bw / 2 + sway, by + bh);
            rlVertex2f(bx - bw / 2 + sway, by + bh); rlVertex2f(bx + sway, by + bh + 18);
            rlVertex2f(bx + bw / 2 + sway, by + bh); rlVertex2f(bx + sway, by + bh + 18);
            rlVertex2f(bx - bw / 2, by); rlVertex2f(bx + bw / 2, by);
            rlEnd();
            // Simbolo en el banner (rombo)
            float sy = by + bh / 2 + sway * 0.3f;
            rlBegin(RL_LINES);
            rlColor4f(0.8f, 0.65f, 0.15f, 0.9f);
            rlVertex2f(bx, sy - 8); rlVertex2f(bx + 6, sy);
            rlVertex2f(bx + 6, sy); rlVertex2f(bx, sy + 8);
            rlVertex2f(bx, sy + 8); rlVertex2f(bx - 6, sy);
            rlVertex2f(bx - 6, sy); rlVertex2f(bx, sy - 8);
            rlEnd();
        }

        // --- Cadenas decorativas (entre antorchas) ---
        for (int c = 0; c < 2; c++) {
            float cx1 = antorchaX[c], cx2 = antorchaX[c + 1];
            float cy = 50.f;
            int segs = 12;
            rlSetLineWidth(1.5f);
            rlBegin(RL_LINES);
            rlColor4f(0.25f, 0.2f, 0.15f, 0.6f);
            for (int s = 0; s < segs; s++) {
                float t1 = (float)s / segs, t2 = (float)(s + 1) / segs;
                float x1 = cx1 + (cx2 - cx1) * t1;
                float x2 = cx1 + (cx2 - cx1) * t2;
                float sag1 = sinf(t1 * 3.14159f) * 20;
                float sag2 = sinf(t2 * 3.14159f) * 20;
                rlVertex2f(x1, cy + sag1); rlVertex2f(x2, cy + sag2);
            }
            rlEnd();
        }
    }

    // ====================================================================
    //  DrawSeleccion — Dibuja el panel de seleccion de un jugador
    // ====================================================================

    void Menu_Combate::DrawSeleccion(int player, int cx, int cy, int sel, bool listo)
    {
        int bando = (player == 1) ? bandoP1 : bandoP2;
        Texture2D** imgs = (bando == 0) ? imagenes_claro : imagenes_oscuro;
        const Pj_info** pjs = (bando == 0) ? personajes_claro : personajes_oscuro;

        float br, bg, bb;
        bandoColor(bando, br, bg, bb);

        float w = 430.f, h = 400.f;
        float x = cx - w / 2.f, y = cy - h / 2.f;

        // Panel con estilo ARCHON
        if (listo) {
            drawPanelSC(x, y, w, h, 0.3f, 0.8f, 0.3f, 0.02f, 0.05f, 0.02f, 0.92f);
        }
        else {
            float al = 0.6f + 0.4f * sinf((float)GetTime() * 2.f);
            drawPanelSC(x, y, w, h, br * al, bg * al, bb * al, 0.02f, 0.03f, 0.06f, 0.92f);
        }

        // Etiqueta del jugador
        const char* lbl = (player == 1) ? "JUGADOR 1" : "JUGADOR 2";
        DrawText(lbl, cx - MeasureText(lbl, 18) / 2, (int)(y + 10), 18, colores[player - 1]);

        // Etiqueta del equipo
        const char* teamLbl = (bando == 0) ? "FUERZAS DE LUZ" : "FUERZAS OSCURAS";
        Color teamCol = (bando == 0) ? Color{ 230,217,64,255 } : Color{ 128,26,204,255 };
        DrawText(teamLbl, cx - MeasureText(teamLbl, 12) / 2, (int)(y + 30), 12, teamCol);

        // Separador bajo titulo
        drawSeparadorSC(x + 10, y + 48, x + w - 10, br * 0.7f, bg * 0.7f, bb * 0.7f);

        // Sprite grande del personaje
        Texture2D* tx = imgs[sel];
        float scale = 180.f / tx->height;
        DrawTextureEx(*tx,
            { cx - tx->width * scale / 2.f, y + 56 },
            0.f, scale, listo ? WHITE : ColorAlpha(WHITE, 0.75f));

        // Nombre del personaje
        std::string_view nombre = pjs[sel]->nombre;
        int nameY = (int)(y + 240);
        Color nameCol = listo ? Color{ 77,230,77,255 } : Color{
            (unsigned char)(br * 255), (unsigned char)(bg * 255), (unsigned char)(bb * 255), 255 };
        DrawText(nombre.data(), cx - MeasureText(nombre.data(), 20) / 2, nameY, 20, nameCol);

        // Separador antes de miniaturas
        drawSeparadorSC(x + 10, y + h - 78, x + w - 10, br * 0.5f, bg * 0.5f, bb * 0.5f);

        // Fila de miniaturas
        int iconSize = 48, gap = 4;
        int totalW = NUM_PJS * (iconSize + gap) - gap;
        int startX = cx - totalW / 2;
        for (int i = 0; i < NUM_PJS; i++) {
            float ix = (float)(startX + i * (iconSize + gap));
            float iy = y + h - 72.f;
            bool es = (i == sel);

            // Fondo miniatura
            rlBegin(RL_QUADS);
            rlColor4f(es ? br * 0.3f : 0.05f,
                es ? bg * 0.3f : 0.05f,
                es ? bb * 0.3f : 0.05f, 0.9f);
            rlVertex2f(ix, iy);
            rlVertex2f(ix + iconSize, iy);
            rlVertex2f(ix + iconSize, iy + iconSize);
            rlVertex2f(ix, iy + iconSize);
            rlEnd();

            // Borde miniatura
            rlSetLineWidth(es ? 2.f : 1.f);
            rlBegin(RL_LINES);
            rlColor4f(br * (es ? 1.f : 0.3f), bg * (es ? 1.f : 0.3f), bb * (es ? 1.f : 0.3f), 1.f);
            rlVertex2f(ix, iy);              rlVertex2f(ix + iconSize, iy);
            rlVertex2f(ix + iconSize, iy);     rlVertex2f(ix + iconSize, iy + iconSize);
            rlVertex2f(ix + iconSize, iy + iconSize); rlVertex2f(ix, iy + iconSize);
            rlVertex2f(ix, iy + iconSize);     rlVertex2f(ix, iy);
            rlEnd();

            // Sprite miniatura
            Texture2D* ic = imgs[i];
            float s = (float)iconSize / ic->height;
            DrawTextureEx(*ic, { ix, iy }, 0.f, s, es ? WHITE : DARKGRAY);
        }

        // Indicador LISTO!
        if (listo) {
            const char* r = "LISTO!";
            DrawText(r, cx - MeasureText(r, 24) / 2, (int)(y - 30), 24, GREEN);
        }
    }

    // ====================================================================
    //  DrawPanelIA — Dibuja el panel del enemigo IA
    // ====================================================================

    void Menu_Combate::DrawPanelIA(int cx, int cy)
    {
        int bandoIA = bandoP2;
        auto& imgs = (bandoIA == 0) ? imagenes_claro : imagenes_oscuro;
        auto& pjs = (bandoIA == 0) ? personajes_claro : personajes_oscuro;

        float br, bg, bb;
        bandoColor(bandoIA, br, bg, bb);

        float w = 430.f, h = 400.f;
        float x = cx - w / 2.f, y = cy - h / 2.f;

        // Panel: rojo si P1 listo, gris si no
        if (P1Listo) {
            drawPanelSC(x, y, w, h, 0.9f, 0.2f, 0.1f, 0.06f, 0.02f, 0.02f, 0.92f);
        }
        else {
            drawPanelSC(x, y, w, h, 0.3f, 0.3f, 0.35f, 0.02f, 0.03f, 0.06f, 0.92f);
        }

        // Etiqueta
        const char* lbl = "ENEMIGO IA";
        DrawText(lbl, cx - MeasureText(lbl, 18) / 2, (int)(y + 10), 18, Color{ 230,51,26,255 });

        // Etiqueta del equipo
        const char* teamLbl = (bandoIA == 0) ? "FUERZAS DE LUZ" : "FUERZAS OSCURAS";
        Color teamCol = (bandoIA == 0) ? Color{ 230,217,64,255 } : Color{ 128,26,204,255 };
        DrawText(teamLbl, cx - MeasureText(teamLbl, 12) / 2, (int)(y + 30), 12, teamCol);

        // Separador
        float pbr = P1Listo ? 0.9f : 0.3f;
        float pbg = P1Listo ? 0.2f : 0.3f;
        float pbb = P1Listo ? 0.1f : 0.35f;
        drawSeparadorSC(x + 10, y + 48, x + w - 10, pbr * 0.7f, pbg * 0.7f, pbb * 0.7f);

        if (!P1Listo) {
            // Personaje IA aun no revelado
            const char* sig = "???";
            DrawText(sig, cx - MeasureText(sig, 80) / 2, cy - 30, 80, DARKGRAY);
        }
        else {
            // Sprite del personaje asignado a la IA
            Texture2D* tx = imgs[selP2];
            float scale = 180.f / tx->height;
            DrawTextureEx(*tx,
                { cx - tx->width * scale / 2.f, y + 56 },
                0.f, scale, WHITE);

            // Nombre
            std::string_view nombre = pjs[selP2]->nombre;
            int nameY = (int)(y + 240);
            DrawText(nombre.data(), cx - MeasureText(nombre.data(), 20) / 2, nameY, 20, Color{ 230,51,26,255 });

            // Separador antes de miniaturas
            drawSeparadorSC(x + 10, y + h - 78, x + w - 10, pbr * 0.5f, pbg * 0.5f, pbb * 0.5f);

            // Miniaturas del equipo IA
            int iconSize = 48, gap = 4;
            int totalW = NUM_PJS * (iconSize + gap) - gap;
            int startX = cx - totalW / 2;
            for (int i = 0; i < NUM_PJS; i++) {
                float ix = (float)(startX + i * (iconSize + gap));
                float iy = y + h - 72.f;
                bool es = (i == selP2);

                rlBegin(RL_QUADS);
                rlColor4f(es ? 0.15f : 0.05f, es ? 0.04f : 0.05f, es ? 0.02f : 0.05f, 0.9f);
                rlVertex2f(ix, iy); rlVertex2f(ix + iconSize, iy);
                rlVertex2f(ix + iconSize, iy + iconSize); rlVertex2f(ix, iy + iconSize);
                rlEnd();

                rlSetLineWidth(es ? 2.f : 1.f);
                rlBegin(RL_LINES);
                rlColor4f(es ? 0.9f : 0.3f, es ? 0.2f : 0.3f, es ? 0.1f : 0.3f, 1.f);
                rlVertex2f(ix, iy);              rlVertex2f(ix + iconSize, iy);
                rlVertex2f(ix + iconSize, iy);     rlVertex2f(ix + iconSize, iy + iconSize);
                rlVertex2f(ix + iconSize, iy + iconSize); rlVertex2f(ix, iy + iconSize);
                rlVertex2f(ix, iy + iconSize);     rlVertex2f(ix, iy);
                rlEnd();

                float s = (float)iconSize / imgs[i]->height;
                DrawTextureEx(*imgs[i], { ix, iy }, 0.f, s, es ? WHITE : DARKGRAY);
            }
        }
    }

    // ====================================================================
    //  UpdateDosJugadores — Logica del modo 2 jugadores
    // ====================================================================

    bool Menu_Combate::UpdateDosJugadores()
    {
        if (IsKeyPressed(KEY_ESCAPE)) {
            quiereVolver = true;
            P1Listo = P2Listo = false;
            selP1 = selP2 = 0;
        }

        // Only start combat if both were already confirmed BEFORE this frame's click
        return p1WasReady && p2WasReady && IsMouseButtonPressed(MOUSE_BUTTON_LEFT);
    }

    // ====================================================================
    //  UpdateVsIA — Logica del modo VS IA
    // ====================================================================

    bool Menu_Combate::UpdateVsIA()
    {
        if (IsKeyPressed(KEY_ESCAPE)) {
            quiereVolver = true;
            P1Listo = P2Listo = false;
            selP1 = selP2 = 0;
        }

        return p1WasReady && IsMouseButtonPressed(MOUSE_BUTTON_LEFT);
    }

    // ====================================================================
    //  UPDATE / DRAW publicos
    // ====================================================================

    bool Menu_Combate::Update()
    {
        if (modo == DOS_JUGADORES)  return UpdateDosJugadores();
        return UpdateVsIA();
    }

    void Menu_Combate::Draw()
    {
        int W = GetScreenWidth();
        int H = GetScreenHeight();
        ClearBackground(BLACK);

        // Fondo medieval procedural
        drawFondoMedieval(W, H, (float)GetTime() * 50.f);

        // Panel contenedor exterior
        drawPanelSC(30.f, 15.f, W - 60.f, H - 30.f,
            0.65f, 0.5f, 0.08f, 0.04f, 0.03f, 0.01f, 0.88f);

        // Titulo con sombra
        const char* titulo = "SELECCION DE PERSONAJES";
        int tw = MeasureText(titulo, 24);
        DrawText(titulo, W / 2 - tw / 2 + 2, 26, 24, Color{ 64,51,13,255 });   // sombra
        DrawText(titulo, W / 2 - tw / 2, 24, 24, Color{ 242,209,51,255 });  // texto

        // Separador bajo titulo
        drawSeparadorSC(60.f, 54.f, W - 60.f, 0.55f, 0.4f, 0.07f);

        // Paneles de seleccion de personajes
        int panelCY = H / 2 + 10;
        DrawSeleccion(1, W / 4, panelCY, selP1, P1Listo);

        if (modo == DOS_JUGADORES)
            DrawSeleccion(2, W * 3 / 4, panelCY, selP2, P2Listo);
        else
            DrawPanelIA(W * 3 / 4, panelCY);

        // Texto "VS" grande con sombra
        const char* vs = "VS";
        int vsw = MeasureText(vs, 60);
        DrawText(vs, W / 2 - vsw / 2 + 3, H / 2 - 12, 60, Color{ 0,0,0,128 });
        DrawText(vs, W / 2 - vsw / 2, H / 2 - 15, 60, Color{ 153,128,26,255 });

        // Etiqueta de dificultad (solo en modo IA)
        if (modo == VS_IA) {
            std::string difText = std::string("Dificultad: ") + nombresDif[dificultad];
            int dtw = MeasureText(difText.c_str(), 16);
            DrawText(difText.c_str(), W / 2 - dtw / 2, H - 96, 16, coloresDif[dificultad]);
        }

        // Instrucciones de controles
        Color instrCol = Color{ 128,102,51,255 };
        Color instrCol2 = Color{ 89,77,38,255 };
        if (modo == DOS_JUGADORES) {
            const char* c1 = "CLICK IZQ = J1";
            const char* c2 = "CLICK DER = J2";
            DrawText(c1, W / 4 - MeasureText(c1, 14) / 2, H - 50, 14, instrCol);
            DrawText(c2, W * 3 / 4 - MeasureText(c2, 14) / 2, H - 50, 14, instrCol);
        }
        else {
            const char* c1 = "Click para seleccionar personaje";
            DrawText(c1, W / 2 - MeasureText(c1, 14) / 2, H - 50, 14, instrCol);
        }
        const char* esc = "ESC = volver";
        DrawText(esc, W / 2 - MeasureText(esc, 12) / 2, H - 32, 12, instrCol2);

        // Back arrow button (screen coords, Y-down)
        {
            float bx = 50.f, by = 28.f;
            float mx = (float)GetMouseX(), my = (float)GetMouseY();
            // Visible box bounds
            float vx0 = bx - 10, vx1 = bx + 70, vy0 = by - 14, vy1 = by + 14;
            // Hover zone: same centre, 10px bigger on each side
            float pad = 10.f;
            bool hover = (mx > vx0 - pad && mx < vx1 + pad && my > vy0 - pad && my < vy1 + pad);
            float al = hover ? 0.9f : 0.45f;
            Color ac = { (unsigned char)(217 * al),(unsigned char)(179 * al),(unsigned char)(38 * al),255 };
            // Arrow shaft
            DrawLine((int)(bx + 2), (int)by, (int)(bx + 18), (int)by, ac);
            // Arrowhead
            DrawLine((int)(bx + 2), (int)by, (int)(bx + 10), (int)(by - 7), ac);
            DrawLine((int)(bx + 2), (int)by, (int)(bx + 10), (int)(by + 7), ac);
            if (hover) DrawTriangle(
                { bx + 2,by }, { bx + 10,by + 7 }, { bx + 10,by - 7 },
                { (unsigned char)(217 * al),(unsigned char)(179 * al),(unsigned char)(38 * al),60 });
            DrawText("VOLVER", (int)(bx + 22), (int)(by - 6), 12, ac);
            // Outline box (visible bounds, smaller than hover zone)
            DrawRectangleLines((int)vx0, (int)vy0, (int)(vx1 - vx0), (int)(vy1 - vy0),
                { (unsigned char)(217 * al),(unsigned char)(179 * al),(unsigned char)(38 * al),(unsigned char)(hover ? 180 : 80) });
        }

        // Mensaje parpadeante cuando ambos estan listos
        bool ambosListos = (modo == DOS_JUGADORES) ? (P1Listo && P2Listo) : P1Listo;
        if (ambosListos && (int)(GetTime() * 2) % 2 == 0) {
            const char* start = "CLICK PARA COMBATIR";
            int stw = MeasureText(start, 24);
            drawPanelSC(W / 2.f - stw / 2.f - 20.f, H - 126.f, stw + 40.f, 36.f,
                0.7f, 0.1f, 0.05f, 0.12f, 0.04f, 0.02f);
            DrawText(start, W / 2 - stw / 2, H - 120, 24, Color{ 255,230,128,255 });
        }
    }

    void Menu_Combate::Reset()
    {
        modo = DOS_JUGADORES;
        selP1 = selP2 = 0;
        P1Listo = P2Listo = false;
        dificultad = 1;
        bandoP1 = 0;
        bandoP2 = 1;
        quiereVolver = false;
    }

    void Menu_Combate::SetModo(bool ia, int bando1, int bando2, int difi)
    {
        modo = ia ? VS_IA : DOS_JUGADORES;
        bandoP1 = bando1;
        bandoP2 = bando2;
        dificultad = difi;
        selP1 = selP2 = 0;
        P1Listo = P2Listo = false;
        quiereVolver = false;
    }

    void Menu_Combate::HandleMouse()
    {
        // Snapshot ready state before this frame's clicks modify it
        p1WasReady = P1Listo;
        p2WasReady = P2Listo;

        bool clickL = IsMouseButtonPressed(MOUSE_BUTTON_LEFT);
        bool clickR = IsMouseButtonPressed(MOUSE_BUTTON_RIGHT);
        int W = GetScreenWidth(), H = GetScreenHeight();
        float mx = (float)GetMouseX(), my = (float)GetMouseY();

        // Back arrow click — zone 10px larger than visible box on each side
        if (clickL && mx > 30 && mx < 130 && my > 4 && my < 52) {
            quiereVolver = true;
            P1Listo = P2Listo = false;
            selP1 = selP2 = 0;
            return;
        }

        // Layout params (must match Draw — iconSize=48, panelH=400)
        float panelCY = H / 2.f + 10.f;
        float panelH = 400.f;
        float panelY = panelCY - panelH / 2.f;
        int iconSize = 48, gap = 4;
        int totalW = NUM_PJS * (iconSize + gap) - gap;
        float iconRow = panelY + panelH - 72.f;

        if (modo == DOS_JUGADORES) {
            // P1 panel thumbnails (left click hovers + confirms)
            float cx1 = W / 4.f;
            float startX = cx1 - totalW / 2.f;
            for (int i = 0; i < NUM_PJS; i++) {
                float ix = startX + i * (iconSize + gap);
                if (mx > ix && mx < ix + iconSize && my > iconRow && my < iconRow + iconSize) {
                    if (!P1Listo) selP1 = i;
                    if (clickL && !P1Listo) P1Listo = true;
                }
            }
            // P2 panel thumbnails (right click hovers + confirms)
            float cx2 = W * 3 / 4.f;
            startX = cx2 - totalW / 2.f;
            for (int i = 0; i < NUM_PJS; i++) {
                float ix = startX + i * (iconSize + gap);
                if (mx > ix && mx < ix + iconSize && my > iconRow && my < iconRow + iconSize) {
                    if (!P2Listo) selP2 = i;
                    if (clickR && !P2Listo) P2Listo = true;
                }
            }
            // Also allow left click on P1 big sprite, right click on P2 big sprite
            float sprW = 100, sprH = 180, sprY = panelY + 56;
            if (clickL && !P1Listo && mx > cx1 - sprW && mx < cx1 + sprW && my > sprY && my < sprY + sprH)
                P1Listo = true;
            if (clickR && !P2Listo && mx > cx2 - sprW && mx < cx2 + sprW && my > sprY && my < sprY + sprH)
                P2Listo = true;
        }
        else { // VS_IA — left click only
            float cx1 = W / 4.f;
            float startX = cx1 - totalW / 2.f;
            for (int i = 0; i < NUM_PJS; i++) {
                float ix = startX + i * (iconSize + gap);
                if (mx > ix && mx < ix + iconSize && my > iconRow && my < iconRow + iconSize) {
                    if (!P1Listo) selP1 = i;
                    if (clickL && !P1Listo) {
                        P1Listo = true;
                        P2Listo = true;
                        selP2 = rand() % NUM_PJS;
                    }
                }
            }
            // Click big sprite to confirm
            float sprW = 100, sprH = 180, sprY = panelY + 56;
            if (clickL && !P1Listo && mx > cx1 - sprW && mx < cx1 + sprW && my > sprY && my < sprY + sprH) {
                P1Listo = true;
                P2Listo = true;
                selP2 = rand() % NUM_PJS;
            }
        }
    }

}