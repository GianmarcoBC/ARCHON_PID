#include "Menu_Combate.h"

void Menu_Combate::DrawSeleccion(int player, int cx, int cy, int sel, bool listo)
{
    Pj p = player == 1 ? *personajes_claro[sel] : *personajes_oscuro[sel];
    Color col = colores[player-1];

    // Marco exterior
    int marcoW = 280, marcoH = 340;
    DrawRectangleLines(cx - marcoW / 2, cy - marcoH / 2, marcoW, marcoH,
        listo ? GREEN : col);

    // Sprite del personaje seleccionado
    Texture2D* tx = (player == 1) ? imagenes_claro[sel] : imagenes_oscuro[sel];
    float scale = 220.0f / tx->height; // Ajusta altura a 220px
    int spriteW = (int)(tx->width * scale);
    int spriteH = (int)(tx->height * scale);
    DrawTextureEx(*tx, { (float)(cx - spriteW / 2), (float)(cy - spriteH / 2 - 20) },
        0.0f, scale, WHITE);

    // Nombre del personaje
    int nWidth = MeasureText(p.nombre, 22);
    DrawText(p.nombre, cx - nWidth / 2, cy + marcoH / 2 - 60, 22, col);

    // Miniaturas de los 4 personajes abajo
    int iconSize = 40;
    int totalW = NUM_PJS * iconSize + (NUM_PJS - 1) * 8;
    int startX = cx - totalW / 2;
    for (int i = 0; i < NUM_PJS; i++) {
        int ix = startX + i * (iconSize + 8);
        int iy = cy + marcoH / 2 + 10;
        bool esSeleccionado = (i == sel);
        // Borde del icono
        DrawRectangleLines(ix, iy, iconSize, iconSize,
            esSeleccionado ? colores[player-1] : DARKGRAY);
        // Miniatura
        if(player == 1){
            float s = (float)iconSize / imagenes_claro[i]->height;
            DrawTextureEx(*imagenes_claro[i], { (float)ix, (float)iy }, 0.0f, s,
                esSeleccionado ? WHITE : DARKGRAY);
        }
        else {
            float s = (float)iconSize / imagenes_oscuro[i]->height;
            DrawTextureEx(*imagenes_oscuro[i], { (float)ix, (float)iy }, 0.0f, s,
                esSeleccionado ? WHITE : DARKGRAY);
        }
        
    }

    // Estado listo
    if (listo) {
        const char* ready = "LISTO!";
        int rWidth = MeasureText(ready, 24);
        DrawText(ready, cx - rWidth / 2, cy - marcoH / 2 - 34, 24, GREEN);
    }

    // Número de jugador
    const char* pLabel = (player == 1) ? "JUGADOR 1" : "JUGADOR 2";
    Color pColor = (player == 1) ? SKYBLUE : RED;
    int pWidth = MeasureText(pLabel, 20);
    DrawText(pLabel, cx - pWidth / 2, cy - marcoH / 2 - 60, 20, pColor);
}

bool Menu_Combate::Update() {
    // P1 navega con A/D, confirma con ESPACIO
    if (IsKeyPressed(KEY_D)) {
        selP1 = (selP1 + 1) % 8;
        P1Listo = false;
    }
    if (IsKeyPressed(KEY_A)) {
        selP1 = (selP1 - 1 + 8) % 8;
        P1Listo = false;
    }
    if (IsKeyPressed(KEY_SPACE)) P1Listo = true;

    // P2 navega con izquierda/derecha, confirma con CTRL
    if (IsKeyPressed(KEY_RIGHT)) {
        selP2 = (selP2 + 1) % 8;
        P2Listo = false;
    }
    if (IsKeyPressed(KEY_LEFT)) {
        selP2 = (selP2 - 1 + 8) % 8;
        P2Listo = false;
    }
    if (IsKeyPressed(KEY_RIGHT_CONTROL)) P2Listo = true;

    return P1Listo && P2Listo && IsKeyPressed(KEY_ENTER); // true = ambos listos, empieza el combate
}

void Menu_Combate::Draw() {

    int W = GetScreenWidth();
    int H = GetScreenHeight();

    ClearBackground(BLACK);

    // --- Fondo ---
    DrawTexturePro(
        Fondo_Menu,
        { 0, 0, (float)Fondo_Menu.width, (float)Fondo_Menu.height },
        { 0, 0, (float)W, (float)H},
        { 0, 0 }, 0.0f, WHITE
    );

    /*
    
    // --- Título ---
    const char* titulo = "SKILL ISSUE BROOO";
    int tSize = 60;
    int tWidth = MeasureText(titulo, tSize);
    // Sombra
    DrawText(titulo, W / 2 - tWidth / 2 + 4, 44, tSize, BLACK);
    DrawText(titulo, W / 2 - tWidth / 2, 40, tSize, GREEN);

     // --- VS central ---
    const char* vs = "VS";
    int vsSize = 80;
    int vsWidth = MeasureText(vs, vsSize);
    DrawText(vs, W / 2 - vsWidth / 2 + 3, H / 2 - 33, vsSize, BLACK);
    DrawText(vs, W / 2 - vsWidth / 2, H / 2 - 36, vsSize, GREEN);

    */


    // --- Paneles de selección ---
    DrawSeleccion(1, W / 4, H / 2, selP1, P1Listo);
    DrawSeleccion(2, W * 3 / 4, H / 2, selP2, P2Listo);



    // --- Controles ---
    DrawText("A / D  +  ESPACIO", W / 4 - MeasureText("A / D  +  ESPACIO", 18) / 2, H - 60, 18, GRAY);
    DrawText("FLECHAS  +  CTRL", W * 3 / 4 - MeasureText("FLECHAS  +  CTRL", 18) / 2, H - 60, 18, GRAY);

    // --- Botón START si los dos están listos ---
    if (P1Listo && P2Listo) {
        const char* start = "PULSA ENTER PARA COMBATIR";
        int sWidth = MeasureText(start, 28);
        // Parpadeo
        if ((int)(GetTime() * 2) % 2 == 0)
            DrawText(start, W / 2 - sWidth / 2, H - 100, 28, YELLOW);
    }
}