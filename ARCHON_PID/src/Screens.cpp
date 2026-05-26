#include "Screens.h"
#include "Drawing.h"
#include "Background.h"
#include "Particles.h"
#include "rlgl.h"
#include "cuchau/Pj_info.h"

// Sprite textures for the encyclopedia (loaded on first use)
static Texture2D encicloSprites[16];  // [page] — one unique sprite per character
static bool      encicloSpritesLoaded = false;

static void cargarEncicloSprites() {
    if (encicloSpritesLoaded) return;
    // Light team (0-7), Dark team (8-15) — matches tropas[] order in GameState::init()
    const Pj_info* pjs[16] = {
        &MH, &Phoenix, &Golem, &Djinni, &Unicorn, &Valkyrie, &Archer, &Knight,
        &Platero, &ShapeShifter, &Troll, &Dragon, &Basilisk, &Banshee, &Manticore, &Goblin,
    };
    for (int p = 0; p < 16; p++) {
        encicloSprites[p] = LoadTexture(pjs[p]->Sprites[0].data());
    }
    encicloSpritesLoaded = true;
}

// Draw a texture sprite in the Y-up rlgl projection, centered at (cx, cy)
// Draw sprite fitted inside a fixed box (maxW x maxH), centered at (cx, cy)
static void drawEncicloSprite(int page, float cx, float cy, float maxW, float maxH) {
    Texture2D& tex = encicloSprites[page];
    if (tex.id == 0) return;
    float scaleX = maxW / tex.width;
    float scaleY = maxH / tex.height;
    float s = (scaleX < scaleY) ? scaleX : scaleY;  // fit, keep aspect ratio
    float w = tex.width  * s;
    float h = tex.height * s;
    Rectangle src  = { 0, 0, (float)tex.width, (float)tex.height };
    Rectangle dest = { 0, 0, w, h };
    rlPushMatrix();
    rlTranslatef(cx - w * 0.5f, cy + h * 0.5f, 0);
    rlScalef(1.0f, -1.0f, 1.0f);
    DrawTexturePro(tex, src, dest, {0, 0}, 0, WHITE);
    rlPopMatrix();
}

void Screens::menuPrincipal(GameState& gs) {
    float t = gs.tiempo;
    Background::castle(gs);
    Particles::drawStars(gs);
    Drawing::logo(t);
    Drawing::antorcha(100, 300, t);
    Drawing::antorcha(700, 300, t);

    for (int i = 0; i < (int)gs.opcionesMenu.size(); i++) {
        float y = 350 - i * 70.0f;
        float offX = std::min(gs.menuOffset + i * 20.0f, 0.0f);
        bool sel = (i == gs.opcionMenuSel);
        rlPushMatrix(); rlTranslatef(offX, 0, 0);
        if (sel) {
            // Animated border
            float bx = sinf(t * 0.01f) * 5, by = sinf(t * 0.01f) * 3;
            rlSetLineWidth(2);
            rlBegin(RL_LINES);
            rlColor4f(1, 0.1f, 0.2f, 1);
            rlVertex2f(250 + bx, y + 25 + by); rlVertex2f(600 + bx, y + 25 + by);
            rlVertex2f(600 + bx, y + 25 + by); rlVertex2f(640 + bx, y - 25 + by);
            rlVertex2f(640 + bx, y - 25 + by); rlVertex2f(290 + bx, y - 25 + by);
            rlVertex2f(290 + bx, y - 25 + by); rlVertex2f(250 + bx, y + 25 + by);
            rlEnd();
            Drawing::panelMenu(y);
            float esc = 1.3f + sinf(t * 0.005f) * 0.05f;
            rlPushMatrix(); rlTranslatef(320, y, 0); rlScalef(esc, esc, 1);
            Drawing::texto18(2, -7, gs.opcionesMenu[i], CFloat(0, 0, 0));
            Drawing::texto18(0, -5, gs.opcionesMenu[i], WHITE);
            rlPopMatrix();
            Drawing::cursorAnimado(290, y, t);
        } else {
            Drawing::texto18(320, y - 5, gs.opcionesMenu[i], CFloat(0.75f, 0.75f, 0.75f));
        }
        Color ic = sel ? WHITE : CFloat(0.7f, 0.7f, 0.7f);
        if (i == 0) Drawing::iconoEspada(300, y, ic);
        if (i == 1) Drawing::iconoLibro(300, y, ic);     // Cargar Partida
        if (i == 2) Drawing::iconoOpciones(300, y, ic);  // Opciones
        if (i == 3) Drawing::iconoLibro(300, y, ic);     // Enciclopedia
        if (i == 4) Drawing::iconoEspada(300, y, ic);    // Salir
        rlPopMatrix();
    }
    Drawing::instrucciones(260, 50, "W/S: Navegar    ENTER: Confirmar", t);
}

