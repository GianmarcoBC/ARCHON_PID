#include "Menu_Combate.h"
#include <cstdlib>
#include "rlgl.h"

// ====================================================================
//  DrawSeleccion — Dibuja el panel de seleccion de un jugador
//
//  Muestra:
//    - Nombre del jugador ("JUGADOR 1" / "JUGADOR 2")
//    - Marco con el sprite grande del personaje seleccionado
//    - Nombre del personaje
//    - Fila de miniaturas de todos los personajes del equipo
//    - "LISTO!" si el jugador ya confirmo
//
//  player: 1 o 2 (determina equipo claro/oscuro y color)
//  cx, cy: centro del panel en pantalla
//  sel: indice del personaje seleccionado
//  listo: si el jugador ya confirmo
// ====================================================================

void Menu_Combate::DrawSeleccion(int player, int cx, int cy, int sel, bool listo)
{
    // Determinar bando: en PvP P1=claro, P2=oscuro; en VS_IA P1 usa bandoP1
    int bando = (modo == DOS_JUGADORES) ? (player - 1) : bandoP1;

    // Arrays del bando correspondiente
    Texture2D** imgs = (bando == 0) ? imagenes_claro : imagenes_oscuro;
    const Pj_info** pjs = (bando == 0) ? personajes_claro : personajes_oscuro;

    // Colores del bando
    float br = (bando == 0) ? 0.2f : 0.7f;
    float bg = (bando == 0) ? 0.5f : 0.1f;
    float bb = (bando == 0) ? 0.9f : 0.1f;

    float x = cx - 150.f, y = cy - 200.f;
    float w = 300.f, h = 380.f;

    // Fondo del panel
    rlBegin(RL_QUADS);
    rlColor4f(0.02f, 0.03f, 0.06f, 0.92f);
    rlVertex2f(x, y);   rlVertex2f(x + w, y);
    rlVertex2f(x + w, y + h); rlVertex2f(x, y + h);
    rlEnd();

    // Borde pulsante (fijo si listo)
    float al = listo ? 1.f : 0.6f + 0.4f * sinf((float)GetTime() * 2.f);
    rlSetLineWidth(listo ? 2.5f : 1.5f);
    rlBegin(RL_LINES);
    rlColor4f(listo ? 0.1f : br * al,
        listo ? 0.8f : bg * al,
        listo ? 0.1f : bb * al, 1.f);
    rlVertex2f(x, y);   rlVertex2f(x + w, y);
    rlVertex2f(x + w, y);   rlVertex2f(x + w, y + h);
    rlVertex2f(x + w, y + h); rlVertex2f(x, y + h);
    rlVertex2f(x, y + h); rlVertex2f(x, y);
    rlEnd();

    // Etiqueta del jugador
    const char* lbl = (player == 1) ? "JUGADOR 1" : "JUGADOR 2";
    DrawText(lbl, cx - MeasureText(lbl, 18) / 2, (int)(y - 26), 18,
        colores[player - 1]);

    // Sprite grande
    Texture2D* tx = imgs[sel];
    float scale = 200.f / tx->height;
    DrawTextureEx(*tx,
        { cx - tx->width * scale / 2.f, y + 20.f },
        0.f, scale, listo ? WHITE : ColorAlpha(WHITE, 0.75f));

    // Nombre del personaje
    std::string_view nombre = pjs[sel]->nombre;
    DrawText(nombre.data(),
        cx - MeasureText(nombre.data(), 20) / 2,
        (int)(y + h - 90), 20, colores[player - 1]);

    // Fila de miniaturas
    int iconSize = 34, gap = 6;
    int totalW = NUM_PJS * (iconSize + gap) - gap;
    int startX = cx - totalW / 2;
    for (int i = 0; i < NUM_PJS; i++) {
        float ix = (float)(startX + i * (iconSize + gap));
        float iy = y + h - 58.f;
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
        rlColor4f(br * (es ? 1.f : 0.3f),
            bg * (es ? 1.f : 0.3f),
            bb * (es ? 1.f : 0.3f), 1.f);
        rlVertex2f(ix, iy);
        rlVertex2f(ix + iconSize, iy);
        rlVertex2f(ix + iconSize, iy);
        rlVertex2f(ix + iconSize, iy + iconSize);
        rlVertex2f(ix + iconSize, iy + iconSize);
        rlVertex2f(ix, iy + iconSize);
        rlVertex2f(ix, iy + iconSize);
        rlVertex2f(ix, iy);
        rlEnd();

        // Sprite miniatura
        Texture2D* ic = imgs[i];
        float s = (float)iconSize / ic->height;
        DrawTextureEx(*ic, { ix, iy }, 0.f, s, es ? WHITE : DARKGRAY);
    }

    // LISTO
    if (listo) {
        const char* r = "LISTO!";
        DrawText(r, cx - MeasureText(r, 22) / 2, (int)(y - 52), 22, GREEN);
    }
}

