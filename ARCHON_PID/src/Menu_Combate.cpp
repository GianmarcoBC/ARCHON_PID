#include "Menu_Combate.h"
#include <ctime>
#include <cstdlib>

// ════════════════════════════════════════════════════════════════════════════
//  PANTALLA DE ELECCIÓN DE MODO
// ════════════════════════════════════════════════════════════════════════════

void Menu_Combate::DrawElegirModo()
{
    int W = GetScreenWidth();
    int H = GetScreenHeight();

    // Título
    const char* titulo = "SELECCIONA MODO";
    DrawText(titulo, W / 2 - MeasureText(titulo, 40) / 2, H / 4, 40, WHITE);

    // Dos botones
    const char* opciones[2] = { "2 JUGADORES", "VS  IA" };
    for (int i = 0; i < 2; i++) {
        int bW = 300, bH = 70;
        int bX = W / 2 - bW / 2;
        int bY = H / 2 - 60 + i * (bH + 24);
        bool activo = (i == opcionModo);

        DrawRectangle(bX, bY, bW, bH, activo ? DARKGRAY : BLACK);
        DrawRectangleLines(bX, bY, bW, bH, activo ? WHITE : GRAY);
        DrawText(opciones[i],
            bX + bW / 2 - MeasureText(opciones[i], 28) / 2,
            bY + bH / 2 - 14, 28,
            activo ? YELLOW : LIGHTGRAY);
    }

    DrawText("W / S  o  FLECHAS  para navegar     ENTER para confirmar",
        W / 2 - MeasureText("W / S  o  FLECHAS  para navegar     ENTER para confirmar", 16) / 2,
        H - 60, 16, GRAY);
}

bool Menu_Combate::UpdateModo()
{
    if (IsKeyPressed(KEY_S) || IsKeyPressed(KEY_DOWN))
        opcionModo = (opcionModo + 1) % 2;
    if (IsKeyPressed(KEY_W) || IsKeyPressed(KEY_UP))
        opcionModo = (opcionModo - 1 + 2) % 2;

    if (IsKeyPressed(KEY_ENTER)) {
        modo = (opcionModo == 0) ? DOS_JUGADORES : VS_IA;
        return true;
    }
    return false;
}

// ════════════════════════════════════════════════════════════════════════════
//  PANEL DE PERSONAJE (compartido por ambos modos para el jugador)
// ════════════════════════════════════════════════════════════════════════════

void Menu_Combate::DrawSeleccion(int player, int cx, int cy, int sel, bool listo)
{
    Color col = colores[player - 1];
    int marcoW = 280, marcoH = 340;
    DrawRectangleLines(cx - marcoW / 2, cy - marcoH / 2, marcoW, marcoH,
        listo ? GREEN : col);

    Texture2D* tx = (player == 1) ? imagenes_claro[sel] : imagenes_oscuro[sel];
    float scale = 220.0f / tx->height;
    DrawTextureEx(*tx,
        { (float)(cx - (int)(tx->width * scale) / 2),
          (float)(cy - (int)(tx->height * scale) / 2 - 20) },
        0.0f, scale, WHITE);

    const char* nombre = (player == 1)
        ? personajes_claro[sel]->nombre
        : personajes_oscuro[sel]->nombre;
    DrawText(nombre, cx - MeasureText(nombre, 22) / 2, cy + marcoH / 2 - 60, 22, col);

    // Miniaturas
    int iconSize = 40;
    int totalW = NUM_PJS * iconSize + (NUM_PJS - 1) * 8;
    int startX = cx - totalW / 2;
    for (int i = 0; i < NUM_PJS; i++) {
        int ix = startX + i * (iconSize + 8);
        int iy = cy + marcoH / 2 + 10;
        bool es = (i == sel);
        DrawRectangleLines(ix, iy, iconSize, iconSize, es ? col : DARKGRAY);
        Texture2D* ic = (player == 1) ? imagenes_claro[i] : imagenes_oscuro[i];
        float s = (float)iconSize / ic->height;
        DrawTextureEx(*ic, { (float)ix,(float)iy }, 0.0f, s, es ? WHITE : DARKGRAY);
    }

    if (listo) {
        const char* r = "LISTO!";
        DrawText(r, cx - MeasureText(r, 24) / 2, cy - marcoH / 2 - 34, 24, GREEN);
    }

    const char* lbl = (player == 1) ? "JUGADOR 1" : "JUGADOR 2";
    DrawText(lbl, cx - MeasureText(lbl, 20) / 2, cy - marcoH / 2 - 60, 20, col);
}

