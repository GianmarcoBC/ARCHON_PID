#include "Screens.h"
#include "Drawing.h"
#include "Background.h"
#include "Particles.h"
#include "SaveSystem.h"
#include "cuchau/Controlador_Combate.h"
#include "Controlador_Tablero.h"
#include "rlgl.h"

namespace Archon_PID {

// ============================================================
// HELPERS COMPARTIDOS
// Estos helpers son estáticos al fichero (no son métodos de clase)
// porque los usan varias pantallas distintas.
// ============================================================

// Panel con borde doble, sombra y esquinas recortadas.
// Lo usan casi todas las pantallas para dar coherencia visual.
static void drawPanel(float x, float y, float w, float h,
    float br, float bg, float bb,   // color del borde
    float fr, float fg, float fb,   // color del fondo
    float alpha = 0.85f)
{
    // Sombra desplazada
    rlBegin(RL_QUADS);
    rlColor4f(0,0,0,0.5f);
    rlVertex2f(x+4,y-4); rlVertex2f(x+w+4,y-4);
    rlVertex2f(x+w+4,y+h+4); rlVertex2f(x+4,y+h+4);
    rlEnd();
    // Fondo
    rlBegin(RL_QUADS);
    rlColor4f(fr,fg,fb,alpha);
    rlVertex2f(x,y); rlVertex2f(x+w,y);
    rlVertex2f(x+w,y+h); rlVertex2f(x,y+h);
    rlEnd();
    // Borde exterior
    rlSetLineWidth(2.5f);
    rlBegin(RL_LINES);
    rlColor4f(br,bg,bb,1);
    rlVertex2f(x,y);     rlVertex2f(x+w,y);
    rlVertex2f(x+w,y);   rlVertex2f(x+w,y+h);
    rlVertex2f(x+w,y+h); rlVertex2f(x,y+h);
    rlVertex2f(x,y+h);   rlVertex2f(x,y);
    rlEnd();
    // Borde interior (más fino, medio transparente)
    rlSetLineWidth(1.f);
    rlBegin(RL_LINES);
    rlColor4f(br*0.6f,bg*0.6f,bb*0.6f,0.5f);
    rlVertex2f(x+5,y+5);     rlVertex2f(x+w-5,y+5);
    rlVertex2f(x+w-5,y+5);   rlVertex2f(x+w-5,y+h-5);
    rlVertex2f(x+w-5,y+h-5); rlVertex2f(x+5,y+h-5);
    rlVertex2f(x+5,y+h-5);   rlVertex2f(x+5,y+5);
    rlEnd();
    // Triángulos en las esquinas para dar sensación de marco tallado
    float cs = 12.f;
    rlBegin(RL_TRIANGLES);
    rlColor4f(br,bg,bb,0.8f);
    rlVertex2f(x,y);       rlVertex2f(x+cs,y);   rlVertex2f(x,y+cs);
    rlVertex2f(x+w,y);     rlVertex2f(x+w-cs,y); rlVertex2f(x+w,y+cs);
    rlVertex2f(x,y+h);     rlVertex2f(x+cs,y+h); rlVertex2f(x,y+h-cs);
    rlVertex2f(x+w,y+h);   rlVertex2f(x+w-cs,y+h); rlVertex2f(x+w,y+h-cs);
    rlEnd();
}

// Título grande centrado con pulso de escala y sombra.
static void drawTitulo(const std::string& txt, float cx, float y,
    float t, float r, float g, float b)
{
    float esc = 1.f + sinf(t*0.006f)*0.025f;
    float tw  = (float)MeasureText(txt.c_str(), 20);
    rlPushMatrix();
    rlTranslatef(cx, y, 0); rlScalef(esc,esc,1);
    Drawing::texto24(-tw/2.f-2, -8, txt, CFloat(r*0.3f,g*0.3f,b*0.3f));
    Drawing::texto24(-tw/2.f,   -5, txt, CFloat(r,g,b));
    rlPopMatrix();
}

// Separador horizontal con un diamante en el centro.
static void drawSeparador(float x1, float y, float x2, float r, float g, float b) {
    rlSetLineWidth(1.5f);
    rlBegin(RL_LINES);
    rlColor4f(r,g,b,1);
    rlVertex2f(x1,y); rlVertex2f(x2,y);
    rlEnd();
    float cx = (x1+x2)/2;
    rlBegin(RL_LINES);
    rlColor4f(r,g,b,1);
    rlVertex2f(cx,y+6); rlVertex2f(cx+7,y);
    rlVertex2f(cx+7,y); rlVertex2f(cx,y-6);
    rlVertex2f(cx,y-6); rlVertex2f(cx-7,y);
    rlVertex2f(cx-7,y); rlVertex2f(cx,y+6);
    rlEnd();
}

// Barra de stat con relleno de color.
static void drawBarraStat(float x, float y, float w, int val, Color col) {
    rlBegin(RL_QUADS);
    rlColor4f(0.08f,0.06f,0.04f,1);
    rlVertex2f(x,y-2); rlVertex2f(x+w,y-2); rlVertex2f(x+w,y+5); rlVertex2f(x,y+5);
    rlEnd();
    float fill = (val/10.f)*w;
    rlBegin(RL_QUADS);
    rlColor4f(col.r/255.f,col.g/255.f,col.b/255.f,0.9f);
    rlVertex2f(x,y-1); rlVertex2f(x+fill,y-1); rlVertex2f(x+fill,y+4); rlVertex2f(x,y+4);
    rlEnd();
    rlSetLineWidth(1.f);
    rlBegin(RL_LINES);
    rlColor4f(0.4f,0.3f,0.15f,0.5f);
    rlVertex2f(x,y-2);   rlVertex2f(x+w,y-2);
    rlVertex2f(x+w,y-2); rlVertex2f(x+w,y+5);
    rlVertex2f(x+w,y+5); rlVertex2f(x,y+5);
    rlVertex2f(x,y+5);   rlVertex2f(x,y-2);
    rlEnd();
}

// Función de transición que usan las Screen (no tienen acceso a App)
// Instantly finish any ongoing transition
static void finalizarTransicion(GameState& gs) {
    if (!gs.slashActivo) return;
    gs.slashActivo = false;
    gs.slashX      = -1000.f;
    if (gs.estadoActual != gs.siguienteEstado) {
        gs.estadoActual = gs.siguienteEstado;
        // Set offsets to 0 so the screen appears instantly
        gs.menuOffset = gs.encicloOffset = gs.opcionesOffset = gs.musicaOffset = 0.f;
        gs.modoJuegoOffset = gs.selModoOffset = gs.configJCOffset = 0.f;
        gs.selEquipoOffset = gs.configDifiOffset = gs.cargaOffset = 0.f;
    }
}

static void transicion(GameState& gs, Estado dest) {
    // If mid-animation, finish it instantly before starting the new one
    if (gs.slashActivo) finalizarTransicion(gs);
    // La pausa usa animación propia, sin slash
    if (dest == PAUSA) {
        gs.estadoAnterior = gs.estadoActual;
        gs.estadoActual   = PAUSA;
        gs.pausaOffset    = 600.f;
        return;
    }
    gs.estadoAnterior  = gs.estadoActual;
    gs.slashActivo     = true;
    gs.siguienteEstado = dest;
}

// ============================================================
// MODO JUEGO — ¿Combate o Juego Completo?
// ============================================================
void ModoJuegoScreen::OnEnter(GameState& gs) {
    gs.opcionModoSel = 0;
}

void ModoJuegoScreen::Draw(GameState& gs) {
    float t  = gs.tiempo;
    float ox = gs.modoJuegoOffset;
    Background::battle(gs);
    Drawing::antorchaGrande(80, 230, t);
    Drawing::antorchaGrande(720, 230, t);

    drawTitulo("MODO DE JUEGO", 400+ox, 520, t, 0.95f,0.82f,0.2f);
    drawSeparador(140+ox, 495, 660+ox, 0.7f,0.5f,0.1f);

    struct { const char* label; const char* desc; } modos[2] = {
        {"COMBATE",       "Duelo directo entre dos equipos."},
        {"JUEGO COMPLETO","Conquista el tablero completo."},
    };

    for (int i = 0; i < 2; i++) {
        float y    = 430 - i*100.f;
        float offX = std::min(ox + i*30.f, 0.f);
        bool  sel  = (i == gs.opcionModoSel);
        rlPushMatrix(); rlTranslatef(offX,0,0);

        if (sel) {
            float bx=sinf(t*0.01f)*4, by=sinf(t*0.013f)*3;
            rlSetLineWidth(2); rlBegin(RL_LINES); rlColor4f(1,0.15f,0.1f,1);
            rlVertex2f(145+bx,y+38+by); rlVertex2f(655+bx,y+38+by);
            rlVertex2f(655+bx,y+38+by); rlVertex2f(695+bx,y-38+by);
            rlVertex2f(695+bx,y-38+by); rlVertex2f(185+bx,y-38+by);
            rlVertex2f(185+bx,y-38+by); rlVertex2f(145+bx,y+38+by);
            rlEnd();
            Drawing::panelBatalla(y, true);
            float esc = 1.2f+sinf(t*0.007f)*0.04f;
            rlPushMatrix(); rlTranslatef(310,y+8,0); rlScalef(esc,esc,1);
            Drawing::texto24(2,-7, modos[i].label, CFloat(0,0,0));
            Drawing::texto24(0,-5, modos[i].label, WHITE);
            rlPopMatrix();
            Drawing::texto12(310, y-18, modos[i].desc, CFloat(0.9f,0.85f,0.7f));
            Drawing::cursorAnimado(165, y, t);
            // Icono según el modo
            if (i==0)      Drawing::iconoEscudos(240, y, WHITE);
            else if (i==1) Drawing::iconoLibro(240, y, WHITE);
        } else {
            Drawing::panelBatalla(y, false);
            Drawing::texto24(310, y-8, modos[i].label, CFloat(0.7f,0.5f,0.5f));
            Color ic = CFloat(0.5f,0.5f,0.5f);
            if (i==0)      Drawing::iconoEscudos(240, y, ic);
            else if (i==1) Drawing::iconoLibro(240, y, ic);
        }
        rlPopMatrix();
    }
    Drawing::instrucciones(235+ox,50,"W/S: Navegar    ENTER: Confirmar    ESC: Volver",t);
    Drawing::botonVolver(60+ox, 565, t);
}

void ModoJuegoScreen::HandleInput(GameState& gs) {
    if (IsKeyPressed(KEY_W) || IsKeyPressed(KEY_UP))
        { if (--gs.opcionModoSel < 0) gs.opcionModoSel = 1; }
    if (IsKeyPressed(KEY_S) || IsKeyPressed(KEY_DOWN))
        { if (++gs.opcionModoSel > 1) gs.opcionModoSel = 0; }
    if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_KP_ENTER)) {
        gs.modoActual = (gs.opcionModoSel == 0) ? MODO_COMBATE : MODO_COMPLETO;
        transicion(gs, SELECCION_MODO);
    }
    if (IsKeyPressed(KEY_ESCAPE)) transicion(gs, MENU);
}

void ModoJuegoScreen::HandleMouse(GameState& gs) {
    float mxv=(float)GetMouseX()*800.f/GetScreenWidth(), myv=600.f-(float)GetMouseY()*600.f/GetScreenHeight();
    bool clicked=IsMouseButtonPressed(MOUSE_BUTTON_LEFT);
    bool moved=(GetMouseDelta().x!=0||GetMouseDelta().y!=0);
    // Back button
    if(clicked && mxv>40&&mxv<135&&myv>543&&myv<589){ transicion(gs,MENU); return; }
    for(int i=0;i<2;i++){
        float oy=430-i*100.f;
        if(mxv>145&&mxv<695&&myv>oy-38&&myv<oy+38){
            gs.opcionModoSel=i;
            if(clicked){
                gs.modoActual=(i==0)?MODO_COMBATE:MODO_COMPLETO;
                transicion(gs,SELECCION_MODO);
            }
        }
    }
}

// ============================================================
// SELECCION MODO — PvP / Contra IA
// Se reutiliza igual para Combate y Juego Completo.
// ============================================================
void SeleccionModoScreen::OnEnter(GameState& gs) {
    gs.opcionSelModoSel = 0;
}