// ====================================================================
//  DrawPanelIA — Dibuja el panel del enemigo IA
//
//  Antes de que P1 confirme: muestra "???" (el enemigo aun no se revela).
//  Despues de confirmar: muestra el personaje aleatorio asignado a la IA
//  con su sprite, nombre y fila de miniaturas.
// ====================================================================

void Menu_Combate::DrawPanelIA(int cx, int cy)
{
    int bandoIA = 1 - bandoP1;  // contrario al de P1
    auto& imgs = (bandoIA == 0) ? imagenes_claro : imagenes_oscuro;
    auto& pjs = (bandoIA == 0) ? personajes_claro : personajes_oscuro;

    int marcoW = 280, marcoH = 340;
    Color col = P1Listo ? RED : DARKGRAY;
    DrawRectangleLines(cx - marcoW / 2, cy - marcoH / 2, marcoW, marcoH, col);

    if (!P1Listo) {
        // Personaje IA aun no revelado
        const char* sig = "???";
        DrawText(sig, cx - MeasureText(sig, 80) / 2, cy - 40, 80, DARKGRAY);
    }
    else {
        // Mostrar el personaje asignado aleatoriamente a la IA
        Texture2D* tx = imgs[selP2];
        float scale = 220.0f / tx->height;
        DrawTextureEx(*tx,
            { (float)(cx - (int)(tx->width * scale) / 2),
              (float)(cy - (int)(tx->height * scale) / 2 - 20) },
            0.0f, scale, WHITE);

        std::string_view nombre = pjs[selP2]->nombre;
        DrawText(nombre.data(), cx - MeasureText(nombre.data(), 22) / 2, cy + marcoH / 2 - 60, 22, RED);

        // Miniaturas del equipo oscuro
        int iconSize = 40;
        int totalW = NUM_PJS * iconSize + (NUM_PJS - 1) * 8;
        int startX = cx - totalW / 2;
        for (int i = 0; i < NUM_PJS; i++) {
            int ix = startX + i * (iconSize + 8);
            int iy = cy + marcoH / 2 + 10;
            bool es = (i == selP2);
            DrawRectangleLines(ix, iy, iconSize, iconSize, es ? RED : DARKGRAY);
            float s = (float)iconSize / imgs[i]->height;
            DrawTextureEx(*imgs[i], { (float)ix,(float)iy },
                0.0f, s, es ? WHITE : DARKGRAY);
        }
    }

    // Etiqueta "ENEMIGO IA"
    const char* lbl = "ENEMIGO IA";
    DrawText(lbl, cx - MeasureText(lbl, 20) / 2, cy - marcoH / 2 - 60, 20, RED);
}

// ====================================================================
//  DrawDificultad — Dibuja el selector de dificultad
//
//  Tres botones horizontales: FACIL (verde), NORMAL (amarillo), DIFICIL (rojo).
//  El boton activo se resalta con su color; los inactivos son gris oscuro.
// ====================================================================

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

// ====================================================================
//  UpdateDosJugadores — Logica del modo 2 jugadores
//
//  P1: A/D para navegar, Espacio para confirmar
//  P2: Flechas para navegar, Ctrl derecho para confirmar
//  ESC: Volver a seleccion de modo
//  Devuelve true cuando ambos estan listos y se pulsa ENTER
// ====================================================================