// ════════════════════════════════════════════════════════════════════════════
//  PANEL DERECHO DE LA IA  (muestra "???" hasta que el jugador confirma)
// ════════════════════════════════════════════════════════════════════════════

void Menu_Combate::DrawPanelIA(int cx, int cy)
{
    int marcoW = 280, marcoH = 340;
    Color col = P1Listo ? RED : DARKGRAY;
    DrawRectangleLines(cx - marcoW / 2, cy - marcoH / 2, marcoW, marcoH, col);

    if (!P1Listo) {
        const char* sig = "???";
        DrawText(sig, cx - MeasureText(sig, 80) / 2, cy - 40, 80, DARKGRAY);
    }
    else {
        // Revelar personaje de la IA
        Texture2D* tx = imagenes_oscuro[selP2];
        float scale = 220.0f / tx->height;
        DrawTextureEx(*tx,
            { (float)(cx - (int)(tx->width * scale) / 2),
              (float)(cy - (int)(tx->height * scale) / 2 - 20) },
            0.0f, scale, WHITE);

        const char* nombre = personajes_oscuro[selP2]->nombre;
        DrawText(nombre, cx - MeasureText(nombre, 22) / 2, cy + marcoH / 2 - 60, 22, RED);

        // Miniaturas (solo decorativas, la IA ya eligió)
        int iconSize = 40;
        int totalW = NUM_PJS * iconSize + (NUM_PJS - 1) * 8;
        int startX = cx - totalW / 2;
        for (int i = 0; i < NUM_PJS; i++) {
            int ix = startX + i * (iconSize + 8);
            int iy = cy + marcoH / 2 + 10;
            bool es = (i == selP2);
            DrawRectangleLines(ix, iy, iconSize, iconSize, es ? RED : DARKGRAY);
            float s = (float)iconSize / imagenes_oscuro[i]->height;
            DrawTextureEx(*imagenes_oscuro[i], { (float)ix,(float)iy },
                0.0f, s, es ? WHITE : DARKGRAY);
        }
    }

    const char* lbl = "ENEMIGO IA";
    DrawText(lbl, cx - MeasureText(lbl, 20) / 2, cy - marcoH / 2 - 60, 20, RED);
}

// ════════════════════════════════════════════════════════════════════════════
//  SELECTOR DE DIFICULTAD  (solo VS_IA)
// ════════════════════════════════════════════════════════════════════════════

void Menu_Combate::DrawDificultad(int cx, int cy)
{
    const char* titulo = "DIFICULTAD";
    DrawText(titulo, cx - MeasureText(titulo, 20) / 2, cy - 40, 20, WHITE);

    for (int i = 0; i < 3; i++) {
        int bW = 110, bH = 34;
        int bX = cx - (3 * bW + 2 * 12) / 2 + i * (bW + 12);
        int bY = cy;
        bool activo = (i == dificultad);
        DrawRectangle(bX, bY, bW, bH, activo ? coloresDif[i] : DARKGRAY);
        DrawRectangleLines(bX, bY, bW, bH, activo ? WHITE : GRAY);
        DrawText(nombresDif[i],
            bX + bW / 2 - MeasureText(nombresDif[i], 16) / 2,
            bY + 9, 16, activo ? BLACK : LIGHTGRAY);
    }
}

// ════════════════════════════════════════════════════════════════════════════
//  UPDATE  2 JUGADORES
// ════════════════════════════════════════════════════════════════════════════

bool Menu_Combate::UpdateDosJugadores()
{
    // P1: A/D + ESPACIO
    if (!P1Listo) {
        if (IsKeyPressed(KEY_D)) selP1 = (selP1 + 1) % NUM_PJS;
        if (IsKeyPressed(KEY_A)) selP1 = (selP1 - 1 + NUM_PJS) % NUM_PJS;
        if (IsKeyPressed(KEY_SPACE)) P1Listo = true;
    }

    // P2: flechas + CTRL
    if (!P2Listo) {
        if (IsKeyPressed(KEY_RIGHT)) selP2 = (selP2 + 1) % NUM_PJS;
        if (IsKeyPressed(KEY_LEFT))  selP2 = (selP2 - 1 + NUM_PJS) % NUM_PJS;
        if (IsKeyPressed(KEY_RIGHT_CONTROL)) P2Listo = true;
    }

    // ESC vuelve a elegir modo
    if (IsKeyPressed(KEY_ESCAPE)) {
        modo = ELIGIENDO_MODO; P1Listo = P2Listo = false;
        selP1 = selP2 = 0;
    }

    return P1Listo && P2Listo && IsKeyPressed(KEY_ENTER);
}