void Screens::menuOpciones(GameState& gs) {
    float t = gs.tiempo;
    Background::theater(gs);
    float ox = gs.opcionesOffset;

    // Panel background
    rlBegin(RL_QUADS);
    rlColor4f(0, 0, 0, 0.6f);
    rlVertex2f(108 + ox, 148); rlVertex2f(698 + ox, 148);
    rlVertex2f(698 + ox, 532); rlVertex2f(108 + ox, 532);
    rlColor4f(0.07f, 0.04f, 0.02f, 1);
    rlVertex2f(105 + ox, 150); rlVertex2f(695 + ox, 150);
    rlVertex2f(695 + ox, 530); rlVertex2f(105 + ox, 530);
    rlEnd();
    // Gold border
    rlSetLineWidth(3);
    rlBegin(RL_LINES);
    rlColor4f(0.65f, 0.5f, 0.08f, 1);
    rlVertex2f(105 + ox, 150); rlVertex2f(695 + ox, 150);
    rlVertex2f(695 + ox, 150); rlVertex2f(695 + ox, 530);
    rlVertex2f(695 + ox, 530); rlVertex2f(105 + ox, 530);
    rlVertex2f(105 + ox, 530); rlVertex2f(105 + ox, 150);
    rlEnd();

    // Title
    float pt = 1.0f + sinf(t * 0.007f) * 0.025f;
    rlPushMatrix();
    rlTranslatef(400 + ox, 508, 0); rlScalef(pt, pt, 1);
    Drawing::texto24(-72, -7, "OPCIONES", CFloat(0.4f, 0.0f, 0.0f));
    Drawing::texto24(-74, -5, "OPCIONES", CFloat(0.88f, 0.78f, 0.18f));
    rlPopMatrix();

    // Divider
    rlSetLineWidth(2);
    rlBegin(RL_LINES);
    rlColor4f(0.55f, 0.4f, 0.07f, 1);
    rlVertex2f(120 + ox, 492); rlVertex2f(680 + ox, 492);
    rlEnd();

    // Music button
    float btnY = 468;
    float btnPulse = 0.12f + 0.06f * sinf(t * 0.012f);
    rlBegin(RL_QUADS);
    rlColor4f(0.15f, 0.05f, 0.3f, btnPulse);
    rlVertex2f(115 + ox, btnY + 16); rlVertex2f(685 + ox, btnY + 16);
    rlVertex2f(685 + ox, btnY - 20); rlVertex2f(115 + ox, btnY - 20);
    rlEnd();
    rlBegin(RL_LINES);
    rlColor4f(0.6f, 0.3f, 0.9f, 1); rlSetLineWidth(1.5f);
    rlVertex2f(115 + ox, btnY + 16); rlVertex2f(685 + ox, btnY + 16);
    rlVertex2f(685 + ox, btnY + 16); rlVertex2f(685 + ox, btnY - 20);
    rlVertex2f(685 + ox, btnY - 20); rlVertex2f(115 + ox, btnY - 20);
    rlVertex2f(115 + ox, btnY - 20); rlVertex2f(115 + ox, btnY + 16);
    rlEnd();
    Drawing::iconoNota(140 + ox, btnY + 2, CFloat(0.8f, 0.6f, 1.0f));
    Drawing::texto18(160 + ox, btnY, "AJUSTES DE MUSICA", CFloat(0.8f, 0.6f, 1.0f));
    Drawing::texto12(590 + ox, btnY, "ENTER", CFloat(0.55f, 0.35f, 0.8f));

    // Options list
    const float startY = 435, stepY = 46;
    for (int i = 0; i < (int)gs.controlesOpciones.size(); i++) {
        float y = startY - i * stepY;
        OpcionControl& op = gs.controlesOpciones[i];
        bool sel = (i == gs.opcionOpcionesSel);
        if (sel) {
            float al = 0.15f + 0.08f * sinf(t * 0.015f);
            rlBegin(RL_QUADS);
            rlColor4f(0.8f, 0.1f, 0.05f, al);
            rlVertex2f(115 + ox, y + 16); rlVertex2f(685 + ox, y + 16);
            rlVertex2f(685 + ox, y - 24); rlVertex2f(115 + ox, y - 24);
            rlEnd();
            rlBegin(RL_LINES);
            rlColor4f(0.9f, 0.2f, 0.05f, 1); rlSetLineWidth(1.5f);
            rlVertex2f(115 + ox, y + 16); rlVertex2f(685 + ox, y + 16);
            rlVertex2f(685 + ox, y + 16); rlVertex2f(685 + ox, y - 24);
            rlVertex2f(685 + ox, y - 24); rlVertex2f(115 + ox, y - 24);
            rlVertex2f(115 + ox, y - 24); rlVertex2f(115 + ox, y + 16);
            rlEnd();
        }
        Color tc = sel ? CFloat(1.0f, 0.9f, 0.5f) : CFloat(0.65f, 0.55f, 0.35f);
        Drawing::texto18(130 + ox, y, op.nombre, tc);

        std::string valStr;
        if (op.esBool) valStr = (op.nombre == "Idioma") ? (op.valor == 0 ? "ES" : "EN") : (op.valor ? "ON" : "OFF");
        else valStr = std::to_string(op.valor);
        Color vc = sel ? CFloat(1, 1, 0.4f) : CFloat(0.6f, 0.5f, 0.3f);
        Drawing::texto18(638 + ox, y, valStr, vc);
        Drawing::barraSlider(320 + ox, y + 4, 300, op.valor, op.maxV, op.esBool);
        if (sel) Drawing::cursorAnimado(118 + ox, y + 4, t);
    }
    Drawing::instrucciones(148 + ox, 163, "W/S: Navegar    A/D: Cambiar valor    ENTER: Musica    ESC: Volver", t);
}