bool Menu_Combate::UpdateDosJugadores()
{
    // Navegacion y confirmacion de P1
    if (!P1Listo) {
        if (IsKeyPressed(KEY_D)) selP1 = (selP1 + 1) % NUM_PJS;
        if (IsKeyPressed(KEY_A)) selP1 = (selP1 - 1 + NUM_PJS) % NUM_PJS;
        if (IsKeyPressed(KEY_SPACE)) P1Listo = true;
    }

    // Navegacion y confirmacion de P2
    if (!P2Listo) {
        if (IsKeyPressed(KEY_RIGHT)) selP2 = (selP2 + 1) % NUM_PJS;
        if (IsKeyPressed(KEY_LEFT))  selP2 = (selP2 - 1 + NUM_PJS) % NUM_PJS;
        if (IsKeyPressed(KEY_RIGHT_CONTROL)) P2Listo = true;
    }

    // ESC: signal caller to go back to ARCHON menu
    if (IsKeyPressed(KEY_ESCAPE)) {
        quiereVolver = true;
        P1Listo = P2Listo = false;
        selP1 = selP2 = 0;
    }

    // Iniciar combate cuando ambos estan listos y se pulsa ENTER
    return P1Listo && P2Listo && IsKeyPressed(KEY_ENTER);
}

// ====================================================================
//  UpdateVsIA — Logica del modo VS IA
//
//  P1: A/D para personaje, Flechas para dificultad, Espacio para confirmar
//  Al confirmar, se asigna un personaje aleatorio a la IA
//  ESC: Volver a seleccion de modo
//  Devuelve true cuando P1 esta listo y se pulsa ENTER
// ====================================================================

bool Menu_Combate::UpdateVsIA()
{
    if (!P1Listo) {
        // Navegar personajes de P1
        if (IsKeyPressed(KEY_A)) selP1 = (selP1 - 1 + NUM_PJS) % NUM_PJS;
        if (IsKeyPressed(KEY_D)) selP1 = (selP1 + 1) % NUM_PJS;

        // Navegar dificultad con flechas
        if (IsKeyPressed(KEY_LEFT))  dificultad = (dificultad - 1 + 3) % 3;
        if (IsKeyPressed(KEY_RIGHT)) dificultad = (dificultad + 1) % 3;

        // Confirmar: seleccionar personaje aleatorio para la IA
        if (IsKeyPressed(KEY_SPACE)) {
            P1Listo = true;
            P2Listo = true;
            selP2 = rand() % NUM_PJS;  // índice aleatorio del equipo contrario
        }
    }

    // ESC: signal caller to go back to ARCHON menu
    if (IsKeyPressed(KEY_ESCAPE)) {
        quiereVolver = true;
        P1Listo = P2Listo = false;
        selP1 = selP2 = 0; dificultad = 1;
    }

    return P1Listo && IsKeyPressed(KEY_ENTER);
}

// ====================================================================
//  UPDATE / DRAW publicos
//
//  Update() delega al metodo correspondiente segun el estado del menu.
//  Draw() dibuja el fondo, los paneles de seleccion, el "VS" central,
//  el selector de dificultad (solo en modo IA), instrucciones y el
//  mensaje parpadeante "PULSA ENTER PARA COMBATIR".
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

    // Fondo del menu (imagen TMNT escalada a pantalla completa)
    DrawTexturePro(Fondo_Menu,
        { 0, 0, (float)Fondo_Menu.width, (float)Fondo_Menu.height },
        { 0, 0, (float)W, (float)H }, { 0, 0 }, 0.0f, WHITE);

    // Panel izquierdo: seleccion de P1 (equipo claro)
    DrawSeleccion(1, W / 4, H / 2, selP1, P1Listo);

    // Panel derecho: P2 (equipo oscuro) o panel de IA
    if (modo == DOS_JUGADORES)
        DrawSeleccion(2, W * 3 / 4, H / 2, selP2, P2Listo);
    else
        DrawPanelIA(W * 3 / 4, H / 2);

    // Texto "VS" grande en el centro con sombra
    const char* vs = "VS";
    DrawText(vs, W / 2 - MeasureText(vs, 80) / 2 + 3, H / 2 - 33, 80, BLACK);   // Sombra
    DrawText(vs, W / 2 - MeasureText(vs, 80) / 2, H / 2 - 36, 80, GRAY);         // Texto

    // Selector de dificultad (solo en modo VS IA)
    if (modo == VS_IA)
        DrawDificultad(W / 2, H * 3 / 4 - 20);

    // Instrucciones de controles en la parte inferior
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

    // Mensaje parpadeante "PULSA ENTER PARA COMBATIR" cuando ambos estan listos
    bool ambosListos = (modo == DOS_JUGADORES) ? (P1Listo && P2Listo) : P1Listo;
    if (ambosListos && (int)(GetTime() * 2) % 2 == 0) {
        const char* start = "PULSA ENTER PARA COMBATIR";
        DrawText(start, W / 2 - MeasureText(start, 28) / 2, H - 90, 28, YELLOW);
    }
}

