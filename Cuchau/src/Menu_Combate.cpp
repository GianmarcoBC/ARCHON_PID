#include "Menu_Combate.h"
#include <cstdlib>

// ====================================================================
//  PANTALLA DE ELECCION DE MODO
//
//  Muestra dos opciones: "2 JUGADORES" y "VS IA".
//  El jugador navega con W/S o flechas y confirma con ENTER.
// ====================================================================

void Menu_Combate::DrawElegirModo()
{
    int W = GetScreenWidth();
    int H = GetScreenHeight();

    // Titulo centrado
    const char* titulo = "SELECCIONA MODO";
    DrawText(titulo, W / 2 - MeasureText(titulo, 40) / 2, H / 4, 40, WHITE);

    // Dibujar los dos botones de opcion
    const char* opciones[2] = { "2 JUGADORES", "VS  IA" };
    for (int i = 0; i < 2; i++) {
        int bW = 300, bH = 70;
        int bX = W / 2 - bW / 2;
        int bY = H / 2 - 60 + i * (bH + 24);
        bool activo = (i == opcionModo);  // Resaltar la opcion seleccionada

        DrawRectangle(bX, bY, bW, bH, activo ? DARKGRAY : BLACK);
        DrawRectangleLines(bX, bY, bW, bH, activo ? WHITE : GRAY);
        DrawText(opciones[i],
            bX + bW / 2 - MeasureText(opciones[i], 28) / 2,
            bY + bH / 2 - 14, 28,
            activo ? YELLOW : LIGHTGRAY);
    }

    // Instrucciones en la parte inferior
    DrawText("W / S  o  FLECHAS  para navegar     ENTER para confirmar",
        W / 2 - MeasureText("W / S  o  FLECHAS  para navegar     ENTER para confirmar", 16) / 2,
        H - 60, 16, GRAY);
}

// ====================================================================
//  UpdateModo — Logica de la pantalla de seleccion de modo
//
//  Cambia entre las opciones con W/S o flechas.
//  ENTER confirma y pasa a la pantalla de seleccion de personajes.
// ====================================================================

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
    Color col = colores[player - 1];
    int marcoW = 280, marcoH = 340;

    // Marco exterior (verde si esta listo, color del jugador si no)
    DrawRectangleLines(cx - marcoW / 2, cy - marcoH / 2, marcoW, marcoH,
        listo ? GREEN : col);

    // Sprite grande del personaje seleccionado, escalado para caber en el marco
    Texture2D* tx = (player == 1) ? imagenes_claro[sel] : imagenes_oscuro[sel];
    float scale = 220.0f / tx->height;
    DrawTextureEx(*tx,
        { (float)(cx - (int)(tx->width * scale) / 2),
          (float)(cy - (int)(tx->height * scale) / 2 - 20) },
        0.0f, scale, WHITE);

    // Nombre del personaje seleccionado
    std::string_view nombre = (player == 1)
        ? personajes_claro[sel]->nombre
        : personajes_oscuro[sel]->nombre;   
    DrawText(nombre.data(), cx - MeasureText(nombre.data(), 22) / 2, cy + marcoH / 2 - 60, 22, col);

    // Fila de miniaturas (iconos pequenos de todos los personajes del equipo)
    int iconSize = 40;
    int totalW = NUM_PJS * iconSize + (NUM_PJS - 1) * 8;
    int startX = cx - totalW / 2;
    for (int i = 0; i < NUM_PJS; i++) {
        int ix = startX + i * (iconSize + 8);
        int iy = cy + marcoH / 2 + 10;
        bool es = (i == sel);  // Resaltar el seleccionado
        DrawRectangleLines(ix, iy, iconSize, iconSize, es ? col : DARKGRAY);
        Texture2D* ic = (player == 1) ? imagenes_claro[i] : imagenes_oscuro[i];
        float s = (float)iconSize / ic->height;
        DrawTextureEx(*ic, { (float)ix,(float)iy }, 0.0f, s, es ? WHITE : DARKGRAY);
    }

    // Texto "LISTO!" si el jugador confirmo su seleccion
    if (listo) {
        const char* r = "LISTO!";
        DrawText(r, cx - MeasureText(r, 24) / 2, cy - marcoH / 2 - 34, 24, GREEN);
    }

    // Etiqueta del jugador
    const char* lbl = (player == 1) ? "JUGADOR 1" : "JUGADOR 2";
    DrawText(lbl, cx - MeasureText(lbl, 20) / 2, cy - marcoH / 2 - 60, 20, col);
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
        Texture2D* tx = imagenes_oscuro[selP2];
        float scale = 220.0f / tx->height;
        DrawTextureEx(*tx,
            { (float)(cx - (int)(tx->width * scale) / 2),
              (float)(cy - (int)(tx->height * scale) / 2 - 20) },
            0.0f, scale, WHITE);

        std::string_view nombre = personajes_oscuro[selP2]->nombre;
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
            float s = (float)iconSize / imagenes_oscuro[i]->height;
            DrawTextureEx(*imagenes_oscuro[i], { (float)ix,(float)iy },
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

    // ESC reinicia y vuelve a la pantalla de seleccion de modo
    if (IsKeyPressed(KEY_ESCAPE)) {
        modo = ELIGIENDO_MODO; P1Listo = P2Listo = false;
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
            selP2   = rand() % NUM_PJS;  // Personaje aleatorio del equipo oscuro
        }
    }

    // ESC reinicia y vuelve a la pantalla de seleccion de modo
    if (IsKeyPressed(KEY_ESCAPE)) {
        modo = ELIGIENDO_MODO; P1Listo = P2Listo = false;
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
    if (modo == ELIGIENDO_MODO) { UpdateModo(); return false; }
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

    // Si estamos eligiendo modo, solo dibujar esa pantalla
    if (modo == ELIGIENDO_MODO) { DrawElegirModo(); return; }

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