void SeleccionModoScreen::Draw(GameState& gs) {
    float t  = gs.tiempo;
    float ox = gs.selModoOffset;
    Background::battle(gs);
    Drawing::antorchaGrande(80,230,t);
    Drawing::antorchaGrande(720,230,t);

    std::string cab = (gs.modoActual==MODO_COMBATE)
        ? "COMBATE - MODO DE JUEGO"
        : "JUEGO COMPLETO - MODO DE JUEGO";
    drawTitulo(cab, 400+ox, 520, t, 0.95f,0.82f,0.2f);
    drawSeparador(140+ox,495,660+ox, 0.7f,0.5f,0.1f);

    const char* opts[2]  = {"DOS JUGADORES (PvP)","CONTRA LA IA"};
    const char* descs[2] = {"Dos jugadores en el mismo dispositivo.",
                             "Enfrenta a la inteligencia artificial."};

    for(int i=0;i<2;i++){
        float y=390-i*110.f, offX=std::min(ox+i*30.f,0.f);
        bool sel=(i==gs.opcionSelModoSel);
        rlPushMatrix(); rlTranslatef(offX,0,0);
        if(sel){
            float bx=sinf(t*0.01f)*4,by=sinf(t*0.013f)*3;
            rlSetLineWidth(2); rlBegin(RL_LINES); rlColor4f(1,0.15f,0.1f,1);
            rlVertex2f(145+bx,y+35+by); rlVertex2f(655+bx,y+35+by);
            rlVertex2f(655+bx,y+35+by); rlVertex2f(695+bx,y-35+by);
            rlVertex2f(695+bx,y-35+by); rlVertex2f(185+bx,y-35+by);
            rlVertex2f(185+bx,y-35+by); rlVertex2f(145+bx,y+35+by);
            rlEnd();
            Drawing::panelBatalla(y,true);
            float esc=1.15f+sinf(t*0.007f)*0.04f;
            rlPushMatrix(); rlTranslatef(310,y+6,0); rlScalef(esc,esc,1);
            Drawing::texto24(2,-7,opts[i],CFloat(0,0,0));
            Drawing::texto24(0,-5,opts[i],WHITE);
            rlPopMatrix();
            Drawing::texto12(310,y-18,descs[i],CFloat(0.9f,0.85f,0.7f));
            Drawing::cursorAnimado(165,y,t);
            if(i==0) Drawing::iconoEscudos(240,y,WHITE);
            else     Drawing::iconoIA(240,y,WHITE);
        } else {
            Drawing::panelBatalla(y,false);
            Drawing::texto24(310,y-8,opts[i],CFloat(0.7f,0.5f,0.5f));
            Color ic=CFloat(0.5f,0.5f,0.5f);
            if(i==0) Drawing::iconoEscudos(240,y,ic);
            else     Drawing::iconoIA(240,y,ic);
        }
        rlPopMatrix();
    }
    Drawing::instrucciones(235+ox,50,"W/S: Navegar    ENTER: Confirmar    ESC: Volver",t);
    Drawing::botonVolver(60+ox, 565, t);
}

void SeleccionModoScreen::HandleInput(GameState& gs) {
    if (IsKeyPressed(KEY_W) || IsKeyPressed(KEY_UP))
        { if (--gs.opcionSelModoSel < 0) gs.opcionSelModoSel = 1; }
    if (IsKeyPressed(KEY_S) || IsKeyPressed(KEY_DOWN))
        { if (++gs.opcionSelModoSel > 1) gs.opcionSelModoSel = 0; }
    if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_KP_ENTER)) {
        if (gs.modoActual == MODO_COMPLETO) transicion(gs, CONFIG_JUEGO_COMPLETO);
        else                                transicion(gs, SELECCION_EQUIPO);
    }
    if (IsKeyPressed(KEY_ESCAPE)) transicion(gs, MODO_JUEGO);
}

void SeleccionModoScreen::HandleMouse(GameState& gs) {
    float mxv=(float)GetMouseX()*800.f/GetScreenWidth(),myv=600.f-(float)GetMouseY()*600.f/GetScreenHeight();
    bool clicked=IsMouseButtonPressed(MOUSE_BUTTON_LEFT);
    if(clicked && mxv>40&&mxv<135&&myv>543&&myv<589){ transicion(gs,MODO_JUEGO); return; }
    for(int i=0;i<2;i++){
        float oy=390-i*110.f;
        if(mxv>145&&mxv<695&&myv>oy-35&&myv<oy+35){
            gs.opcionSelModoSel=i;
            if(clicked){
                if(gs.modoActual==MODO_COMPLETO) transicion(gs,CONFIG_JUEGO_COMPLETO);
                else transicion(gs,SELECCION_EQUIPO);
            }
        }
    }
}

// ============================================================
// CONFIG JUEGO COMPLETO — Bando + Dificultad IA
// Dos paneles navegables con A/D para cambiar el foco.
// ============================================================
void ConfigJuegoCompletoScreen::OnEnter(GameState& gs) {
    gs.opcionBandoSel = 0;
    gs.opcionDifiSel  = 1;
    gs.configJCFoco   = 0;
    nombrePartida = "Partida Nueva";
    editandoNombre = false;
    slotCursor = 0;
}

void ConfigJuegoCompletoScreen::drawPanelBando(GameState& gs, float ox) {
    float t=gs.tiempo, panX=60+ox, panY=110, panW=300, panH=330;
    drawPanel(panX,panY,panW,panH, 0.65f,0.5f,0.08f, 0.06f,0.04f,0.02f);
    Drawing::texto18(panX+20, panY+panH-25, "ELIGE TU BANDO", CFloat(0.8f,0.7f,0.15f));
    drawSeparador(panX+10,panY+panH-38,panX+panW-10, 0.55f,0.4f,0.07f);

    // Luz, Oscuridad, Random
    float cols[3][3]={{0.9f,0.85f,0.25f},{0.5f,0.1f,0.75f},{0.3f,0.6f,0.9f}};
    const char* labels[3]={"LUZ","OSCURIDAD","RANDOM"};

    for(int i=0;i<3;i++){
        float by=panY+panH-80-i*82.f;
        bool sel=(i==gs.opcionBandoSel), hov=(i==hoverBando && !sel);
        bool foco=(gs.configJCFoco==0);
        // Fondo de fila: seleccionado > hover > normal
        float bgMul = sel ? 0.22f : (hov ? 0.14f : 0.07f);
        rlBegin(RL_QUADS);
        rlColor4f(cols[i][0]*bgMul, cols[i][1]*bgMul, cols[i][2]*bgMul, 0.9f);
        rlVertex2f(panX+12,by-28); rlVertex2f(panX+panW-12,by-28);
        rlVertex2f(panX+panW-12,by+28); rlVertex2f(panX+12,by+28);
        rlEnd();
        // Borde: seleccionado = animado grueso, hover = fino estático
        if(sel&&foco){
            float al=0.5f+0.4f*sinf(t*0.012f);
            rlSetLineWidth(2); rlBegin(RL_LINES);
            rlColor4f(cols[i][0]*al,cols[i][1]*al,cols[i][2]*al,1);
            rlVertex2f(panX+12,by-28); rlVertex2f(panX+panW-12,by-28);
            rlVertex2f(panX+panW-12,by-28); rlVertex2f(panX+panW-12,by+28);
            rlVertex2f(panX+panW-12,by+28); rlVertex2f(panX+12,by+28);
            rlVertex2f(panX+12,by+28); rlVertex2f(panX+12,by-28);
            rlEnd();
        } else if(hov){
            rlSetLineWidth(1); rlBegin(RL_LINES);
            rlColor4f(cols[i][0]*0.4f,cols[i][1]*0.4f,cols[i][2]*0.4f,0.7f);
            rlVertex2f(panX+12,by-28); rlVertex2f(panX+panW-12,by-28);
            rlVertex2f(panX+panW-12,by-28); rlVertex2f(panX+panW-12,by+28);
            rlVertex2f(panX+panW-12,by+28); rlVertex2f(panX+12,by+28);
            rlVertex2f(panX+12,by+28); rlVertex2f(panX+12,by-28);
            rlEnd();
        }
        // Icono de bando
        bool activo = sel || hov;
        if(i==0)      Drawing::iconoEspada(panX+35,by, activo?CFloat(cols[i][0],cols[i][1],cols[i][2]):CFloat(0.5f,0.5f,0.5f));
        else if(i==1) Drawing::iconoOpciones(panX+35,by, activo?CFloat(cols[i][0],cols[i][1],cols[i][2]):CFloat(0.5f,0.5f,0.5f));
        else          Drawing::iconoLibro(panX+35,by, activo?CFloat(cols[i][0],cols[i][1],cols[i][2]):CFloat(0.5f,0.5f,0.5f));

        float escL=sel&&foco?1.1f+sinf(t*0.008f)*0.03f:1.f;
        rlPushMatrix(); rlTranslatef(panX+65,by,0); rlScalef(escL,escL,1);
        Drawing::texto18(0,-7,labels[i], activo?CFloat(cols[i][0],cols[i][1],cols[i][2]):CFloat(0.55f,0.5f,0.4f));
        rlPopMatrix();
        if(sel&&foco) Drawing::cursorAnimado(panX+20,by,t);
    }
}

void ConfigJuegoCompletoScreen::drawPanelDificultad(GameState& gs, float ox) {
    float t=gs.tiempo, panX=440+ox, panY=110, panW=300, panH=330;
    bool esIA=(gs.opcionSelModoSel==1);
    float br=esIA?0.3f:0.2f, bg=esIA?0.5f:0.2f, bb=esIA?0.8f:0.2f;
    drawPanel(panX,panY,panW,panH, br,bg,bb, 0.04f,0.04f,0.07f);
    Drawing::texto18(panX+20, panY+panH-25, "DIFICULTAD IA",
        esIA?CFloat(0.5f,0.7f,1.f):CFloat(0.55f,0.55f,0.6f));
    drawSeparador(panX+10,panY+panH-38,panX+panW-10, br,bg,bb);

    if(!esIA){
        Drawing::texto12(panX+20,panY+panH/2,"Solo para modo IA.",CFloat(0.6f,0.55f,0.5f));
        return;
    }
    float cols[3][3]={{0.3f,0.8f,0.3f},{0.8f,0.8f,0.2f},{0.9f,0.2f,0.15f}};
    const char* noms[3]={"PLATERO","MH","SANSEGUNDO"};
    for(int i=0;i<3;i++){
        float by=panY+panH-72-i*82.f;
        bool sel=(i==gs.opcionDifiSel), hov=(i==hoverDifi && !sel);
        bool foco=(gs.configJCFoco==1);
        float bgMul = sel ? 0.18f : (hov ? 0.12f : 0.06f);
        rlBegin(RL_QUADS);
        rlColor4f(cols[i][0]*bgMul, cols[i][1]*bgMul, cols[i][2]*bgMul, 0.9f);
        rlVertex2f(panX+12,by-20); rlVertex2f(panX+panW-12,by-20);
        rlVertex2f(panX+panW-12,by+20); rlVertex2f(panX+12,by+20);
        rlEnd();
        if(sel&&foco){
            float al=0.5f+0.4f*sinf(t*0.012f);
            rlSetLineWidth(2); rlBegin(RL_LINES);
            rlColor4f(cols[i][0]*al,cols[i][1]*al,cols[i][2]*al,1);
            rlVertex2f(panX+12,by-20); rlVertex2f(panX+panW-12,by-20);
            rlVertex2f(panX+panW-12,by-20); rlVertex2f(panX+panW-12,by+20);
            rlVertex2f(panX+panW-12,by+20); rlVertex2f(panX+12,by+20);
            rlVertex2f(panX+12,by+20); rlVertex2f(panX+12,by-20);
            rlEnd();
        } else if(hov){
            rlSetLineWidth(1); rlBegin(RL_LINES);
            rlColor4f(cols[i][0]*0.4f,cols[i][1]*0.4f,cols[i][2]*0.4f,0.7f);
            rlVertex2f(panX+12,by-20); rlVertex2f(panX+panW-12,by-20);
            rlVertex2f(panX+panW-12,by-20); rlVertex2f(panX+panW-12,by+20);
            rlVertex2f(panX+panW-12,by+20); rlVertex2f(panX+12,by+20);
            rlVertex2f(panX+12,by+20); rlVertex2f(panX+12,by-20);
            rlEnd();
        }
        // Estrellas como indicador de nivel
        bool activo = sel || hov;
        for(int s=0;s<=i;s++)
            Drawing::circulo(panX+panW-20-s*14.f,by,4,8,
                activo?CFloat(cols[i][0],cols[i][1],cols[i][2]):CFloat(cols[i][0]*0.45f,cols[i][1]*0.45f,cols[i][2]*0.45f));
        float escL=sel&&foco?1.1f+sinf(t*0.008f)*0.03f:1.f;
        rlPushMatrix(); rlTranslatef(panX+45,by,0); rlScalef(escL,escL,1);
        Drawing::texto18(0,-7,noms[i], activo?CFloat(cols[i][0],cols[i][1],cols[i][2]):CFloat(0.5f,0.45f,0.4f));
        rlPopMatrix();
        if(sel&&foco) Drawing::cursorAnimado(panX+20,by,t);
    }
}