void Screens::menuMusica(GameState& gs) {
    float t = gs.tiempo;
    Background::organ(gs);
    float ox = gs.musicaOffset;
    Cancion& cact = gs.canciones[gs.cancionActual];

    // Main panel
    rlBegin(RL_QUADS);
    rlColor4f(0.0f, 0.0f, 0.0f, 0.75f);
    rlVertex2f(48 + ox, 118); rlVertex2f(758 + ox, 118);
    rlVertex2f(758 + ox, 530); rlVertex2f(48 + ox, 530);
    rlEnd();
    rlSetLineWidth(3);
    rlBegin(RL_LINES);
    rlColor4f(0.35f, 0.15f, 0.55f, 1);
    rlVertex2f(45 + ox, 115); rlVertex2f(755 + ox, 115);
    rlVertex2f(755 + ox, 115); rlVertex2f(755 + ox, 532);
    rlVertex2f(755 + ox, 532); rlVertex2f(45 + ox, 532);
    rlVertex2f(45 + ox, 532); rlVertex2f(45 + ox, 115);
    rlEnd();

    // Title
    float pt = 1.0f + sinf(t * 0.007f) * 0.02f;
    rlPushMatrix();
    rlTranslatef(400 + ox, 513, 0); rlScalef(pt, pt, 1);
    Drawing::texto24(-82, -7, "SALA DE ROLITAS", CFloat(cact.r * 0.5f, cact.g * 0.4f, cact.b * 0.5f));
    Drawing::texto24(-84, -5, "SALA DE ROLITAS", CFloat(cact.r, cact.g, cact.b));
    rlPopMatrix();

    // Divider
    rlSetLineWidth(2);
    rlBegin(RL_LINES);
    rlColor4f(cact.r * 0.6f, cact.g * 0.5f, cact.b * 0.7f, 1);
    rlVertex2f(60 + ox, 497); rlVertex2f(740 + ox, 497);
    rlEnd();

    // Spinning disc
    float diskX = 180 + ox, diskY = 340, diskR = 65;
    float diskRot = gs.reproduciendo ? t * 0.8f : 0;
    rlPushMatrix();
    rlTranslatef(diskX, diskY, 0);
    rlRotatef(diskRot, 0, 0, 1);
    Drawing::circulo(0, 0, diskR, 40, CFloat(0.08f, 0.05f, 0.12f));
    for (int s = 1; s <= 5; s++)
        Drawing::circuloLinea(0, 0, diskR * s / 6.0f, 36, CFloat(0.18f, 0.1f, 0.25f, 0.6f));
    Drawing::circulo(0, 0, diskR * 0.32f, 20, CFloat(cact.r * 0.7f, cact.g * 0.6f, cact.b * 0.8f));
    rlBegin(RL_LINES);
    rlColor4f(0.05f, 0.02f, 0.08f, 1);
    rlVertex2f(-diskR * 0.08f, 0); rlVertex2f(diskR * 0.08f, 0);
    rlVertex2f(0, -diskR * 0.08f); rlVertex2f(0, diskR * 0.08f);
    rlEnd();
    rlPopMatrix();
    Drawing::circuloLinea(diskX, diskY, diskR, 40, CFloat(cact.r * 0.5f, cact.g * 0.4f, cact.b * 0.7f));

    // Tonearm
    float agujaA = -0.4f + sinf(t * 0.001f) * 0.05f;
    rlSetLineWidth(2);
    rlBegin(RL_LINES);
    rlColor4f(0.7f, 0.65f, 0.75f, 1);
    rlVertex2f(diskX + diskR - 5, diskY + diskR + 15);
    rlVertex2f(diskX + cosf(agujaA) * (diskR * 0.5f), diskY + sinf(agujaA) * (diskR * 0.5f));
    rlEnd();
    Drawing::circulo(diskX + diskR - 5, diskY + diskR + 15, 5, 12, CFloat(0.8f, 0.7f, 0.85f));

    // Song info
    Drawing::texto18(60 + ox, 265, cact.titulo, CFloat(cact.r, cact.g, cact.b));
    Drawing::texto12(60 + ox, 248, cact.artista, CFloat(0.6f, 0.55f, 0.7f));
    Drawing::texto12(60 + ox, 232, "Suena en: " + cact.contexto, CFloat(0.4f, 0.35f, 0.5f));
    Drawing::texto12(60 + ox, 216, "Duracion: " + cact.duracion, CFloat(0.5f, 0.45f, 0.6f));

    // Progress bar
    float progX = 60 + ox, progY = 200, progW = 290;
    rlBegin(RL_QUADS);
    rlColor4f(0.1f, 0.06f, 0.18f, 1);
    rlVertex2f(progX, progY - 4); rlVertex2f(progX + progW, progY - 4);
    rlVertex2f(progX + progW, progY + 4); rlVertex2f(progX, progY + 4);
    rlEnd();
    float progFill = gs.progresoCancion * progW;
    rlBegin(RL_QUADS);
    rlColor4f(cact.r * 0.8f, cact.g * 0.7f, cact.b * 0.9f, 1);
    rlVertex2f(progX, progY - 3); rlVertex2f(progX + progFill, progY - 3);
    rlVertex2f(progX + progFill, progY + 3); rlVertex2f(progX, progY + 3);
    rlEnd();
    Drawing::circulo(progX + progFill, progY, 5, 10, WHITE);
    Drawing::texto12(progX, progY - 15, "0:00", CFloat(0.5f, 0.45f, 0.6f));
    Drawing::texto12(progX + progW - 25, progY - 15, cact.duracion, CFloat(0.5f, 0.45f, 0.6f));

    // Transport controls
    float ctrlY = 165, ctrlCX = 200 + ox, sp = 42;

    // Prev track
    Drawing::botonControl(ctrlCX - sp * 2, ctrlY, 16, false, 0.6f, 0.5f, 0.8f);
    rlBegin(RL_TRIANGLES);
    rlColor4f(0.7f, 0.6f, 0.9f, 1);
    rlVertex2f(ctrlCX - sp * 2 - 6, ctrlY);
    rlVertex2f(ctrlCX - sp * 2 + 5, ctrlY + 7);
    rlVertex2f(ctrlCX - sp * 2 + 5, ctrlY - 7);
    rlEnd();

    // Rewind
    Drawing::botonControl(ctrlCX - sp, ctrlY, 16, false, 0.6f, 0.5f, 0.8f);
    rlBegin(RL_TRIANGLES);
    rlColor4f(0.7f, 0.6f, 0.9f, 1);
    rlVertex2f(ctrlCX - sp + 2, ctrlY); rlVertex2f(ctrlCX - sp - 6, ctrlY + 6); rlVertex2f(ctrlCX - sp - 6, ctrlY - 6);
    rlVertex2f(ctrlCX - sp + 8, ctrlY); rlVertex2f(ctrlCX - sp, ctrlY + 6); rlVertex2f(ctrlCX - sp, ctrlY - 6);
    rlEnd();

    // Play/Pause
    Drawing::botonControl(ctrlCX, ctrlY, 22, true, cact.r, cact.g, cact.b);
    if (gs.reproduciendo) {
        rlBegin(RL_QUADS);
        rlColor4f(cact.r, cact.g, cact.b, 1);
        rlVertex2f(ctrlCX - 8, ctrlY - 9); rlVertex2f(ctrlCX - 3, ctrlY - 9);
        rlVertex2f(ctrlCX - 3, ctrlY + 9); rlVertex2f(ctrlCX - 8, ctrlY + 9);
        rlVertex2f(ctrlCX + 3, ctrlY - 9); rlVertex2f(ctrlCX + 8, ctrlY - 9);
        rlVertex2f(ctrlCX + 8, ctrlY + 9); rlVertex2f(ctrlCX + 3, ctrlY + 9);
        rlEnd();
    } else {
        rlBegin(RL_TRIANGLES);
        rlColor4f(cact.r, cact.g, cact.b, 1);
        rlVertex2f(ctrlCX - 6, ctrlY - 10); rlVertex2f(ctrlCX - 6, ctrlY + 10); rlVertex2f(ctrlCX + 10, ctrlY);
        rlEnd();
    }

    // Forward
    Drawing::botonControl(ctrlCX + sp, ctrlY, 16, false, 0.6f, 0.5f, 0.8f);
    rlBegin(RL_TRIANGLES);
    rlColor4f(0.7f, 0.6f, 0.9f, 1);
    rlVertex2f(ctrlCX + sp - 2, ctrlY); rlVertex2f(ctrlCX + sp + 6, ctrlY + 6); rlVertex2f(ctrlCX + sp + 6, ctrlY - 6);
    rlVertex2f(ctrlCX + sp - 8, ctrlY); rlVertex2f(ctrlCX + sp, ctrlY + 6); rlVertex2f(ctrlCX + sp, ctrlY - 6);
    rlEnd();

    // Next track
    Drawing::botonControl(ctrlCX + sp * 2, ctrlY, 16, false, 0.6f, 0.5f, 0.8f);
    rlBegin(RL_TRIANGLES);
    rlColor4f(0.7f, 0.6f, 0.9f, 1);
    rlVertex2f(ctrlCX + sp * 2 + 6, ctrlY);
    rlVertex2f(ctrlCX + sp * 2 - 5, ctrlY + 7);
    rlVertex2f(ctrlCX + sp * 2 - 5, ctrlY - 7);
    rlEnd();

    // Repeat button
    float repX = ctrlCX - sp * 3 + 8, repY = ctrlY;
    Drawing::botonControl(repX, repY, 12, gs.repetir, 0.5f, 0.5f, 0.9f);
    Drawing::circuloLinea(repX, repY, 7, 20, CFloat(gs.repetir ? 0.8f : 0.45f, gs.repetir ? 0.8f : 0.45f, 1.0f));
    Drawing::texto12(repX - 8, repY - 20, "REP", CFloat(0.5f, 0.45f, 0.65f));

    // Random button
    float rndX = ctrlCX + sp * 3 - 8, rndY = ctrlY;
    Drawing::botonControl(rndX, rndY, 12, gs.aleatorio, 0.5f, 0.9f, 0.5f);
    rlBegin(RL_LINES);
    rlColor4f(0.45f, gs.aleatorio ? 0.9f : 0.45f, 0.45f, 1);
    rlVertex2f(rndX - 6, rndY - 4); rlVertex2f(rndX + 6, rndY + 4);
    rlVertex2f(rndX - 6, rndY + 4); rlVertex2f(rndX + 6, rndY - 4);
    rlEnd();
    Drawing::texto12(rndX - 8, rndY - 20, "RND", CFloat(0.4f, 0.55f, 0.4f));

    // Volume
    Drawing::texto12(60 + ox, 142, "VOL", CFloat(0.45f, 0.35f, 0.6f));
    Drawing::barraSlider(88 + ox, 138, 180, gs.volumenMusica, 10, false);

    // Song list
    float listX = 370 + ox, listY = 475, listW = 365, listH = 30;
    rlBegin(RL_QUADS);
    rlColor4f(0.04f, 0.02f, 0.08f, 0.8f);
    rlVertex2f(listX, listY - (int)gs.canciones.size() * listH - 5);
    rlVertex2f(listX + listW, listY - (int)gs.canciones.size() * listH - 5);
    rlVertex2f(listX + listW, listY + 5);
    rlVertex2f(listX, listY + 5);
    rlEnd();
    Drawing::texto12(listX + 8, listY - 8, "#  TITULO                    DURACION", CFloat(0.5f, 0.3f, 0.75f));

    for (int i = 0; i < (int)gs.canciones.size(); i++) {
        float fy = listY - 30.0f - i * listH;
        bool esActual = (i == gs.cancionActual);
        bool esHover = (i == gs.cancionHover);
        if (esActual) {
            rlBegin(RL_QUADS);
            rlColor4f(gs.canciones[i].r * 0.3f, gs.canciones[i].g * 0.25f, gs.canciones[i].b * 0.4f, 0.7f);
            rlVertex2f(listX + 2, fy - 8); rlVertex2f(listX + listW - 2, fy - 8);
            rlVertex2f(listX + listW - 2, fy + 14); rlVertex2f(listX + 2, fy + 14);
            rlEnd();
        } else if (esHover) {
            rlBegin(RL_QUADS);
            rlColor4f(0.15f, 0.08f, 0.25f, 0.5f);
            rlVertex2f(listX + 2, fy - 8); rlVertex2f(listX + listW - 2, fy - 8);
            rlVertex2f(listX + listW - 2, fy + 14); rlVertex2f(listX + 2, fy + 14);
            rlEnd();
        }
        if (esActual && gs.reproduciendo) {
            // Playing bars animation
            for (int b = 0; b < 3; b++) {
                float barH = 4 + 3 * sinf(t * (0.03f + b * 0.005f) + (float)(i + b));
                rlBegin(RL_QUADS);
                rlColor4f(gs.canciones[i].r, gs.canciones[i].g, gs.canciones[i].b, 1);
                rlVertex2f(listX + 6 + b * 3, fy); rlVertex2f(listX + 8 + b * 3, fy);
                rlVertex2f(listX + 8 + b * 3, fy + barH); rlVertex2f(listX + 6 + b * 3, fy + barH);
                rlEnd();
            }
        } else {
            Color nc = esActual ? CFloat(gs.canciones[i].r, gs.canciones[i].g, gs.canciones[i].b) : CFloat(0.4f, 0.35f, 0.5f);
            Drawing::texto12(listX + 6, fy, std::to_string(i + 1), nc);
        }
        Color tc = esActual ? CFloat(gs.canciones[i].r, gs.canciones[i].g, gs.canciones[i].b)
                 : esHover  ? CFloat(0.75f, 0.65f, 0.9f) : CFloat(0.6f, 0.55f, 0.7f);
        Drawing::texto12(listX + 25, fy, gs.canciones[i].titulo, tc);
        Color dc = esActual ? CFloat(gs.canciones[i].r * 0.8f, gs.canciones[i].g * 0.7f, gs.canciones[i].b * 0.9f) : CFloat(0.4f, 0.35f, 0.5f);
        Drawing::texto12(listX + listW - 38, fy, gs.canciones[i].duracion, dc);
    }

    // Spectrum visualizer
    float vizX = 370 + ox, vizY = 128, vizW = 365, vizH = 60;
    rlBegin(RL_QUADS);
    rlColor4f(0.02f, 0.01f, 0.06f, 0.9f);
    rlVertex2f(vizX, vizY); rlVertex2f(vizX + vizW, vizY);
    rlVertex2f(vizX + vizW, vizY + vizH); rlVertex2f(vizX, vizY + vizH);
    rlEnd();
    float bw = (vizW - 4) / NUM_BARRAS;
    for (int b = 0; b < NUM_BARRAS; b++) {
        float bx2 = vizX + 2 + b * bw;
        float bh = gs.barrasViz[b];
        float t2 = (float)b / NUM_BARRAS;
        float br = cact.r * (0.5f + t2 * 0.5f);
        float bg = cact.g * (0.3f + t2 * 0.4f);
        float bb = cact.b * (0.6f + t2 * 0.4f);
        float alpha = gs.reproduciendo ? 0.85f : 0.35f;
        rlBegin(RL_QUADS);
        rlColor4f(br, bg, bb, alpha);
        rlVertex2f(bx2, vizY + vizH / 2 - bh);
        rlVertex2f(bx2 + bw * 0.8f, vizY + vizH / 2 - bh);
        rlVertex2f(bx2 + bw * 0.8f, vizY + vizH / 2 + bh);
        rlVertex2f(bx2, vizY + vizH / 2 + bh);
        rlEnd();
    }
    Drawing::texto12(vizX + 3, vizY + 2, "ESPECTRO", CFloat(0.3f, 0.2f, 0.5f));

    Drawing::instrucciones(200 + ox, 120, "W/S: Cancion    ESPACIO: Play/Pause    R: Repetir    X: Aleatorio    A/D: Vol    ESC: Volver", t);
}