void Menu_Combate::Reset()
{
    modo = DOS_JUGADORES;
    selP1 = selP2 = 0;
    P1Listo = P2Listo = false;
    dificultad = 1;
    quiereVolver = false;
}

void Menu_Combate::SetModo(bool ia, int bando)
{
    modo = ia ? VS_IA : DOS_JUGADORES;
    bandoP1 = bando;
    selP1 = selP2 = 0;
    P1Listo = P2Listo = false;
    dificultad = 1;
    quiereVolver = false;
}

void Menu_Combate::HandleMouse()
{
    if (!IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) return;
    int W = GetScreenWidth(), H = GetScreenHeight();
    float mx = (float)GetMouseX(), my = (float)GetMouseY();

    if (modo == DOS_JUGADORES) {
        // Panel P1 (izquierda): navegar miniaturas con clic
        float cx1 = W / 4.f;
        float panelY = H / 2.f - 200.f;
        float iconSize = 34, gap = 6;
        int totalW = NUM_PJS * (iconSize + gap) - gap;
        float startX = cx1 - totalW / 2.f;
        float iconRow = panelY + 380.f - 60.f;

        for (int i = 0; i < NUM_PJS; i++) {
            float ix = startX + i * (iconSize + gap);
            if (mx > ix && mx<ix + iconSize && my>iconRow && my < iconRow + iconSize) {
                if (!P1Listo) selP1 = i;
            }
        }
        // Panel P2 (derecha)
        float cx2 = W * 3 / 4.f;
        startX = cx2 - totalW / 2.f;
        for (int i = 0; i < NUM_PJS; i++) {
            float ix = startX + i * (iconSize + gap);
            if (mx > ix && mx<ix + iconSize && my>iconRow && my < iconRow + iconSize) {
                if (!P2Listo) selP2 = i;
            }
        }
        // Confirmar P1 con clic en sprite grande
        float sprW = 100, sprH = 200;
        if (!P1Listo && mx > cx1 - sprW && mx<cx1 + sprW && my>H / 2.f - sprH && my < H / 2.f + sprH)
            P1Listo = true;
        if (!P2Listo && mx > W * 3 / 4.f - sprW && mx<W * 3 / 4.f + sprW && my>H / 2.f - sprH && my < H / 2.f + sprH)
            P2Listo = true;
    }
    else { // VS_IA
        float cx1 = W / 4.f;
        float panelY = H / 2.f - 200.f;
        float iconSize = 34, gap = 6;
        int totalW = NUM_PJS * (iconSize + gap) - gap;
        float startX = cx1 - totalW / 2.f;
        float iconRow = panelY + 380.f - 60.f;

        for (int i = 0; i < NUM_PJS; i++) {
            float ix = startX + i * (iconSize + gap);
            if (mx > ix && mx<ix + iconSize && my>iconRow && my < iconRow + iconSize) {
                if (!P1Listo) selP1 = i;
            }
        }
        // Botones de dificultad
        float bW = 100, bH = 28, gapD = 10;
        float dstartX = W / 2.f - (3 * bW + 2 * gapD) / 2.f;
        float dY = H * 3 / 4.f - 14.f;
        for (int i = 0; i < 3; i++) {
            float bx = dstartX + i * (bW + gapD);
            if (mx > bx && mx<bx + bW && my>dY && my < dY + bH)
                dificultad = i;
        }
        // Confirmar con clic en sprite grande
        float sprW = 100, sprH = 200;
        if (!P1Listo && mx > cx1 - sprW && mx<cx1 + sprW && my>H / 2.f - sprH && my < H / 2.f + sprH) {
            P1Listo = true;
            P2Listo = true;
            selP2 = rand() % NUM_PJS;
        }
    }

    // Volver con clic fuera de los paneles no es intuitivo — ESC sigue siendo la salida
}