void ConfigJuegoCompletoScreen::drawSlotPicker(GameState& gs, float panX, float panY, float panW, float panH, float t) {
    Drawing::texto18(panX+panW/2-80, panY+panH-30, "ELEGIR SLOT DE GUARDADO", CFloat(0.4f,1.f,0.4f));
    drawSeparador(panX+20,panY+panH-48,panX+panW-20, 0.3f,0.6f,0.3f);

    auto slots = SaveSystem::ObtenerSlotsTablero();
    for (int i = 0; i < 4; i++) {
        float y = panY+panH-80-i*68.f;
        bool sel = (i == slotCursor);
        float h = 58;
        rlBegin(RL_QUADS);
        rlColor4f(sel?0.06f:0.03f, sel?0.12f:0.05f, sel?0.06f:0.03f, 0.95f);
        rlVertex2f(panX+20,y-h/2); rlVertex2f(panX+panW-20,y-h/2);
        rlVertex2f(panX+panW-20,y+h/2); rlVertex2f(panX+20,y+h/2);
        rlEnd();
        if (sel) {
            float al=0.5f+0.4f*sinf(t*0.012f);
            rlSetLineWidth(2); rlBegin(RL_LINES);
            rlColor4f(0.3f*al,0.8f*al,0.3f*al,1);
            rlVertex2f(panX+20,y-h/2); rlVertex2f(panX+panW-20,y-h/2);
            rlVertex2f(panX+panW-20,y-h/2); rlVertex2f(panX+panW-20,y+h/2);
            rlVertex2f(panX+panW-20,y+h/2); rlVertex2f(panX+20,y+h/2);
            rlVertex2f(panX+20,y+h/2); rlVertex2f(panX+20,y-h/2);
            rlEnd();
            Drawing::cursorAnimado(panX+28,y,t);
        }
        std::string label = "SLOT " + std::to_string(i+1);
        if (slots[i].valida) {
            Drawing::texto18(panX+45,y+8, label+": "+slots[i].nombre,
                sel?CFloat(1,0.95f,0.6f):CFloat(0.7f,0.65f,0.5f));
            Drawing::texto12(panX+45,y-10, slots[i].fecha+" | "+slots[i].bando,
                CFloat(0.5f,0.5f,0.4f));
        } else {
            Drawing::texto18(panX+45,y, label+" - vacio",
                sel?CFloat(0.7f,0.9f,0.7f):CFloat(0.5f,0.5f,0.4f));
        }
    }
}

void ConfigJuegoCompletoScreen::Draw(GameState& gs) {
    float t=gs.tiempo, ox=gs.configJCOffset;
    Background::library(gs);

    bool esPVP = (gs.opcionSelModoSel == 0);

    if (esPVP) {
        // PVP: solo slot picker + nombre
        float panX=140+ox, panY=80, panW=520, panH=400;
        drawPanel(panX,panY,panW,panH, 0.65f,0.5f,0.08f, 0.05f,0.03f,0.01f,0.90f);
        drawTitulo("NUEVA PARTIDA PVP", 400+ox, panY+panH-10, t, 0.85f,0.75f,0.15f);
        drawSlotPicker(gs, panX, panY, panW, panH, t);
        // Campo de nombre
        float nbX=panX+40, nbY=panY+10, nbW=panW-80, nbH=32;
        drawPanel(nbX,nbY,nbW,nbH, 0.08f,0.06f,0.02f, 0.04f,0.03f,0.01f);
        if(editandoNombre){
            float al=0.5f+0.4f*sinf(t*0.012f);
            rlSetLineWidth(2); rlBegin(RL_LINES);
            rlColor4f(0.9f*al,0.8f*al,0.2f*al,1);
            rlVertex2f(nbX,nbY); rlVertex2f(nbX+nbW,nbY);
            rlVertex2f(nbX+nbW,nbY); rlVertex2f(nbX+nbW,nbY+nbH);
            rlVertex2f(nbX+nbW,nbY+nbH); rlVertex2f(nbX,nbY+nbH);
            rlVertex2f(nbX,nbY+nbH); rlVertex2f(nbX,nbY);
            rlEnd();
        }
        Drawing::texto12(nbX-70,nbY+10,"NOMBRE:", editandoNombre?CFloat(1,0.9f,0.5f):CFloat(0.7f,0.6f,0.4f));
        std::string display = nombrePartida;
        if(editandoNombre && ((int)(t*0.006f))%2==0) display += "_";
        Drawing::texto18(nbX+10,nbY+7,display, editandoNombre?CFloat(1,0.95f,0.7f):CFloat(0.8f,0.75f,0.55f));
        Drawing::instrucciones(panX+40,panY-15,"W/S: Navegar    TAB: Nombre    ENTER: Confirmar    ESC: Volver",t);
        Drawing::botonVolver(60+ox, 565, t);
        return;
    }

    // IA: pantalla original con bando + dificultad
    drawPanel(45+ox,95,710,370, 0.65f,0.5f,0.08f, 0.05f,0.03f,0.01f,0.90f);
    drawTitulo("CONFIGURACION DE PARTIDA", 400+ox, 488, t, 0.85f,0.75f,0.15f);
    drawSeparador(60+ox,465,740+ox, 0.55f,0.4f,0.07f);
    drawPanelBando(gs,ox);
    drawPanelDificultad(gs,ox);
    // Indicador de qué panel está activo
    const char* focoTxt = (gs.configJCFoco==0)?"[ BANDO ] > Dificultad":"Bando > [ DIFICULTAD ]";
    Drawing::texto12(300+ox,120, focoTxt, CFloat(0.7f,0.6f,0.2f));
    // Campo de nombre de partida
    float nbX=160+ox, nbY=68, nbW=480, nbH=32;
    drawPanel(nbX,nbY,nbW,nbH, 0.08f,0.06f,0.02f, 0.04f,0.03f,0.01f);
    if(editandoNombre){
        float al=0.5f+0.4f*sinf(t*0.012f);
        rlSetLineWidth(2); rlBegin(RL_LINES);
        rlColor4f(0.9f*al,0.8f*al,0.2f*al,1);
        rlVertex2f(nbX,nbY); rlVertex2f(nbX+nbW,nbY);
        rlVertex2f(nbX+nbW,nbY); rlVertex2f(nbX+nbW,nbY+nbH);
        rlVertex2f(nbX+nbW,nbY+nbH); rlVertex2f(nbX,nbY+nbH);
        rlVertex2f(nbX,nbY+nbH); rlVertex2f(nbX,nbY);
        rlEnd();
    }
    Drawing::texto12(nbX-90,nbY+10,"NOMBRE:", editandoNombre?CFloat(1,0.9f,0.5f):CFloat(0.7f,0.6f,0.4f));
    std::string display = nombrePartida;
    if(editandoNombre && ((int)(t*0.006f))%2==0) display += "_";
    Drawing::texto18(nbX+10,nbY+7,display, editandoNombre?CFloat(1,0.95f,0.7f):CFloat(0.8f,0.75f,0.55f));

    // Botón confirmar
    drawPanel(280+ox,34,240,28, 0.7f,0.1f,0.05f, 0.12f,0.04f,0.02f);
    Drawing::texto18(295+ox,38,"CONFIRMAR", CFloat(1,0.9f,0.5f));
    Drawing::instrucciones(55+ox,14,"A/D: Panel  W/S: Navegar  TAB: Nombre  ENTER: Confirmar  ESC: Volver",t);
    Drawing::botonVolver(60+ox, 565, t);
}

void ConfigJuegoCompletoScreen::HandleInput(GameState& gs) {
    // TAB alterna entre edición del nombre y selección de paneles
    if (IsKeyPressed(KEY_TAB)) {
        editandoNombre = !editandoNombre;
        return;
    }

    // Si estamos editando el nombre, capturar texto
    if (editandoNombre) {
        int ch = GetCharPressed();
        while (ch > 0) {
            if (ch >= 32 && ch < 127 && (int)nombrePartida.size() < 30)
                nombrePartida += (char)ch;
            ch = GetCharPressed();
        }
        if (IsKeyPressed(KEY_BACKSPACE) && !nombrePartida.empty())
            nombrePartida.pop_back();
        if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_KP_ENTER))
            editandoNombre = false;  // salir de edición, no confirmar partida
        if (IsKeyPressed(KEY_ESCAPE))
            editandoNombre = false;
        return;
    }

    bool esPVP = (gs.opcionSelModoSel == 0);

    if (esPVP) {
        // PVP: navegar slots
        if (IsKeyPressed(KEY_W) || IsKeyPressed(KEY_UP))
            { if (--slotCursor < 0) slotCursor = 3; }
        if (IsKeyPressed(KEY_S) || IsKeyPressed(KEY_DOWN))
            { if (++slotCursor > 3) slotCursor = 0; }
        if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_KP_ENTER)) {
            gs.bandoSel      = BANDO_RANDOM;
            gs.dificultadSel = (Dificultad)0;
            gs.nombreGuardado = nombrePartida;
            PartidaGuardada p;
            p.slot = slotCursor;
            p.nombre = nombrePartida;
            p.modo = "Juego Completo";
            p.bando = "Random";
            p.dificultad = 0;
            p.vsAI = false;
            p.valida = true;
            time_t ahora = time(nullptr);
            char fechaBuf[32];
            struct tm tmInfo;
            localtime_s(&tmInfo, &ahora);
            strftime(fechaBuf, sizeof(fechaBuf), "%d/%m/%Y %H:%M", &tmInfo);
            p.fecha = fechaBuf;
            SaveSystem::GuardarSlotTablero(p);
            gs.partidaActualIdx = slotCursor;
            SaveSystem::cargarTodas(gs);
            transicion(gs, TABLERO);
        }
        if (IsKeyPressed(KEY_ESCAPE)) transicion(gs, SELECCION_MODO);
        return;
    }

    // IA: pantalla original
    bool esIA = (gs.opcionSelModoSel == 1);
    // A/D cambian el panel activo (solo si hay IA que configurar)
    if (IsKeyPressed(KEY_A) || IsKeyPressed(KEY_LEFT))
        { if (esIA) gs.configJCFoco = 0; }
    if (IsKeyPressed(KEY_D) || IsKeyPressed(KEY_RIGHT))
        { if (esIA) gs.configJCFoco = 1; }
    // W/S navegan dentro del panel activo
    if (IsKeyPressed(KEY_W) || IsKeyPressed(KEY_UP)) {
        if (gs.configJCFoco == 0)
            { if (--gs.opcionBandoSel < 0) gs.opcionBandoSel = 2; }
        else if (esIA)
            { if (--gs.opcionDifiSel < 0) gs.opcionDifiSel = 3; }
    }
    if (IsKeyPressed(KEY_S) || IsKeyPressed(KEY_DOWN)) {
        if (gs.configJCFoco == 0)
            { if (++gs.opcionBandoSel > 2) gs.opcionBandoSel = 0; }
        else if (esIA)
            { if (++gs.opcionDifiSel > 3) gs.opcionDifiSel = 0; }
    }
    if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_KP_ENTER)) {
        gs.bandoSel      = (Bando)(gs.opcionBandoSel + 1);
        gs.dificultadSel = (Dificultad)gs.opcionDifiSel;
        gs.nombreGuardado = nombrePartida;
        // Buscar primer slot libre
        auto slots = SaveSystem::ObtenerSlotsTablero();
        int freeSlot = -1;
        for (int i = 0; i < 4; i++) { if (!slots[i].valida) { freeSlot = i; break; } }
        if (freeSlot == -1) freeSlot = 0; // si todos ocupados, sobreescribir el 0
        // Crear la partida guardada
        PartidaGuardada p;
        p.slot = freeSlot;
        p.nombre = nombrePartida;
        p.modo = "Juego Completo";
        p.bando = (gs.bandoSel == BANDO_LUZ ? "Luz" :
            gs.bandoSel == BANDO_OSCURIDAD ? "Oscuridad" : "Random");
        p.dificultad = (int)gs.dificultadSel;
        p.vsAI = (gs.opcionSelModoSel == 1);
        p.valida = true;
        // Timestamp
        time_t ahora = time(nullptr);
        char fechaBuf[32];
        struct tm tmInfo;
        localtime_s(&tmInfo, &ahora);
        strftime(fechaBuf, sizeof(fechaBuf), "%d/%m/%Y %H:%M", &tmInfo);
        p.fecha = fechaBuf;
        SaveSystem::GuardarSlotTablero(p);
        gs.partidaActualIdx = freeSlot;
        SaveSystem::cargarTodas(gs);
        transicion(gs, TABLERO);
    }
    if (IsKeyPressed(KEY_ESCAPE)) transicion(gs, SELECCION_MODO);
}