void Screens::enciclopedia(GameState& gs) {
    float t = gs.tiempo;
    Background::library(gs);
    float ox = gs.encicloOffset;

    // Book shadow
    rlBegin(RL_QUADS);
    rlColor4f(0, 0, 0, 0.5f);
    rlVertex2f(78 + ox, 65); rlVertex2f(728 + ox, 65);
    rlVertex2f(728 + ox, 558); rlVertex2f(78 + ox, 558);
    // Book cover
    rlColor4f(0.22f, 0.13f, 0.04f, 1);
    rlVertex2f(75 + ox, 68); rlVertex2f(725 + ox, 68);
    rlVertex2f(725 + ox, 555); rlVertex2f(75 + ox, 555);
    rlEnd();
    // Gold border
    rlSetLineWidth(3);
    rlBegin(RL_LINES);
    rlColor4f(0.65f, 0.5f, 0.08f, 1);
    rlVertex2f(75 + ox, 68); rlVertex2f(725 + ox, 68);
    rlVertex2f(725 + ox, 68); rlVertex2f(725 + ox, 555);
    rlVertex2f(725 + ox, 555); rlVertex2f(75 + ox, 555);
    rlVertex2f(75 + ox, 555); rlVertex2f(75 + ox, 68);
    rlEnd();

    // Spine
    rlBegin(RL_QUADS);
    rlColor4f(0.15f, 0.08f, 0.02f, 1);
    rlVertex2f(390 + ox, 68); rlVertex2f(410 + ox, 68);
    rlVertex2f(410 + ox, 555); rlVertex2f(390 + ox, 555);
    rlEnd();

    // Left page
    rlBegin(RL_QUADS);
    rlColor4f(0.82f, 0.75f, 0.58f, 1);
    rlVertex2f(85 + ox, 78); rlVertex2f(388 + ox, 78);
    rlVertex2f(388 + ox, 545); rlVertex2f(85 + ox, 545);
    rlEnd();
    // Page lines
    rlBegin(RL_LINES);
    rlColor4f(0.65f, 0.58f, 0.42f, 1);
    for (int l = 0; l < 18; l++) {
        float ly2 = 100 + l * 24.0f;
        rlVertex2f(100 + ox, ly2); rlVertex2f(375 + ox, ly2);
    }
    rlEnd();

    Drawing::texto18(130 + ox, 510, "CODEX ARCHON", CFloat(0.2f, 0.1f, 0.02f));

    // Troop sprite — texture fitted into a fixed box with floating animation
    cargarEncicloSprites();
    float flotacion = sinf(t * 0.012f) * 4;
    drawEncicloSprite(gs.paginaLibro, 230 + ox, 350 + flotacion, 140, 160);

    DatosTropa& tropa = gs.tropas[gs.paginaLibro];
    bool esLuz = (tropa.bando == "Fuerzas de Luz");
    Drawing::texto12(115 + ox, 248, tropa.bando, CFloat(esLuz ? 0.7f : 0.5f, esLuz ? 0.55f : 0.1f, esLuz ? 0.1f : 0.4f));
    Drawing::texto12(115 + ox, 225, tropa.stat1, CFloat(0.25f, 0.15f, 0.03f));
    Drawing::texto12(115 + ox, 207, tropa.stat2, CFloat(0.25f, 0.15f, 0.03f));
    Drawing::texto12(115 + ox, 189, tropa.stat3, CFloat(0.25f, 0.15f, 0.03f));
    Drawing::texto12(220 + ox, 92, std::to_string(gs.paginaLibro * 2 + 1), CFloat(0.35f, 0.22f, 0.04f));

    // Right page
    rlBegin(RL_QUADS);
    rlColor4f(0.80f, 0.73f, 0.56f, 1);
    rlVertex2f(412 + ox, 78); rlVertex2f(715 + ox, 78);
    rlVertex2f(715 + ox, 545); rlVertex2f(412 + ox, 545);
    rlEnd();
    rlBegin(RL_LINES);
    rlColor4f(0.63f, 0.56f, 0.40f, 1);
    for (int l = 0; l < 18; l++) {
        float ly2 = 100 + l * 24.0f;
        rlVertex2f(425 + ox, ly2); rlVertex2f(702 + ox, ly2);
    }
    rlEnd();

    Drawing::texto24(430 + ox, 505, tropa.nombre, CFloat(0.15f, 0.15f, 0.55f));
    for (int i = 0; i < (int)tropa.descripcion.size(); i++)
        Drawing::texto12(430 + ox, 470 - i * 22.0f, tropa.descripcion[i], CFloat(0.12f, 0.12f, 0.45f));

    // Faction emblem
    float selX = 565 + ox, selY = 280;
    Drawing::circuloLinea(selX, selY, 45, 24, CFloat(0.5f, 0.35f, 0.07f));
    Drawing::circuloLinea(selX, selY, 38, 24, CFloat(0.5f, 0.35f, 0.07f));
    if (esLuz) {
        for (int k = 0; k < 2; k++) {
            rlBegin(RL_LINES);
            rlColor4f(0.7f, 0.6f, 0.15f, 1);
            for (int i = 0; i < 6; i++) {
                float a1 = i * 2.0f * M_PI / 6 + k * M_PI / 6 - M_PI / 2;
                float a2 = ((i + 1) % 6) * 2.0f * M_PI / 6 + k * M_PI / 6 - M_PI / 2;
                rlVertex2f(selX + cosf(a1) * 30, selY + sinf(a1) * 30);
                rlVertex2f(selX + cosf(a2) * 30, selY + sinf(a2) * 30);
            }
            rlEnd();
        }
    } else {
        rlBegin(RL_LINES);
        rlColor4f(0.5f, 0.05f, 0.05f, 1);
        for (int i = 0; i < 5; i++) {
            float a1 = i * 2.0f * M_PI / 5 + M_PI / 2;
            float a2 = ((i + 2) % 5) * 2.0f * M_PI / 5 + M_PI / 2;
            rlVertex2f(selX + cosf(a1) * 30, selY + sinf(a1) * 30);
            rlVertex2f(selX + cosf(a2) * 30, selY + sinf(a2) * 30);
        }
        rlEnd();
    }

    Drawing::texto12(555 + ox, 92, std::to_string(gs.paginaLibro * 2 + 2), CFloat(0.35f, 0.22f, 0.04f));

    // Page turn arrows
    float alf = 0.5f + 0.5f * sinf(t * 0.015f);
    if (gs.paginaLibro > 0) {
        rlBegin(RL_TRIANGLES);
        rlColor4f(alf * 0.7f, alf * 0.5f, alf * 0.2f, 1);
        rlVertex2f(95 + ox, 310); rlVertex2f(110 + ox, 320); rlVertex2f(110 + ox, 300);
        rlEnd();
    }
    if (gs.paginaLibro < gs.TOTAL_PAGINAS - 1) {
        rlBegin(RL_TRIANGLES);
        rlColor4f(alf * 0.7f, alf * 0.5f, alf * 0.2f, 1);
        rlVertex2f(705 + ox, 310); rlVertex2f(690 + ox, 320); rlVertex2f(690 + ox, 300);
        rlEnd();
    }

    // Title bar
    float tpuls = 1.0f + sinf(t * 0.008f) * 0.02f;
    rlPushMatrix();
    rlTranslatef(400 + ox, 578, 0); rlScalef(tpuls, tpuls, 1);
    Drawing::texto24(-75, -8, "ENCICLOPEDIA", CFloat(0.75f, 0.6f, 0.15f));
    rlPopMatrix();

    Drawing::instrucciones(245 + ox, 50, "A/D o clic: Cambiar pagina    ESC: Volver", t);
}

void Screens::espadaSlash(GameState& gs) {
    if (!gs.slashActivo) return;
    rlPushMatrix();
    rlTranslatef(gs.slashX, 300, 0);
    rlRotatef(45, 0, 0, 1);
    rlBegin(RL_QUADS);
    rlColor4f(0.9f, 0.9f, 0.9f, 1);
    rlVertex2f(-10, -200); rlVertex2f(10, -200); rlVertex2f(10, 200); rlVertex2f(-10, 200);
    rlColor4f(0.4f, 0.2f, 0.0f, 1);
    rlVertex2f(-20, -200); rlVertex2f(20, -200); rlVertex2f(20, -220); rlVertex2f(-20, -220);
    rlEnd();
    rlPopMatrix();

    float alpha = std::min((gs.slashX + 1000) / 900.0f, 1.0f);
    rlBegin(RL_QUADS);
    rlColor4f(0, 0, 0, alpha);
    rlVertex2f(0, 0); rlVertex2f(800, 0); rlVertex2f(800, 600); rlVertex2f(0, 600);
    rlEnd();
}