// ════════════════════════════════════════════════════════════════════════════
//  UPDATE  VS IA
// ════════════════════════════════════════════════════════════════════════════

bool Menu_Combate::UpdateVsIA()
{
    srand(time(NULL));

    if (!P1Listo) {
        // Navegar personaje con A/D
        if (IsKeyPressed(KEY_A)) selP1 = (selP1 - 1 + NUM_PJS) % NUM_PJS;
        if (IsKeyPressed(KEY_D)) selP1 = (selP1 + 1) % NUM_PJS;

        // Navegar dificultad con flechas izq/der
        if (IsKeyPressed(KEY_LEFT))  dificultad = (dificultad - 1 + 3) % 3;
        if (IsKeyPressed(KEY_RIGHT)) dificultad = (dificultad + 1) % 3;

        // Confirmar con ESPACIO → IA elige al azar
        if (IsKeyPressed(KEY_SPACE)) {
            P1Listo = true;
            P2Listo = true;
            selP2 = rand() % NUM_PJS;
        }
    }

    // ESC vuelve a elegir modo
    if (IsKeyPressed(KEY_ESCAPE)) {
        modo = ELIGIENDO_MODO; P1Listo = P2Listo = false;
        selP1 = selP2 = 0; dificultad = 1;
    }

    return P1Listo && IsKeyPressed(KEY_ENTER);
}

// ════════════════════════════════════════════════════════════════════════════
//  UPDATE  público
// ════════════════════════════════════════════════════════════════════════════

bool Menu_Combate::Update()
{
    if (modo == ELIGIENDO_MODO) { UpdateModo(); return false; }
    if (modo == DOS_JUGADORES)  return UpdateDosJugadores();
    return UpdateVsIA();
}

// ════════════════════════════════════════════════════════════════════════════
//  DRAW
// ════════════════════════════════════════════════════════════════════════════

void Menu_Combate::Draw()
{
    int W = GetScreenWidth();
    int H = GetScreenHeight();
    ClearBackground(BLACK);

    // Fondo común
    DrawTexturePro(Fondo_Menu,
        { 0,0,(float)Fondo_Menu.width,(float)Fondo_Menu.height },
        { 0,0,(float)W,(float)H }, { 0,0 }, 0.0f, WHITE);

    // ── Pantalla de elección de modo ─────────────────────────────────────────
    if (modo == ELIGIENDO_MODO) {
        DrawElegirModo();
        return;
    }

    // ── Paneles de selección ─────────────────────────────────────────────────
    DrawSeleccion(1, W / 4, H / 2, selP1, P1Listo);

    if (modo == DOS_JUGADORES)
        DrawSeleccion(2, W * 3 / 4, H / 2, selP2, P2Listo);
    else
        DrawPanelIA(W * 3 / 4, H / 2);

    // VS central
    const char* vs = "VS";
    DrawText(vs, W / 2 - MeasureText(vs, 80) / 2 + 3, H / 2 - 33, 80, BLACK);
    DrawText(vs, W / 2 - MeasureText(vs, 80) / 2, H / 2 - 36, 80, GRAY);

    // ── Dificultad (solo IA) ─────────────────────────────────────────────────
    if (modo == VS_IA)
        DrawDificultad(W / 2, H * 3 / 4 - 20);

    // ── Controles ────────────────────────────────────────────────────────────
    if (modo == DOS_JUGADORES) {
        DrawText("A/D + ESPACIO",
            W / 4 - MeasureText("A/D + ESPACIO", 16) / 2, H - 50, 16, GRAY);
        DrawText("FLECHAS + CTRL",
            W * 3 / 4 - MeasureText("FLECHAS + CTRL", 16) / 2, H - 50, 16, GRAY);
    }
    else {
        DrawText("A/D = personaje     FLECHAS = dificultad     ESPACIO = confirmar",
            W / 2 - MeasureText("A/D = personaje     FLECHAS = dificultad     ESPACIO = confirmar", 15) / 2,
            H - 50, 15, GRAY);
    }
    DrawText("ESC = volver", W / 2 - MeasureText("ESC = volver", 14) / 2, H - 28, 14, DARKGRAY);

    // ── ENTER para empezar ────────────────────────────────────────────────────
    bool ambosListos = (modo == DOS_JUGADORES) ? (P1Listo && P2Listo) : P1Listo;
    if (ambosListos && (int)(GetTime() * 2) % 2 == 0) {
        const char* start = "PULSA ENTER PARA COMBATIR";
        DrawText(start, W / 2 - MeasureText(start, 28) / 2, H - 90, 28, YELLOW);
    }
}