void ConfigJuegoCompletoScreen::HandleMouse(GameState& gs) {
    float mxv=(float)GetMouseX()*800.f/GetScreenWidth(),myv=600.f-(float)GetMouseY()*600.f/GetScreenHeight(),ox=gs.configJCOffset;
    bool clicked=IsMouseButtonPressed(MOUSE_BUTTON_LEFT);
    bool moved = (GetMouseDelta().x != 0 || GetMouseDelta().y != 0);
    if(clicked && mxv>40&&mxv<135&&myv>543&&myv<589){ transicion(gs,SELECCION_MODO); return; }

    bool esPVP = (gs.opcionSelModoSel == 0);

    if (esPVP) {
        float panX=140+ox, panY=80, panW=520, panH=400;
        // Click en slots
        for (int i = 0; i < 4; i++) {
            float y = panY+panH-80-i*68.f;
            float h = 58;
            if (mxv>panX+20&&mxv<panX+panW-20&&myv>y-h/2&&myv<y+h/2) {
                if (moved) slotCursor = i;
                if (clicked) {
                    slotCursor = i;
                    // Confirmar directamente al hacer click en un slot
                    gs.bandoSel = BANDO_RANDOM;
                    gs.dificultadSel = (Dificultad)0;
                    gs.nombreGuardado = nombrePartida;
                    PartidaGuardada p;
                    p.slot = slotCursor;
                    p.nombre = nombrePartida;
                    p.modo = "Juego Completo";
                    p.bando = "Random";
                    p.dificultad = 0;
                    p.vsAI = false;
                    p.valida = true;
                    time_t ahora = time(nullptr);
                    char fechaBuf[32];
                    struct tm tmInfo;
                    localtime_s(&tmInfo, &ahora);
                    strftime(fechaBuf, sizeof(fechaBuf), "%d/%m/%Y %H:%M", &tmInfo);
                    p.fecha = fechaBuf;
                    SaveSystem::GuardarSlotTablero(p);
                    gs.partidaActualIdx = slotCursor;
                    SaveSystem::cargarTodas(gs);
                    transicion(gs, TABLERO);
                    return;
                }
            }
        }
        // Click en campo nombre
        float nbX=panX+40, nbY=panY+10, nbW=panW-80, nbH=32;
        if(clicked && mxv>nbX&&mxv<nbX+nbW&&myv>nbY&&myv<nbY+nbH)
            editandoNombre = true;
        return;
    }

    // IA: pantalla original
    // Hover: resaltar sin seleccionar; Click: seleccionar
    hoverBando = -1;
    hoverDifi  = -1;
    if(mxv>60+ox&&mxv<360+ox&&myv>110&&myv<440){
        if(moved) gs.configJCFoco=0;
        for(int i=0;i<3;i++){
            float by=440-80-i*82.f;
            if(myv>by-28&&myv<by+28){
                hoverBando=i;
                if(clicked) gs.opcionBandoSel=i;
            }
        }
    }
    if(mxv>440+ox&&mxv<740+ox&&myv>110&&myv<440){
        if(moved) gs.configJCFoco=1;
        for(int i=0;i<3;i++){
            float by=440-72-i*82.f;
            if(myv>by-20&&myv<by+20){
                hoverDifi=i;
                if(clicked) gs.opcionDifiSel=i;
            }
        }
    }
    // Click: name field
    if(clicked && mxv>160+ox&&mxv<640+ox&&myv>68&&myv<100){
        editandoNombre = true;
    }
    // Click: confirm button
    if(clicked && mxv>280+ox&&mxv<520+ox&&myv>34&&myv<62){
        editandoNombre = false;
        gs.bandoSel=(Bando)(gs.opcionBandoSel+1);
        gs.dificultadSel=(Dificultad)gs.opcionDifiSel;
        gs.nombreGuardado = nombrePartida;
        auto slots = SaveSystem::ObtenerSlotsTablero();
        int freeSlot = -1;
        for (int i = 0; i < 4; i++) { if (!slots[i].valida) { freeSlot = i; break; } }
        if (freeSlot == -1) freeSlot = 0;
        PartidaGuardada p;
        p.slot = freeSlot;
        p.nombre = nombrePartida;
        p.modo = "Juego Completo";
        p.bando = (gs.bandoSel == BANDO_LUZ ? "Luz" :
            gs.bandoSel == BANDO_OSCURIDAD ? "Oscuridad" : "Random");
        p.dificultad = (int)gs.dificultadSel;
        p.vsAI = (gs.opcionSelModoSel == 1);
        p.valida = true;
        time_t ahora = time(nullptr);
        char fechaBuf[32];
        struct tm tmInfo;
        localtime_s(&tmInfo, &ahora);
        strftime(fechaBuf, sizeof(fechaBuf), "%d/%m/%Y %H:%M", &tmInfo);
        p.fecha = fechaBuf;
        SaveSystem::GuardarSlotTablero(p);
        gs.partidaActualIdx = freeSlot;
        SaveSystem::cargarTodas(gs);
        transicion(gs, TABLERO);
    }
}

// ============================================================
// SELECCION EQUIPO — Luz u Oscuridad (Combate)
// Primero elige J1, luego J2 si es PvP.
// ============================================================
void SeleccionEquipoScreen::OnEnter(GameState& gs) {
    gs.equipoSel1    = 0;
    gs.equipoSel2    = 1;
    gs.equipoFoco    = 0;
    gs.seleccionandoJ1 = true;
}

void SeleccionEquipoScreen::drawBarraStat(float x, float y, float w, int val, Color col) {
    ::drawBarraStat(x,y,w,val,col);
}

void SeleccionEquipoScreen::drawTarjetaEquipo(GameState& gs, int idx,
    float cx, float cy, bool activo, float t)
{
    float w=185, h=270;
    float x=cx-w/2, y=cy-h/2;
    DatosEquipo& eq=gs.equipos[idx];

    // Aura pulsante cuando está seleccionado
    if(activo){
        float al=0.12f+0.08f*sinf(t*0.01f);
        Drawing::circulo(cx,cy,140,36,CFloat(eq.colorR,eq.colorG,eq.colorB,al));
    }

    drawPanel(x,y,w,h,
        activo?eq.colorR:0.3f,
        activo?eq.colorG:0.25f,
        activo?eq.colorB:0.2f,
        0.05f+eq.colorR*0.04f, 0.04f+eq.colorG*0.03f, 0.04f+eq.colorB*0.05f,
        0.92f);

    // Simbolo del equipo (sol para Luz, craneo para Oscuridad)
    float fl = activo ? sinf(t*0.012f)*5 : 0;
    float sEsc = activo ? 1.2f + sinf(t*0.008f)*0.08f : 0.9f;
    if(idx==0) Drawing::simboloLuz(cx, cy+20+fl, sEsc, t);
    else       Drawing::simboloOscuridad(cx, cy+20+fl, sEsc, t);

    // Nombre del equipo
    Drawing::texto18(cx-(float)MeasureText(eq.nombre.c_str(),18)/2.f,
        y+h-28, eq.nombre,
        activo?CFloat(eq.colorR,eq.colorG,eq.colorB):CFloat(0.65f,0.6f,0.5f));
}

void SeleccionEquipoScreen::Draw(GameState& gs) {
    float t=gs.tiempo, ox=gs.selEquipoOffset;
    Background::library(gs);

    drawPanel(35+ox,80,730,440, 0.65f,0.5f,0.08f, 0.05f,0.03f,0.01f,0.88f);
    drawTitulo("SELECCION DE EQUIPO", 400+ox, 502, t, 0.85f,0.75f,0.15f);

    // Indicador de quién elige
    bool esP1=gs.seleccionandoJ1;
    std::string quienElige = esP1 ? "JUGADOR 1 - Elige tu equipo" : "JUGADOR 2 - Elige tu equipo";
    Color cJ = esP1 ? CFloat(0.9f,0.85f,0.3f) : CFloat(0.5f,0.6f,1.f);
    Drawing::texto18(210+ox, 473, quienElige, cJ);
    drawSeparador(55+ox,462,745+ox, 0.55f,0.4f,0.07f);

    // Dos tarjetas, una por equipo
    float cx1=220+ox, cx2=580+ox, cy=285;
    int activo1 = esP1 ? gs.equipoSel1 : gs.equipoSel2;  // cuál tiene el foco ahora

    drawTarjetaEquipo(gs, 0, cx1, cy, activo1==0, t);
    drawTarjetaEquipo(gs, 1, cx2, cy, activo1==1, t);

    // Etiquetas P1/P2 sobre la tarjeta correspondiente
    if(gs.equipoSel1==0) Drawing::texto12(cx1-10, cy+148, "P1", CFloat(0.9f,0.85f,0.3f));
    if(gs.equipoSel1==1) Drawing::texto12(cx2-10, cy+148, "P1", CFloat(0.9f,0.85f,0.3f));
    if(!gs.seleccionandoJ1){
        if(gs.equipoSel2==0) Drawing::texto12(cx1+12, cy+148, "P2", CFloat(0.5f,0.6f,1.f));
        if(gs.equipoSel2==1) Drawing::texto12(cx2+12, cy+148, "P2", CFloat(0.5f,0.6f,1.f));
    }

    // Descripción del equipo con foco
    DatosEquipo& eq = gs.equipos[activo1];
    Drawing::texto12(90+ox, 128, eq.descripcion, CFloat(0.75f,0.7f,0.55f));

    // Botón de avance
    bool esPvP=(gs.opcionSelModoSel==0);
    std::string btnTxt = (esP1&&esPvP) ? "SIGUIENTE" : "LUCHAR";
    drawPanel(275+ox,56,250,36, 0.7f,0.1f,0.05f, 0.12f,0.04f,0.02f);
    Drawing::texto18(290+ox,62, btnTxt, CFloat(1,0.9f,0.5f));

    Drawing::instrucciones(125+ox,40,
        "Click: Elegir equipo    ESC: Volver", t);
    Drawing::botonVolver(60+ox, 565, t);
}

void SeleccionEquipoScreen::HandleInput(GameState& gs) {
    int n = (int)gs.equipos.size();  // siempre 2
    // A/D cambian el equipo del jugador activo
    if (IsKeyPressed(KEY_A) || IsKeyPressed(KEY_LEFT)) {
        if (gs.seleccionandoJ1) { if (--gs.equipoSel1 < 0) gs.equipoSel1 = n - 1; }
        else                    { if (--gs.equipoSel2 < 0) gs.equipoSel2 = n - 1; }
    }
    if (IsKeyPressed(KEY_D) || IsKeyPressed(KEY_RIGHT)) {
        if (gs.seleccionandoJ1) { if (++gs.equipoSel1 >= n) gs.equipoSel1 = 0; }
        else                    { if (++gs.equipoSel2 >= n) gs.equipoSel2 = 0; }
    }
    if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_KP_ENTER)) {
        bool esPvP = (gs.opcionSelModoSel == 0);
        if (gs.seleccionandoJ1 && esPvP) {
            gs.seleccionandoJ1 = false;  // J2 elige ahora
        } else {
            // En modo IA el equipo 2 es el contrario automático
            if (!esPvP) gs.equipoSel2 = 1 - gs.equipoSel1;
            if (gs.modoActual == MODO_COMBATE)
                transicion(gs, esPvP ? CUCHAU_COMBATE : CONFIG_DIFICULTAD);
            else
                transicion(gs, esPvP ? TABLERO : CONFIG_DIFICULTAD);
        }
    }
    if (IsKeyPressed(KEY_ESCAPE)) {
        if (!gs.seleccionandoJ1) gs.seleccionandoJ1 = true;
        else transicion(gs, SELECCION_MODO);
    }
}

void SeleccionEquipoScreen::HandleMouse(GameState& gs) {
    float mxv=(float)GetMouseX()*800.f/GetScreenWidth(),myv=600.f-(float)GetMouseY()*600.f/GetScreenHeight(),ox=gs.selEquipoOffset;
    bool clicked=IsMouseButtonPressed(MOUSE_BUTTON_LEFT);
    if(clicked && mxv>40&&mxv<135&&myv>543&&myv<589){
        if(!gs.seleccionandoJ1) gs.seleccionandoJ1=true;
        else transicion(gs,SELECCION_MODO);
        return;
    }
    float cx1=220+ox, cx2=580+ox, cy=285, hw=92, hh=135;
    // Hover: highlight team card under mouse
    if(mxv>cx1-hw&&mxv<cx1+hw&&myv>cy-hh&&myv<cy+hh){
        if(gs.seleccionandoJ1) gs.equipoSel1=0; else gs.equipoSel2=0;
    }
    if(mxv>cx2-hw&&mxv<cx2+hw&&myv>cy-hh&&myv<cy+hh){
        if(gs.seleccionandoJ1) gs.equipoSel1=1; else gs.equipoSel2=1;
    }
    if(!clicked) return;
    // Click on a card or button: select + confirm
    auto confirmar = [&](){
        bool esPvP=(gs.opcionSelModoSel==0);
        if(gs.seleccionandoJ1&&esPvP){ gs.seleccionandoJ1=false; return; }
        if(!esPvP) gs.equipoSel2 = 1 - gs.equipoSel1;
        if(gs.modoActual==MODO_COMBATE){
            if(esPvP) transicion(gs,CUCHAU_COMBATE);
            else transicion(gs,CONFIG_DIFICULTAD);
        } else {
            if(esPvP) transicion(gs,TABLERO);
            else transicion(gs,CONFIG_DIFICULTAD);
        }
    };
    if(mxv>cx1-hw&&mxv<cx1+hw&&myv>cy-hh&&myv<cy+hh) confirmar();
    if(mxv>cx2-hw&&mxv<cx2+hw&&myv>cy-hh&&myv<cy+hh) confirmar();
    if(mxv>275+ox&&mxv<525+ox&&myv>56&&myv<92) confirmar();
}

// ============================================================
// CONFIG DIFICULTAD IA — solo Combate con IA
// ============================================================
void ConfigDificultadScreen::OnEnter(GameState& gs) {
    gs.opcionDifiCombateSel=1;
}

void ConfigDificultadScreen::Draw(GameState& gs) {
    float t=gs.tiempo, ox=gs.configDifiOffset;
    Background::theater(gs);
    Drawing::antorchaGrande(100,140,t); Drawing::antorchaGrande(700,140,t);

    drawPanel(155+ox,128,490,350, 0.3f,0.5f,0.8f, 0.03f,0.04f,0.07f,0.92f);
    drawTitulo("DIFICULTAD DE LA IA", 400+ox, 494, t, 0.5f,0.7f,1.f);
    drawSeparador(170+ox,470,630+ox, 0.3f,0.5f,0.8f);

    // Recordatorio de equipos elegidos
    if(!gs.equipos.empty()){
        DatosEquipo& e1=gs.equipos[gs.equipoSel1];
        DatosEquipo& e2=gs.equipos[gs.equipoSel2];
        Drawing::texto12(170+ox,452,"Tu equipo: "+e1.nombre, CFloat(0.9f,0.85f,0.3f));
        Drawing::texto12(390+ox,452,"IA: "+e2.nombre,         CFloat(0.5f,0.6f,1.f));
    }

    float cols[3][3]={{0.3f,0.8f,0.3f},{0.8f,0.8f,0.2f},{0.9f,0.2f,0.15f}};
    const char* noms[3]={"PLATERO","MH","SANSEGUNDO"};
    const char* descs[3]={"IA facil. Comete errores con frecuencia.",
                           "IA defensiva. Un reto equilibrado.",
                           "IA agresiva. Sin misericordia."};
    for(int i=0;i<3;i++){
        float y=410-i*85.f;
        bool sel=(i==gs.opcionDifiCombateSel);
        rlBegin(RL_QUADS);
        rlColor4f(cols[i][0]*(sel?0.18f:0.06f),
                  cols[i][1]*(sel?0.18f:0.06f),
                  cols[i][2]*(sel?0.18f:0.06f),0.9f);
        rlVertex2f(170+ox,y-24); rlVertex2f(630+ox,y-24);
        rlVertex2f(630+ox,y+24); rlVertex2f(170+ox,y+24);
        rlEnd();
        if(sel){
            float al=0.5f+0.4f*sinf(t*0.012f);
            rlSetLineWidth(2); rlBegin(RL_LINES);
            rlColor4f(cols[i][0]*al,cols[i][1]*al,cols[i][2]*al,1);
            rlVertex2f(170+ox,y-24); rlVertex2f(630+ox,y-24);
            rlVertex2f(630+ox,y-24); rlVertex2f(630+ox,y+24);
            rlVertex2f(630+ox,y+24); rlVertex2f(170+ox,y+24);
            rlVertex2f(170+ox,y+24); rlVertex2f(170+ox,y-24);
            rlEnd();
            Drawing::cursorAnimado(178+ox,y,t);
        }
        for(int s=0;s<=i;s++)
            Drawing::circulo(610+ox-s*16.f,y,4,10,
                sel?CFloat(cols[i][0],cols[i][1],cols[i][2]):CFloat(cols[i][0]*0.4f,cols[i][1]*0.4f,cols[i][2]*0.4f));
        float escL=sel?1.1f+sinf(t*0.008f)*0.03f:1.f;
        rlPushMatrix(); rlTranslatef(200+ox,y+2,0); rlScalef(escL,escL,1);
        Drawing::texto18(0,-8,noms[i], sel?CFloat(cols[i][0],cols[i][1],cols[i][2]):CFloat(0.5f,0.5f,0.55f));
        rlPopMatrix();
        if(sel) Drawing::texto12(270+ox,y-20,descs[i],CFloat(0.8f,0.8f,0.85f));
    }
    drawPanel(280+ox,141,240,36, 0.7f,0.1f,0.05f, 0.12f,0.04f,0.02f);
    Drawing::texto18(295+ox,147,"LUCHAR",CFloat(1,0.9f,0.5f));
    Drawing::instrucciones(175+ox,50,"W/S: Navegar    ENTER: Confirmar    ESC: Volver",t);
    Drawing::botonVolver(60+ox, 565, t);
}

void ConfigDificultadScreen::HandleInput(GameState& gs) {
    if (IsKeyPressed(KEY_W) || IsKeyPressed(KEY_UP))
        { if (--gs.opcionDifiCombateSel < 0) gs.opcionDifiCombateSel = 2; }
    if (IsKeyPressed(KEY_S) || IsKeyPressed(KEY_DOWN))
        { if (++gs.opcionDifiCombateSel > 2) gs.opcionDifiCombateSel = 0; }
    if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_KP_ENTER)) {
        gs.dificultadSel = (Dificultad)gs.opcionDifiCombateSel;
        if (gs.modoActual == MODO_COMBATE) transicion(gs, CUCHAU_COMBATE);
        else                               transicion(gs, TABLERO);
    }
    if (IsKeyPressed(KEY_ESCAPE)) transicion(gs, SELECCION_EQUIPO);
}

void ConfigDificultadScreen::HandleMouse(GameState& gs) {
    float mxv=(float)GetMouseX()*800.f/GetScreenWidth(),myv=600.f-(float)GetMouseY()*600.f/GetScreenHeight(),ox=gs.configDifiOffset;
    bool clicked=IsMouseButtonPressed(MOUSE_BUTTON_LEFT);
    if(clicked && mxv>40&&mxv<135&&myv>543&&myv<589){ transicion(gs,SELECCION_EQUIPO); return; }
    // Hover: highlight difficulty
    for(int i=0;i<3;i++){
        float y=410-i*85.f;
        if(mxv>170+ox&&mxv<630+ox&&myv>y-24&&myv<y+24){
            gs.opcionDifiCombateSel=i;
            // Click on a difficulty row: select + confirm
            if(clicked){
                gs.dificultadSel=(Dificultad)i;
                if(gs.modoActual==MODO_COMBATE) transicion(gs,CUCHAU_COMBATE);
                else transicion(gs,TABLERO);
            }
        }
    }
    // Click: confirm button
    if(clicked && mxv>280+ox&&mxv<520+ox&&myv>141&&myv<177){
        gs.dificultadSel=(Dificultad)gs.opcionDifiCombateSel;
        if(gs.modoActual==MODO_COMBATE) transicion(gs,CUCHAU_COMBATE);
        else transicion(gs,TABLERO);
    }
}

// ============================================================
// CARGAR PARTIDA
// Lee del disco al entrar y muestra las partidas guardadas.
// ============================================================
void CargarPartidaScreen::OnEnter(GameState& gs) {
    gs.opcionCargaSel = 0;
    gs.opcionSlotSel = 0;
    seccionActiva = 0;
    dialogoActivo = false;
    dialogoOpcion = 0;
    editandoNombre = false;
    slotsTablero = SaveSystem::ObtenerSlotsTablero();
    slotsCombate = SaveSystem::ObtenerSlots();
}

void CargarPartidaScreen::drawTarjetaPartida(const PartidaGuardada& p,
    float x, float y, float w, bool sel, float t)
{
    float h=80;
    rlBegin(RL_QUADS);
    rlColor4f(0.6f*(sel?0.14f:0.06f),0.45f*(sel?0.14f:0.06f),0.1f*(sel?0.14f:0.06f),0.95f);
    rlVertex2f(x,y); rlVertex2f(x+w,y); rlVertex2f(x+w,y+h); rlVertex2f(x,y+h);
    rlEnd();
    float al=sel?0.6f+0.4f*sinf(t*0.012f):0.3f;
    rlSetLineWidth(sel?2.f:1.f);
    rlBegin(RL_LINES);
    rlColor4f(0.65f*al,0.5f*al,0.08f*al,1);
    rlVertex2f(x,y);   rlVertex2f(x+w,y);
    rlVertex2f(x+w,y); rlVertex2f(x+w,y+h);
    rlVertex2f(x+w,y+h); rlVertex2f(x,y+h);
    rlVertex2f(x,y+h); rlVertex2f(x,y);
    rlEnd();

    std::string label = "SLOT " + std::to_string(p.slot + 1);
    if (!p.valida) {
        Drawing::texto12(x + 10, y + h / 2 - 6, label + "  -  vacio", CFloat(0.55f, 0.5f, 0.4f));
        if(sel) Drawing::cursorAnimado(x+5,y+h/2,t);
        return;
    }

    Drawing::texto18(x+12,y+h-24, label + ": " + p.nombre,
        sel?CFloat(1,0.9f,0.5f):CFloat(0.75f,0.7f,0.55f));
    Drawing::texto12(x+12,y+h-40, p.bando + " | " + (p.vsAI ? "vs IA" : "PvP"),
        CFloat(0.6f,0.55f,0.4f));
    Drawing::texto12(x+12,y+h-52,p.fecha,CFloat(0.65f,0.6f,0.5f));

    // Indicador de estado guardado
    Drawing::texto12(x+12,y+9,
        p.boardState.empty() ? "Sin progreso" : "Progreso guardado",
        p.boardState.empty() ? CFloat(0.5f,0.45f,0.3f) : CFloat(0.4f,0.7f,0.4f));

    if(sel) Drawing::cursorAnimado(x+5,y+h/2,t);
}

void CargarPartidaScreen::drawTarjetaSlot(const SaveData& s,
    float x, float y, float w, bool sel, float t)
{
    float h = 72;
    rlBegin(RL_QUADS);
    rlColor4f(sel ? 0.04f : 0.02f, sel ? 0.08f : 0.04f, sel ? 0.14f : 0.06f, 0.95f);
    rlVertex2f(x, y); rlVertex2f(x + w, y); rlVertex2f(x + w, y + h); rlVertex2f(x, y + h);
    rlEnd();
    float al = sel ? 0.6f + 0.4f * sinf(t * 0.012f) : 0.3f;
    rlSetLineWidth(sel ? 2.f : 1.f);
    rlBegin(RL_LINES);
    rlColor4f(0.2f * al, 0.4f * al, 0.7f * al, 1);
    rlVertex2f(x, y);     rlVertex2f(x + w, y);
    rlVertex2f(x + w, y);   rlVertex2f(x + w, y + h);
    rlVertex2f(x + w, y + h); rlVertex2f(x, y + h);
    rlVertex2f(x, y + h);   rlVertex2f(x, y);
    rlEnd();

    std::string label = "SLOT " + std::to_string(s.slot + 1);
    if (!s.valida) {
        Drawing::texto12(x + 10, y + h / 2 - 6, label + "  -  vacio", CFloat(0.55f, 0.5f, 0.6f));
        return;
    }
    Drawing::texto18(x + 10, y + h - 22,
        label + ":  " + s.nombreP1 + " vs " + s.nombreP2,
        sel ? CFloat(0.5f, 0.8f, 1.f) : CFloat(0.55f, 0.7f, 0.85f));
    std::string difi = s.dificultad == 0 ? "Facil" : s.dificultad == 1 ? "Normal" :
        s.dificultad == 2 ? "Dificil" : "Maestro";
    Drawing::texto12(x + 10, y + h - 38, s.modoIA ? "IA | " + difi : "PvP", CFloat(0.5f, 0.6f, 0.75f));
    Drawing::texto12(x + 10, y + 8,
        s.nombreP1 + " " + std::to_string((int)s.vidaP1) + "hp  |  " +
        s.nombreP2 + " " + std::to_string((int)s.vidaP2) + "hp",
        CFloat(0.45f, 0.6f, 0.5f));
    if (sel) Drawing::cursorAnimado(x + 4, y + h / 2, t);
}

void CargarPartidaScreen::accionCargar(GameState& gs) {
    if (seccionActiva == 0 && slotsTablero[gs.opcionCargaSel].valida) {
        const auto& p = slotsTablero[gs.opcionCargaSel];
        gs.modoActual = MODO_COMPLETO;
        gs.dificultadSel = (Dificultad)p.dificultad;
        gs.bandoSel = (p.bando == "Luz") ? BANDO_LUZ :
            (p.bando == "Oscuridad") ? BANDO_OSCURIDAD : BANDO_RANDOM;
        gs.opcionSelModoSel = p.vsAI ? 1 : 0;
        gs.boardStatePendiente = p.boardState;
        gs.partidaActualIdx = p.slot;
        SaveSystem::cargarTodas(gs);
        // Buscar el indice real en el vector (slot != indice si hay huecos)
        for (int i = 0; i < (int)gs.partidas.size(); i++) {
            if (gs.partidas[i].slot == p.slot) { gs.partidaActualIdx = i; break; }
        }
        transicion(gs, TABLERO);
    } else if (seccionActiva == 1 && slotsCombate[gs.opcionSlotSel].valida) {
        SaveSystem::pendiente = slotsCombate[gs.opcionSlotSel];
        transicion(gs, CUCHAU_COMBATE);
    }
    dialogoActivo = false;
}

void CargarPartidaScreen::accionEliminar(GameState& gs) {
    if (seccionActiva == 0 && slotsTablero[gs.opcionCargaSel].valida) {
        SaveSystem::BorrarSlotTablero(gs.opcionCargaSel);
        slotsTablero = SaveSystem::ObtenerSlotsTablero();
    } else if (seccionActiva == 1 && slotsCombate[gs.opcionSlotSel].valida) {
        SaveSystem::BorrarCombate(gs.opcionSlotSel);
        slotsCombate = SaveSystem::ObtenerSlots();
    }
    dialogoActivo = false;
}

void CargarPartidaScreen::drawDialogo(GameState& gs, float t, float ox) {
    // Oscurecer fondo
    rlBegin(RL_QUADS);
    rlColor4f(0, 0, 0, 0.55f);
    rlVertex2f(0, 0); rlVertex2f(800, 0); rlVertex2f(800, 600); rlVertex2f(0, 600);
    rlEnd();

    // Panel del diálogo
    float pw = 280, ph = 200;
    float px = 400 + ox - pw / 2, py = 300 - ph / 2;
    drawPanel(px, py, pw, ph, 0.65f, 0.5f, 0.08f, 0.07f, 0.04f, 0.02f, 0.97f);

    // Título: nombre del slot seleccionado
    std::string titulo;
    int selIdx = (seccionActiva == 0) ? gs.opcionCargaSel : gs.opcionSlotSel;
    if (seccionActiva == 0 && slotsTablero[selIdx].valida)
        titulo = slotsTablero[selIdx].nombre;
    else if (seccionActiva == 1 && slotsCombate[selIdx].valida)
        titulo = slotsCombate[selIdx].nombreP1 + " vs " + slotsCombate[selIdx].nombreP2;
    else
        titulo = "Slot " + std::to_string(selIdx + 1);

    float tw = (float)MeasureText(titulo.c_str(), 16);
    Drawing::texto18(px + pw / 2 - tw / 2, py + ph - 28, titulo, CFloat(1, 0.9f, 0.5f));
    drawSeparador(px + 15, py + ph - 42, px + pw - 15, 0.55f, 0.4f, 0.07f);

    // Opciones: CARGAR, RENOMBRAR, ELIMINAR
    const char* opciones[] = { "CARGAR", "RENOMBRAR", "ELIMINAR" };
    // Solo tablero tiene RENOMBRAR
    int numOpciones = (seccionActiva == 0) ? 3 : 2;
    int opcionesMap[] = { 0, 1, 2 }; // para combate: 0=CARGAR, 2=ELIMINAR
    if (seccionActiva == 1) { opcionesMap[1] = 2; }

    if (editandoNombre) {
        // Campo de edición de nombre
        Drawing::texto18(px + 20, py + ph - 80, "Nuevo nombre:", CFloat(0.8f, 0.75f, 0.5f));
        float fieldY = py + ph - 110;
        drawPanel(px + 15, fieldY, pw - 30, 28, 0.4f, 0.35f, 0.1f, 0.03f, 0.02f, 0.01f, 0.9f);
        // Cursor parpadeante
        std::string display = nuevoNombre;
        if (((int)(t * 0.008f)) % 2 == 0) display += "_";
        Drawing::texto18(px + 22, fieldY + 6, display, WHITE);
        Drawing::instrucciones(px + 20, py + 12, "ENTER: Confirmar    ESC: Cancelar", t);
    } else {
        for (int i = 0; i < numOpciones; i++) {
            int opIdx = opcionesMap[i];
            bool sel = (dialogoOpcion == i);
            float y = py + ph - 75 - i * 45.f;

            Color col;
            if (opIdx == 2) // ELIMINAR
                col = sel ? CFloat(1.f, 0.5f, 0.5f) : CFloat(0.7f, 0.4f, 0.4f);
            else if (opIdx == 1) // RENOMBRAR
                col = sel ? CFloat(0.7f, 0.9f, 1.f) : CFloat(0.5f, 0.65f, 0.75f);
            else // CARGAR
                col = sel ? CFloat(0.5f, 1.f, 0.5f) : CFloat(0.4f, 0.7f, 0.4f);

            float otw = (float)MeasureText(opciones[opIdx], 16);
            Drawing::texto18(px + pw / 2 - otw / 2, y - 5, opciones[opIdx], col);
            if (sel) Drawing::cursorAnimado(px + pw / 2 - otw / 2 - 20, y, t);
        }
        Drawing::instrucciones(px + 20, py + 12, "W/S: Navegar    ENTER: Confirmar    ESC: Volver", t);
    }
}

void CargarPartidaScreen::Draw(GameState& gs) {
    float t = gs.tiempo, ox = gs.cargaOffset;
    Background::library(gs);
    drawPanel(50 + ox, 88, 700, 428, 0.65f, 0.5f, 0.08f, 0.05f, 0.03f, 0.01f, 0.90f);
    drawTitulo("CARGAR PARTIDA", 400 + ox, 530, t, 0.85f, 0.75f, 0.15f);
    drawSeparador(65 + ox, 505, 735 + ox, 0.55f, 0.4f, 0.07f);

    // Línea divisoria central
    float cx = 400 + ox;
    rlBegin(RL_LINES);
    rlColor4f(0.4f, 0.35f, 0.1f, 0.5f);
    rlVertex2f(cx, 100); rlVertex2f(cx, 500);
    rlEnd();

    // — Columna izquierda: partidas de tablero —
    Color colIzq = (seccionActiva == 0) ? CFloat(1, 0.9f, 0.5f) : CFloat(0.65f, 0.6f, 0.45f);
    Drawing::texto12(90 + ox, 490, "JUEGO COMPLETO", colIzq);

    {
        float startY = 472, step = 88;
        for (int i = 0; i < 4; i++) {
            float y = startY - i * step;
            bool sel = (seccionActiva == 0 && i == gs.opcionCargaSel);
            drawTarjetaPartida(slotsTablero[i], 65 + ox, y - 72, 320, sel, t);
        }
    }

    // — Columna derecha: slots de combate —
    Color colDer = (seccionActiva == 1) ? CFloat(0.5f, 0.8f, 1.f) : CFloat(0.4f, 0.6f, 0.7f);
    Drawing::texto12(420 + ox, 490, "COMBATES", colDer);

    {
        float startY = 472, step = 88;
        for (int i = 0; i < 4; i++) {
            float y = startY - i * step;
            bool sel = (seccionActiva == 1 && i == gs.opcionSlotSel);
            drawTarjetaSlot(slotsCombate[i], 410 + ox, y - 68, 310, sel, t);
        }
    }

    Drawing::instrucciones(80 + ox, 58,
        "W/S: Navegar    TAB: Seccion    ENTER: Seleccionar    ESC: Volver", t);
    Drawing::botonVolver(60+ox, 565, t);

    // Diálogo encima de todo
    if (dialogoActivo) drawDialogo(gs, t, ox);
}

void CargarPartidaScreen::HandleInput(GameState& gs) {
    // --- Modo edición de nombre ---
    if (dialogoActivo && editandoNombre) {
        if (IsKeyPressed(KEY_ESCAPE)) { editandoNombre = false; return; }
        if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_KP_ENTER)) {
            if (!nuevoNombre.empty()) {
                int idx = gs.opcionCargaSel;
                if (seccionActiva == 0 && slotsTablero[idx].valida) {
                    slotsTablero[idx].nombre = nuevoNombre;
                    SaveSystem::GuardarSlotTablero(slotsTablero[idx]);
                    slotsTablero = SaveSystem::ObtenerSlotsTablero();
                }
            }
            editandoNombre = false;
            dialogoActivo = false;
            return;
        }
        if (IsKeyPressed(KEY_BACKSPACE) && !nuevoNombre.empty()) nuevoNombre.pop_back();
        int ch = GetCharPressed();
        while (ch > 0) {
            if (ch >= 32 && ch < 128 && (int)nuevoNombre.size() < 30)
                nuevoNombre += (char)ch;
            ch = GetCharPressed();
        }
        return;
    }

    // --- Diálogo de acciones ---
    if (dialogoActivo) {
        int numOpciones = (seccionActiva == 0) ? 3 : 2;
        if (IsKeyPressed(KEY_W) || IsKeyPressed(KEY_UP)) {
            if (--dialogoOpcion < 0) dialogoOpcion = numOpciones - 1;
        }
        if (IsKeyPressed(KEY_S) || IsKeyPressed(KEY_DOWN)) {
            if (++dialogoOpcion >= numOpciones) dialogoOpcion = 0;
        }
        if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_KP_ENTER)) {
            // Mapear opción según sección
            int opcion = dialogoOpcion;
            if (seccionActiva == 1 && opcion == 1) opcion = 2; // combate: 1→ELIMINAR
            if (opcion == 0) accionCargar(gs);
            else if (opcion == 1) { // RENOMBRAR (solo tablero)
                int idx = gs.opcionCargaSel;
                nuevoNombre = slotsTablero[idx].valida ? slotsTablero[idx].nombre : "";
                editandoNombre = true;
            }
            else if (opcion == 2) accionEliminar(gs);
        }
        if (IsKeyPressed(KEY_ESCAPE)) dialogoActivo = false;
        return;
    }

    // --- Navegación normal ---
    if (IsKeyPressed(KEY_TAB))
        seccionActiva = 1 - seccionActiva;

    if (IsKeyPressed(KEY_W) || IsKeyPressed(KEY_UP)) {
        if (seccionActiva == 0) { if (--gs.opcionCargaSel < 0) gs.opcionCargaSel = 3; }
        else { if (--gs.opcionSlotSel < 0) gs.opcionSlotSel = 3; }
    }
    if (IsKeyPressed(KEY_S) || IsKeyPressed(KEY_DOWN)) {
        if (seccionActiva == 0) { if (++gs.opcionCargaSel > 3) gs.opcionCargaSel = 0; }
        else { if (++gs.opcionSlotSel > 3) gs.opcionSlotSel = 0; }
    }
    if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_KP_ENTER)) {
        bool valido = (seccionActiva == 0) ? slotsTablero[gs.opcionCargaSel].valida
                                           : slotsCombate[gs.opcionSlotSel].valida;
        if (valido) {
            dialogoActivo = true;
            dialogoOpcion = 0;
            editandoNombre = false;
        }
    }
    if (IsKeyPressed(KEY_ESCAPE)) transicion(gs, MENU);
}

void CargarPartidaScreen::HandleMouse(GameState& gs) {
    float mxv = (float)GetMouseX() * 800.f / GetScreenWidth();
    float myv = 600.f - (float)GetMouseY() * 600.f / GetScreenHeight();
    float ox = gs.cargaOffset;
    bool clicked = IsMouseButtonPressed(MOUSE_BUTTON_LEFT);

    // --- Diálogo abierto: solo interactuar con el diálogo ---
    if (dialogoActivo) {
        if (editandoNombre) return; // texto se maneja por teclado

        float pw = 280, ph = 200;
        float px = 400 + ox - pw / 2, py = 300 - ph / 2;
        int numOpciones = (seccionActiva == 0) ? 3 : 2;

        for (int i = 0; i < numOpciones; i++) {
            float y = py + ph - 75 - i * 45.f;
            if (mxv > px + 20 && mxv < px + pw - 20 && myv > y - 18 && myv < y + 18) {
                dialogoOpcion = i;
                if (clicked) {
                    int opcion = i;
                    if (seccionActiva == 1 && opcion == 1) opcion = 2;
                    if (opcion == 0) accionCargar(gs);
                    else if (opcion == 1) {
                        int idx = gs.opcionCargaSel;
                        nuevoNombre = slotsTablero[idx].valida ? slotsTablero[idx].nombre : "";
                        editandoNombre = true;
                    }
                    else if (opcion == 2) accionEliminar(gs);
                }
            }
        }

        // Click fuera del diálogo = cerrar
        if (clicked && (mxv < px || mxv > px + pw || myv < py || myv > py + ph))
            dialogoActivo = false;
        return;
    }

    // --- Navegación normal ---
    if (clicked && mxv > 40 && mxv < 135 && myv > 543 && myv < 589) {
        transicion(gs, MENU); return;
    }

    float startY = 472, step = 88;
    // Hover sobre slots
    for (int i = 0; i < 4; i++) {
        float y = startY - i * step;
        if (mxv > 65 + ox && mxv < 385 + ox && myv > y - 72 && myv < y) {
            seccionActiva = 0; gs.opcionCargaSel = i;
            if (clicked && slotsTablero[i].valida) {
                dialogoActivo = true;
                dialogoOpcion = 0;
                editandoNombre = false;
            }
        }
    }
    for (int i = 0; i < 4; i++) {
        float y = startY - i * step;
        if (mxv > 410 + ox && mxv < 720 + ox && myv > y - 68 && myv < y) {
            seccionActiva = 1; gs.opcionSlotSel = i;
            if (clicked && slotsCombate[i].valida) {
                dialogoActivo = true;
                dialogoOpcion = 0;
                editandoNombre = false;
            }
        }
    }
}
// ============================================================
// PAUSA — overlay que entra desde arriba sin slash.
// Desde aquí se puede guardar la partida actual.
// ============================================================

void PausaScreen::DrawSlotPicker(GameState& gs, float panX, float panY, float panW, float panH, float t) {
    Drawing::texto18(panX+panW/2-60, panY+panH-30, "ELEGIR SLOT", CFloat(0.4f,1.f,0.4f));
    drawSeparador(panX+20,panY+panH-48,panX+panW-20, 0.3f,0.6f,0.3f);

    for (int i = 0; i < 4; i++) {
        float y = panY+panH-80-i*68.f;
        bool sel = (i == slotCursor);
        float h = 58;

        // Fondo del slot
        rlBegin(RL_QUADS);
        rlColor4f(sel?0.06f:0.03f, sel?0.12f:0.05f, sel?0.06f:0.03f, 0.95f);
        rlVertex2f(panX+20,y-h/2); rlVertex2f(panX+panW-20,y-h/2);
        rlVertex2f(panX+panW-20,y+h/2); rlVertex2f(panX+20,y+h/2);
        rlEnd();

        if (sel) {
            float al=0.5f+0.4f*sinf(t*0.012f);
            rlSetLineWidth(2); rlBegin(RL_LINES);
            rlColor4f(0.3f*al,0.8f*al,0.3f*al,1);
            rlVertex2f(panX+20,y-h/2); rlVertex2f(panX+panW-20,y-h/2);
            rlVertex2f(panX+panW-20,y-h/2); rlVertex2f(panX+panW-20,y+h/2);
            rlVertex2f(panX+panW-20,y+h/2); rlVertex2f(panX+20,y+h/2);
            rlVertex2f(panX+20,y+h/2); rlVertex2f(panX+20,y-h/2);
            rlEnd();
            Drawing::cursorAnimado(panX+28,y,t);
        }

        std::string label = "SLOT " + std::to_string(i+1);
        if (esTablero) {
            auto slots = SaveSystem::ObtenerSlotsTablero();
            if (slots[i].valida) {
                Drawing::texto18(panX+45,y+8, label+": "+slots[i].nombre,
                    sel?CFloat(1,0.95f,0.6f):CFloat(0.7f,0.65f,0.5f));
                Drawing::texto12(panX+45,y-10, slots[i].fecha+" | "+slots[i].bando,
                    CFloat(0.5f,0.5f,0.4f));
            } else {
                Drawing::texto18(panX+45,y, label+" - vacio",
                    sel?CFloat(0.7f,0.9f,0.7f):CFloat(0.5f,0.5f,0.4f));
            }
        } else {
            auto slots = SaveSystem::ObtenerSlots();
            if (slots[i].valida) {
                Drawing::texto18(panX+45,y+8, label+": "+slots[i].nombreP1+" vs "+slots[i].nombreP2,
                    sel?CFloat(0.6f,0.85f,1.f):CFloat(0.5f,0.65f,0.8f));
                std::string difi = slots[i].dificultad==0?"Facil":slots[i].dificultad==1?"Normal":"Dificil";
                Drawing::texto12(panX+45,y-10, slots[i].modoIA?"IA | "+difi:"PvP",
                    CFloat(0.4f,0.5f,0.6f));
            } else {
                Drawing::texto18(panX+45,y, label+" - vacio",
                    sel?CFloat(0.7f,0.9f,0.7f):CFloat(0.5f,0.5f,0.4f));
            }
        }
    }
    // Campo de nombre
    float nbX=panX+40, nbY=panY+42, nbW=panW-80, nbH=28;
    drawPanel(nbX,nbY,nbW,nbH, 0.08f,0.06f,0.02f, 0.04f,0.03f,0.01f);
    if(editandoNombre){
        float al=0.5f+0.4f*sinf(t*0.012f);
        rlSetLineWidth(2); rlBegin(RL_LINES);
        rlColor4f(0.3f*al,0.8f*al,0.3f*al,1);
        rlVertex2f(nbX,nbY); rlVertex2f(nbX+nbW,nbY);
        rlVertex2f(nbX+nbW,nbY); rlVertex2f(nbX+nbW,nbY+nbH);
        rlVertex2f(nbX+nbW,nbY+nbH); rlVertex2f(nbX,nbY+nbH);
        rlVertex2f(nbX,nbY+nbH); rlVertex2f(nbX,nbY);
        rlEnd();
    }
    Drawing::texto12(nbX-70,nbY+8,"NOMBRE:", editandoNombre?CFloat(0.4f,1.f,0.4f):CFloat(0.5f,0.5f,0.4f));
    std::string displayNombre = nombreGuardado;
    if(editandoNombre && ((int)(t*0.006f))%2==0) displayNombre += "_";
    Drawing::texto18(nbX+10,nbY+5,displayNombre, editandoNombre?CFloat(0.7f,1.f,0.7f):CFloat(0.7f,0.7f,0.6f));

    Drawing::instrucciones(panX+20,panY+10,"W/S: Navegar    TAB: Nombre    ENTER: Guardar    ESC: Cancelar",t);
}

void PausaScreen::OnEnter(GameState& gs) {
    gs.opcionPausaSel = 0;
    gs.pausaOffset    = 600.f;
    gs.guardadoOk     = false;
    submenu = 0;
    slotCursor = 0;
    editandoNombre = false;
    nombreGuardado = "";
}

void PausaScreen::Update(GameState& gs) {
    // El panel baja hasta y=0 con inercia
    if(gs.pausaOffset > 0)
        gs.pausaOffset = std::max(0.f, gs.pausaOffset - 18.f);
    // Cuenta regresiva del mensaje de confirmación de guardado
    if(gs.guardadoOk){
        gs.guardadoTimer -= GetFrameTime();
        if(gs.guardadoTimer <= 0.f) gs.guardadoOk=false;
    }
}

void PausaScreen::Draw(GameState& gs) {
    float t=gs.tiempo;
    float oy=gs.pausaOffset;  // desplazamiento vertical (entra desde arriba)

    // Oscurecer lo que hay detrás
    rlBegin(RL_QUADS);
    rlColor4f(0,0,0,0.65f);
    rlVertex2f(0,0); rlVertex2f(800,0); rlVertex2f(800,600); rlVertex2f(0,600);
    rlEnd();

    float panX=190, panY=110-oy, panW=420, panH=400;
    drawPanel(panX,panY,panW,panH, 0.65f,0.5f,0.08f, 0.07f,0.04f,0.02f,0.97f);
    drawTitulo("PAUSA", 400, panY+panH-30, t, 0.9f,0.8f,0.2f);
    drawSeparador(panX+20,panY+panH-52,panX+panW-20, 0.55f,0.4f,0.07f);

    // Mensaje de guardado correcto (aparece 2 segundos)
    if(gs.guardadoOk){
        float al=std::min(gs.guardadoTimer,0.5f)/0.5f;
        drawPanel(230,panY+panH-62,340,22, 0.3f,0.8f,0.3f, 0.1f,0.25f,0.1f,al*0.9f);
        Drawing::texto12(260,panY+panH-58,"Partida guardada correctamente.",CFloat(0.6f,1.f,0.6f,al));
    }

    if (submenu == 1) {
        DrawSlotPicker(gs, panX, panY, panW, panH, t);
        Drawing::botonVolver(60, 565, t);
        return;
    }

    for(int i=0;i<(int)gs.opcionesPausa.size();i++){
        float y=panY+panH-96-i*72.f;  // stepY=72 para 4 opciones
        bool sel=(i==gs.opcionPausaSel);
        bool esGuardar=(gs.opcionesPausa[i]=="GUARDAR PARTIDA");

        if(sel){
            float bx=sinf(t*0.01f)*3, by=sinf(t*0.01f)*2;
            rlSetLineWidth(2); rlBegin(RL_LINES);
            rlColor4f(esGuardar?0.3f:1.f,
                      esGuardar?0.8f:0.1f,
                      esGuardar?0.3f:0.2f, 1);
            rlVertex2f(panX+30+bx,y+20+by); rlVertex2f(panX+panW-30+bx,y+20+by);
            rlVertex2f(panX+panW-30+bx,y+20+by); rlVertex2f(panX+panW-10+bx,y-20+by);
            rlVertex2f(panX+panW-10+bx,y-20+by); rlVertex2f(panX+50+bx,y-20+by);
            rlVertex2f(panX+50+bx,y-20+by); rlVertex2f(panX+30+bx,y+20+by);
            rlEnd();
            Drawing::panelMenu(y);
            float esc=1.1f+sinf(t*0.005f)*0.04f;
            rlPushMatrix(); rlTranslatef(400,y,0); rlScalef(esc,esc,1);
            float tw=(float)MeasureText(gs.opcionesPausa[i].c_str(),18);
            Drawing::texto18(-tw/2.f-2,-7,gs.opcionesPausa[i],CFloat(0,0,0));
            Drawing::texto18(-tw/2.f,  -5,gs.opcionesPausa[i],
                esGuardar?CFloat(0.4f,1.f,0.4f):WHITE);
            rlPopMatrix();
            Drawing::cursorAnimado(panX+40,y,t);
            if(esGuardar)
                Drawing::iconoLibro(panX+70,y,CFloat(0.4f,1.f,0.4f));
            else if(gs.opcionesPausa[i]=="OPCIONES")
                Drawing::iconoOpciones(panX+70,y,WHITE);
            else if(gs.opcionesPausa[i]=="MENU PRINCIPAL")
                Drawing::iconoEspada(panX+70,y,WHITE);
        } else {
            Color tc = esGuardar ? CFloat(0.3f,0.7f,0.3f) : CFloat(0.7f,0.7f,0.7f);
            float tw=(float)MeasureText(gs.opcionesPausa[i].c_str(),16);
            Drawing::texto18(400-tw/2.f,y-5,gs.opcionesPausa[i],tc);
            if(esGuardar)
                Drawing::iconoLibro(panX+70,y,CFloat(0.3f,0.6f,0.3f));
            else if(gs.opcionesPausa[i]=="OPCIONES")
                Drawing::iconoOpciones(panX+70,y,CFloat(0.6f,0.6f,0.6f));
            else if(gs.opcionesPausa[i]=="MENU PRINCIPAL")
                Drawing::iconoEspada(panX+70,y,CFloat(0.6f,0.6f,0.6f));
        }
    }
    Drawing::instrucciones(230,panY-18,"W/S: Navegar    ENTER: Confirmar    ESC: Continuar",t);
    Drawing::botonVolver(60, 565, t);
}

void PausaScreen::HandleInput(GameState& gs) {
    // --- Submenu: elegir slot para guardar ---
    if (submenu == 1) {
        // TAB alterna edición de nombre
        if (IsKeyPressed(KEY_TAB)) {
            editandoNombre = !editandoNombre;
            return;
        }
        // Si estamos editando el nombre, capturar texto
        if (editandoNombre) {
            int ch = GetCharPressed();
            while (ch > 0) {
                if (ch >= 32 && ch < 127 && (int)nombreGuardado.size() < 30)
                    nombreGuardado += (char)ch;
                ch = GetCharPressed();
            }
            if (IsKeyPressed(KEY_BACKSPACE) && !nombreGuardado.empty())
                nombreGuardado.pop_back();
            if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_KP_ENTER))
                editandoNombre = false;
            if (IsKeyPressed(KEY_ESCAPE))
                editandoNombre = false;
            return;
        }
        if (IsKeyPressed(KEY_W) || IsKeyPressed(KEY_UP)) {
            if (--slotCursor < 0) slotCursor = 3;
        }
        if (IsKeyPressed(KEY_S) || IsKeyPressed(KEY_DOWN)) {
            if (++slotCursor > 3) slotCursor = 0;
        }
        if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_KP_ENTER)) {
            bool ok = false;
            if (esTablero && gs.tableroActivo) {
                // Si hay combate 3D activo, guardar su estado en el controlador
                if (gs.combateActivo)
                    gs.tableroActivo->setCombateState(
                        gs.combateActivo->GetP1Vida(), gs.combateActivo->GetP1Pos().x, gs.combateActivo->GetP1Pos().y,
                        gs.combateActivo->GetP2Vida(), gs.combateActivo->GetP2Pos().x, gs.combateActivo->GetP2Pos().y);
                else
                    gs.tableroActivo->clearCombateState();

                auto slots = SaveSystem::ObtenerSlotsTablero();
                PartidaGuardada p = slots[slotCursor];
                if (gs.partidaActualIdx >= 0 && gs.partidaActualIdx < (int)gs.partidas.size())
                    p = gs.partidas[gs.partidaActualIdx];
                p.slot = slotCursor;
                if (!nombreGuardado.empty()) p.nombre = nombreGuardado;
                p.boardState = gs.tableroActivo->GuardarEstado();
                p.valida = true;
                time_t now = time(nullptr);
                char buf[64]; strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M", localtime(&now));
                p.fecha = buf;
                SaveSystem::GuardarSlotTablero(p);
                ok = true;
            } else if (!esTablero && gs.combateActivo) {
                ok = gs.combateActivo->GuardarEstado(gs.combateModoIA, gs.combateDificultad, slotCursor);
            }
            if (ok) { gs.guardadoOk = true; gs.guardadoTimer = 2.f; }
            submenu = 0;
        }
        if (IsKeyPressed(KEY_ESCAPE)) { editandoNombre = false; submenu = 0; }
        return;
    }

    // --- Menu principal de pausa ---
    if (IsKeyPressed(KEY_W) || IsKeyPressed(KEY_UP)) {
        if (--gs.opcionPausaSel < 0)
            gs.opcionPausaSel = (int)gs.opcionesPausa.size() - 1;
    }
    if (IsKeyPressed(KEY_S) || IsKeyPressed(KEY_DOWN)) {
        if (++gs.opcionPausaSel >= (int)gs.opcionesPausa.size())
            gs.opcionPausaSel = 0;
    }
    if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_KP_ENTER)) {
        const std::string& opt = gs.opcionesPausa[gs.opcionPausaSel];
        if (opt == "CONTINUAR") {
            gs.estadoActual = gs.estadoAnterior;
        } else if (opt == "GUARDAR PARTIDA") {
            esTablero = (gs.estadoAnterior == TABLERO);
            slotCursor = 0;
            editandoNombre = false;
            // Pre-rellenar nombre con el de la partida actual
            if (esTablero && gs.partidaActualIdx >= 0 && gs.partidaActualIdx < (int)gs.partidas.size())
                nombreGuardado = gs.partidas[gs.partidaActualIdx].nombre;
            else
                nombreGuardado = gs.nombreGuardado.empty() ? "Partida" : gs.nombreGuardado;
            submenu = 1;
        } else if (opt == "OPCIONES") {
            gs.estadoActual = gs.estadoAnterior;
            transicion(gs, OPCIONES);
        } else if (opt == "MENU PRINCIPAL") {
            gs.estadoActual = gs.estadoAnterior;
            transicion(gs, MENU);
        }
    }
    // ESC = continuar directamente
    if (IsKeyPressed(KEY_ESCAPE)) gs.estadoActual = gs.estadoAnterior;
}

void PausaScreen::HandleMouse(GameState& gs) {
    float mxv=(float)GetMouseX()*800.f/GetScreenWidth(), myv=600.f-(float)GetMouseY()*600.f/GetScreenHeight();
    bool clicked=IsMouseButtonPressed(MOUSE_BUTTON_LEFT);
    bool moved = (GetMouseDelta().x != 0 || GetMouseDelta().y != 0);
    // Back button
    if(clicked && mxv>40&&mxv<135&&myv>543&&myv<589){
        if (submenu == 1) { submenu = 0; return; }
        gs.estadoActual=gs.estadoAnterior; return;
    }
    float oy=gs.pausaOffset, panY=110-oy, panW=420, panH=400, panX=190;

    // --- Submenu: slot picker con ratón ---
    if (submenu == 1) {
        // Click en campo nombre
        float nbX=panX+40, nbY=panY+42, nbW=panW-80, nbH=28;
        if(clicked && mxv>nbX&&mxv<nbX+nbW&&myv>nbY&&myv<nbY+nbH){
            editandoNombre = true;
            return;
        }
        for (int i = 0; i < 4; i++) {
            float y = panY+panH-80-i*68.f;
            float h = 58;
            if (mxv>panX+20&&mxv<panX+panW-20&&myv>y-h/2&&myv<y+h/2) {
                if (moved) slotCursor = i;
                if (clicked) {
                    slotCursor = i;
                    bool ok = false;
                    if (esTablero && gs.tableroActivo) {
                        if (gs.combateActivo)
                            gs.tableroActivo->setCombateState(
                                gs.combateActivo->GetP1Vida(), gs.combateActivo->GetP1Pos().x, gs.combateActivo->GetP1Pos().y,
                                gs.combateActivo->GetP2Vida(), gs.combateActivo->GetP2Pos().x, gs.combateActivo->GetP2Pos().y);
                        else
                            gs.tableroActivo->clearCombateState();

                        auto slots = SaveSystem::ObtenerSlotsTablero();
                        PartidaGuardada p = slots[slotCursor];
                        if (gs.partidaActualIdx >= 0 && gs.partidaActualIdx < (int)gs.partidas.size())
                            p = gs.partidas[gs.partidaActualIdx];
                        p.slot = slotCursor;
                        if (!nombreGuardado.empty()) p.nombre = nombreGuardado;
                        p.boardState = gs.tableroActivo->GuardarEstado();
                        p.valida = true;
                        time_t now = time(nullptr);
                        char buf[64]; strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M", localtime(&now));
                        p.fecha = buf;
                        SaveSystem::GuardarSlotTablero(p);
                        ok = true;
                    } else if (!esTablero && gs.combateActivo) {
                        ok = gs.combateActivo->GuardarEstado(gs.combateModoIA, gs.combateDificultad, slotCursor);
                    }
                    if (ok) { gs.guardadoOk = true; gs.guardadoTimer = 2.f; }
                    submenu = 0;
                }
            }
        }
        return;
    }

    // --- Menu principal de pausa ---
    for(int i=0;i<(int)gs.opcionesPausa.size();i++){
        float y=panY+panH-96-i*72.f;
        if(mxv>210&&mxv<590&&myv>y-22&&myv<y+22){
            if (moved) gs.opcionPausaSel=i;
            if(clicked){
                const std::string& opt=gs.opcionesPausa[i];
                if(opt=="CONTINUAR") gs.estadoActual=gs.estadoAnterior;
                else if(opt=="GUARDAR PARTIDA"){
                    esTablero = (gs.estadoAnterior == TABLERO);
                    slotCursor = 0;
                    editandoNombre = false;
                    if (esTablero && gs.partidaActualIdx >= 0 && gs.partidaActualIdx < (int)gs.partidas.size())
                        nombreGuardado = gs.partidas[gs.partidaActualIdx].nombre;
                    else
                        nombreGuardado = gs.nombreGuardado.empty() ? "Partida" : gs.nombreGuardado;
                    submenu = 1;
                }
                else if(opt=="OPCIONES"){
                    gs.estadoActual=gs.estadoAnterior;
                    transicion(gs,OPCIONES);
                }
                else if(opt=="MENU PRINCIPAL") { gs.estadoActual=gs.estadoAnterior; transicion(gs,MENU); }
            }
        }
    }
}

}