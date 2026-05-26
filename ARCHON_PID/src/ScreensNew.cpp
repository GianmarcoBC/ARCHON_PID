#include "Screens.h"
#include "Drawing.h"
#include "Background.h"
#include "Particles.h"
#include "SaveSystem.h"
#include "rlgl.h"

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
    Drawing::instrucciones(235+ox,50,"W/S: Seleccionar    ENTER: Confirmar    ESC: Volver",t);
}

void ModoJuegoScreen::HandleInput(GameState& gs) {
    if(IsKeyPressed(KEY_W)||IsKeyPressed(KEY_UP))
        { if(--gs.opcionModoSel<0) gs.opcionModoSel=1; }
    if(IsKeyPressed(KEY_S)||IsKeyPressed(KEY_DOWN))
        { if(++gs.opcionModoSel>1) gs.opcionModoSel=0; }
    if(IsKeyPressed(KEY_ENTER)) {
        gs.modoActual = (gs.opcionModoSel==0) ? MODO_COMBATE : MODO_COMPLETO;
        transicion(gs, SELECCION_MODO);
    }
    if(IsKeyPressed(KEY_ESCAPE)) transicion(gs, MENU);
}

void ModoJuegoScreen::HandleMouse(GameState& gs) {
    float mxv=(float)GetMouseX()*800.f/GetScreenWidth(), myv=600.f-(float)GetMouseY()*600.f/GetScreenHeight();
    for(int i=0;i<2;i++){
        float oy=430-i*100.f;
        if(mxv>145&&mxv<695&&myv>oy-38&&myv<oy+38){
            gs.opcionModoSel=i;
            gs.modoActual=(i==0)?MODO_COMBATE:MODO_COMPLETO;
            transicion(gs,SELECCION_MODO);
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
        ? "COMBATE — MODO DE JUEGO"
        : "JUEGO COMPLETO — MODO DE JUEGO";
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
    Drawing::instrucciones(235+ox,50,"W/S: Seleccionar    ENTER: Confirmar    ESC: Volver",t);
}

void SeleccionModoScreen::HandleInput(GameState& gs) {
    if(IsKeyPressed(KEY_W)||IsKeyPressed(KEY_UP))
        { if(--gs.opcionSelModoSel<0) gs.opcionSelModoSel=1; }
    if(IsKeyPressed(KEY_S)||IsKeyPressed(KEY_DOWN))
        { if(++gs.opcionSelModoSel>1) gs.opcionSelModoSel=0; }
    if(IsKeyPressed(KEY_ENTER)){
        if(gs.modoActual==MODO_COMPLETO)
            transicion(gs, CONFIG_JUEGO_COMPLETO);
        else
            transicion(gs, CUCHAU_COMBATE);
    }
    if(IsKeyPressed(KEY_ESCAPE)) transicion(gs, MODO_JUEGO);
}

void SeleccionModoScreen::HandleMouse(GameState& gs) {
    float mxv=(float)GetMouseX()*800.f/GetScreenWidth(),myv=600.f-(float)GetMouseY()*600.f/GetScreenHeight();
    for(int i=0;i<2;i++){
        float oy=390-i*110.f;
        if(mxv>145&&mxv<695&&myv>oy-35&&myv<oy+35){
            gs.opcionSelModoSel=i;
            if(gs.modoActual==MODO_COMPLETO) transicion(gs,CONFIG_JUEGO_COMPLETO);
            else transicion(gs,CUCHAU_COMBATE);
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
        bool sel=(i==gs.opcionBandoSel), foco=(gs.configJCFoco==0);
        // Fondo de fila
        rlBegin(RL_QUADS);
        rlColor4f(cols[i][0]*(sel?0.22f:0.07f),
                  cols[i][1]*(sel?0.22f:0.07f),
                  cols[i][2]*(sel?0.22f:0.07f),0.9f);
        rlVertex2f(panX+12,by-28); rlVertex2f(panX+panW-12,by-28);
        rlVertex2f(panX+panW-12,by+28); rlVertex2f(panX+12,by+28);
        rlEnd();
        if(sel&&foco){
            float al=0.5f+0.4f*sinf(t*0.012f);
            rlSetLineWidth(2); rlBegin(RL_LINES);
            rlColor4f(cols[i][0]*al,cols[i][1]*al,cols[i][2]*al,1);
            rlVertex2f(panX+12,by-28); rlVertex2f(panX+panW-12,by-28);
            rlVertex2f(panX+panW-12,by-28); rlVertex2f(panX+panW-12,by+28);
            rlVertex2f(panX+panW-12,by+28); rlVertex2f(panX+12,by+28);
            rlVertex2f(panX+12,by+28); rlVertex2f(panX+12,by-28);
            rlEnd();
        }
        // Icono de bando
        if(i==0)      Drawing::iconoEspada(panX+35,by, sel?CFloat(cols[i][0],cols[i][1],cols[i][2]):CFloat(0.5f,0.5f,0.5f));
        else if(i==1) Drawing::iconoOpciones(panX+35,by, sel?CFloat(cols[i][0],cols[i][1],cols[i][2]):CFloat(0.5f,0.5f,0.5f));
        else          Drawing::iconoLibro(panX+35,by, sel?CFloat(cols[i][0],cols[i][1],cols[i][2]):CFloat(0.5f,0.5f,0.5f));

        float escL=sel&&foco?1.1f+sinf(t*0.008f)*0.03f:1.f;
        rlPushMatrix(); rlTranslatef(panX+65,by,0); rlScalef(escL,escL,1);
        Drawing::texto18(0,-7,labels[i], sel?CFloat(cols[i][0],cols[i][1],cols[i][2]):CFloat(0.55f,0.5f,0.4f));
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
        esIA?CFloat(0.5f,0.7f,1.f):CFloat(0.35f,0.35f,0.4f));
    drawSeparador(panX+10,panY+panH-38,panX+panW-10, br,bg,bb);

    if(!esIA){
        Drawing::texto12(panX+20,panY+panH/2,"Solo para modo IA.",CFloat(0.4f,0.35f,0.3f));
        return;
    }
    float cols[4][3]={{0.3f,0.8f,0.3f},{0.8f,0.8f,0.2f},{0.9f,0.4f,0.1f},{0.9f,0.1f,0.2f}};
    const char* noms[4]={"FACIL","NORMAL","DIFICIL","MAESTRO"};
    for(int i=0;i<4;i++){
        float by=panY+panH-72-i*62.f;
        bool sel=(i==gs.opcionDifiSel),foco=(gs.configJCFoco==1);
        rlBegin(RL_QUADS);
        rlColor4f(cols[i][0]*(sel?0.18f:0.06f),
                  cols[i][1]*(sel?0.18f:0.06f),
                  cols[i][2]*(sel?0.18f:0.06f),0.9f);
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
        }
        // Estrellas como indicador de nivel
        for(int s=0;s<=i;s++)
            Drawing::circulo(panX+panW-20-s*14.f,by,4,8,
                sel?CFloat(cols[i][0],cols[i][1],cols[i][2]):CFloat(cols[i][0]*0.45f,cols[i][1]*0.45f,cols[i][2]*0.45f));
        float escL=sel&&foco?1.1f+sinf(t*0.008f)*0.03f:1.f;
        rlPushMatrix(); rlTranslatef(panX+45,by,0); rlScalef(escL,escL,1);
        Drawing::texto18(0,-7,noms[i], sel?CFloat(cols[i][0],cols[i][1],cols[i][2]):CFloat(0.5f,0.45f,0.4f));
        rlPopMatrix();
        if(sel&&foco) Drawing::cursorAnimado(panX+20,by,t);
    }
}

void ConfigJuegoCompletoScreen::Draw(GameState& gs) {
    float t=gs.tiempo, ox=gs.configJCOffset;
    Background::library(gs);
    drawPanel(45+ox,95,710,370, 0.65f,0.5f,0.08f, 0.05f,0.03f,0.01f,0.90f);
    drawTitulo("CONFIGURACION DE PARTIDA", 400+ox, 488, t, 0.85f,0.75f,0.15f);
    drawSeparador(60+ox,465,740+ox, 0.55f,0.4f,0.07f);
    drawPanelBando(gs,ox);
    drawPanelDificultad(gs,ox);
    // Indicador de qué panel está activo
    const char* focoTxt = (gs.configJCFoco==0)?"[ BANDO ] > Dificultad":"Bando > [ DIFICULTAD ]";
    Drawing::texto12(300+ox,120, focoTxt, CFloat(0.7f,0.6f,0.2f));
    // Botón confirmar
    drawPanel(280+ox,58,240,36, 0.7f,0.1f,0.05f, 0.12f,0.04f,0.02f);
    Drawing::texto18(295+ox,64,"CONFIRMAR  [ENTER]", CFloat(1,0.9f,0.5f));
    Drawing::instrucciones(100+ox,40,"A/D: Panel    W/S: Seleccionar    ENTER: Confirmar    ESC: Volver",t);
}

void ConfigJuegoCompletoScreen::HandleInput(GameState& gs) {
    bool esIA=(gs.opcionSelModoSel==1);
    if(IsKeyPressed(KEY_A)||IsKeyPressed(KEY_LEFT))  { if(esIA) gs.configJCFoco=0; }
    if(IsKeyPressed(KEY_D)||IsKeyPressed(KEY_RIGHT)) { if(esIA) gs.configJCFoco=1; }
    if(IsKeyPressed(KEY_W)||IsKeyPressed(KEY_UP)){
        if(gs.configJCFoco==0){ if(--gs.opcionBandoSel<0) gs.opcionBandoSel=2; }
        else if(esIA){ if(--gs.opcionDifiSel<0) gs.opcionDifiSel=3; }
    }
    if(IsKeyPressed(KEY_S)||IsKeyPressed(KEY_DOWN)){
        if(gs.configJCFoco==0){ if(++gs.opcionBandoSel>2) gs.opcionBandoSel=0; }
        else if(esIA){ if(++gs.opcionDifiSel>3) gs.opcionDifiSel=0; }
    }
    if(IsKeyPressed(KEY_ENTER)){
        gs.bandoSel=(Bando)(gs.opcionBandoSel+1);
        gs.dificultadSel=(Dificultad)gs.opcionDifiSel;
        transicion(gs, gs.opcionSelModoSel==0?JUGAR_PVP:JUGAR_IA);
    }
    if(IsKeyPressed(KEY_ESCAPE)) transicion(gs,SELECCION_MODO);
}

void ConfigJuegoCompletoScreen::HandleMouse(GameState& gs) {
    float mxv=(float)GetMouseX()*800.f/GetScreenWidth(),myv=600.f-(float)GetMouseY()*600.f/GetScreenHeight(),ox=gs.configJCOffset;
    if(mxv>60+ox&&mxv<360+ox&&myv>110&&myv<440){
        gs.configJCFoco=0;
        for(int i=0;i<3;i++){float by=440-80-i*82.f;if(myv>by-28&&myv<by+28) gs.opcionBandoSel=i;}
    }
    if(mxv>440+ox&&mxv<740+ox&&myv>110&&myv<440){
        gs.configJCFoco=1;
        for(int i=0;i<4;i++){float by=440-72-i*62.f;if(myv>by-20&&myv<by+20) gs.opcionDifiSel=i;}
    }
    if(mxv>280+ox&&mxv<520+ox&&myv>58&&myv<94){
        gs.bandoSel=(Bando)(gs.opcionBandoSel+1);
        gs.dificultadSel=(Dificultad)gs.opcionDifiSel;
        transicion(gs,gs.opcionSelModoSel==0?JUGAR_PVP:JUGAR_IA);
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

    // Los dos sprites del equipo flotando
    float fl = activo ? sinf(t*0.012f)*5 : 0;
    Drawing::dibujarSprite(eq.sprite1, cx-30, cy+20+fl,   0.9f, t);
    Drawing::dibujarSprite(eq.sprite2, cx+30, cy+20+fl*0.8f, 0.9f, t);

    // Nombre del equipo
    Drawing::texto18(cx-(float)MeasureText(eq.nombre.c_str(),18)/2.f,
        y+h-28, eq.nombre,
        activo?CFloat(eq.colorR,eq.colorG,eq.colorB):CFloat(0.65f,0.6f,0.5f));

    // Stats solo si está activo
    if(activo){
        float sx=x+10, sw=w-20, sy=y+12;
        Drawing::texto12(sx,sy,"ATQ",CFloat(0.75f,0.65f,0.5f));
        drawBarraStat(sx+26,sy+2,sw-26,eq.ataque,   CFloat(0.9f,0.2f,0.1f));
        Drawing::texto12(sx,sy+14,"DEF",CFloat(0.75f,0.65f,0.5f));
        drawBarraStat(sx+26,sy+16,sw-26,eq.defensa,  CFloat(0.2f,0.5f,0.9f));
        Drawing::texto12(sx,sy+28,"VEL",CFloat(0.75f,0.65f,0.5f));
        drawBarraStat(sx+26,sy+30,sw-26,eq.velocidad,CFloat(0.2f,0.9f,0.3f));
        Drawing::texto12(sx,sy+42,"MAG",CFloat(0.75f,0.65f,0.5f));
        drawBarraStat(sx+26,sy+44,sw-26,eq.magia,    CFloat(0.8f,0.3f,0.9f));
    }
}

void SeleccionEquipoScreen::Draw(GameState& gs) {
    float t=gs.tiempo, ox=gs.selEquipoOffset;
    Background::library(gs);

    drawPanel(35+ox,80,730,440, 0.65f,0.5f,0.08f, 0.05f,0.03f,0.01f,0.88f);
    drawTitulo("SELECCION DE EQUIPO", 400+ox, 502, t, 0.85f,0.75f,0.15f);

    // Indicador de quién elige
    bool esP1=gs.seleccionandoJ1;
    std::string quienElige = esP1 ? "JUGADOR 1 — Elige tu equipo" : "JUGADOR 2 — Elige tu equipo";
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
    std::string btnTxt = (esP1&&esPvP) ? "SIGUIENTE  [ENTER]" : "LUCHAR  [ENTER]";
    drawPanel(275+ox,56,250,36, 0.7f,0.1f,0.05f, 0.12f,0.04f,0.02f);
    Drawing::texto18(290+ox,62, btnTxt, CFloat(1,0.9f,0.5f));

    Drawing::instrucciones(125+ox,40,
        "A/D: Elegir equipo    ENTER: Confirmar    ESC: Volver", t);
}

void SeleccionEquipoScreen::HandleInput(GameState& gs) {
    bool esPvP=(gs.opcionSelModoSel==0);
    // A/D cambia el equipo del jugador que está eligiendo
    auto moverSel = [&](int dir){
        if(gs.seleccionandoJ1){
            gs.equipoSel1=(gs.equipoSel1+dir+2)%2;
        } else {
            gs.equipoSel2=(gs.equipoSel2+dir+2)%2;
        }
    };
    if(IsKeyPressed(KEY_A)||IsKeyPressed(KEY_LEFT))  moverSel(-1);
    if(IsKeyPressed(KEY_D)||IsKeyPressed(KEY_RIGHT)) moverSel(1);

    if(IsKeyPressed(KEY_ENTER)){
        if(gs.seleccionandoJ1 && esPvP){
            gs.seleccionandoJ1=false;  // ahora elige J2
        } else {
            if(gs.opcionSelModoSel==0) transicion(gs,JUGAR_PVP);
            else                        transicion(gs,CONFIG_DIFICULTAD);
        }
    }
    if(IsKeyPressed(KEY_ESCAPE)){
        if(!gs.seleccionandoJ1) gs.seleccionandoJ1=true;  // J1 vuelve a elegir
        else transicion(gs,SELECCION_MODO);
    }
}

void SeleccionEquipoScreen::HandleMouse(GameState& gs) {
    if(!IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) return;
    float mxv=(float)GetMouseX()*800.f/GetScreenWidth(),myv=600.f-(float)GetMouseY()*600.f/GetScreenHeight(),ox=gs.selEquipoOffset;
    float cx1=220+ox, cx2=580+ox, cy=285, hw=92, hh=135;
    // Clic en tarjeta izquierda (equipo 0)
    if(mxv>cx1-hw&&mxv<cx1+hw&&myv>cy-hh&&myv<cy+hh){
        if(gs.seleccionandoJ1) gs.equipoSel1=0; else gs.equipoSel2=0;
    }
    // Clic en tarjeta derecha (equipo 1)
    if(mxv>cx2-hw&&mxv<cx2+hw&&myv>cy-hh&&myv<cy+hh){
        if(gs.seleccionandoJ1) gs.equipoSel1=1; else gs.equipoSel2=1;
    }
    // Botón confirmar
    if(mxv>275+ox&&mxv<525+ox&&myv>56&&myv<92){
        bool esPvP=(gs.opcionSelModoSel==0);
        if(gs.seleccionandoJ1&&esPvP){ gs.seleccionandoJ1=false; }
        else{ if(gs.opcionSelModoSel==0) transicion(gs,JUGAR_PVP); else transicion(gs,CONFIG_DIFICULTAD); }
    }
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

    float cols[4][3]={{0.3f,0.8f,0.3f},{0.8f,0.8f,0.2f},{0.9f,0.4f,0.1f},{0.9f,0.1f,0.2f}};
    const char* noms[4]={"FACIL","NORMAL","DIFICIL","MAESTRO"};
    const char* descs[4]={"Comete errores con frecuencia.",
                           "Un reto equilibrado.",
                           "Planifica con antelacion.",
                           "Sin misericordia. Solo para expertos."};
    for(int i=0;i<4;i++){
        float y=430-i*70.f;
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
    Drawing::texto18(295+ox,147,"LUCHAR  [ENTER]",CFloat(1,0.9f,0.5f));
    Drawing::instrucciones(175+ox,50,"W/S: Dificultad    ENTER: Comenzar    ESC: Volver",t);
}

void ConfigDificultadScreen::HandleInput(GameState& gs) {
    if(IsKeyPressed(KEY_W)||IsKeyPressed(KEY_UP))
        { if(--gs.opcionDifiCombateSel<0) gs.opcionDifiCombateSel=3; }
    if(IsKeyPressed(KEY_S)||IsKeyPressed(KEY_DOWN))
        { if(++gs.opcionDifiCombateSel>3) gs.opcionDifiCombateSel=0; }
    if(IsKeyPressed(KEY_ENTER)){
        gs.dificultadSel=(Dificultad)gs.opcionDifiCombateSel;
        transicion(gs,JUGAR_IA);
    }
    if(IsKeyPressed(KEY_ESCAPE)) transicion(gs,SELECCION_EQUIPO);
}

void ConfigDificultadScreen::HandleMouse(GameState& gs) {
    float mxv=(float)GetMouseX()*800.f/GetScreenWidth(),myv=600.f-(float)GetMouseY()*600.f/GetScreenHeight(),ox=gs.configDifiOffset;
    for(int i=0;i<4;i++){
        float y=430-i*70.f;
        if(mxv>170+ox&&mxv<630+ox&&myv>y-24&&myv<y+24) gs.opcionDifiCombateSel=i;
    }
    if(mxv>280+ox&&mxv<520+ox&&myv>141&&myv<177){
        gs.dificultadSel=(Dificultad)gs.opcionDifiCombateSel;
        transicion(gs,JUGAR_IA);
    }
}

// ============================================================
// CARGAR PARTIDA
// Lee del disco al entrar y muestra las partidas guardadas.
// ============================================================
void CargarPartidaScreen::OnEnter(GameState& gs) {
    gs.opcionCargaSel=0;
    // Recargar del fichero cada vez que se entra en esta pantalla
    SaveSystem::cargarTodas(gs);
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

    Drawing::texto18(x+12,y+h-24,p.nombre,
        sel?CFloat(1,0.9f,0.5f):CFloat(0.75f,0.7f,0.55f));
    Drawing::texto12(x+12,y+h-40,p.modo+" | "+p.bando+" | Turno "+std::to_string(p.turno),
        CFloat(0.6f,0.55f,0.4f));
    Drawing::texto12(x+12,y+h-52,p.fecha,CFloat(0.45f,0.4f,0.3f));

    // Barra de progreso
    float bw=w-160;
    rlBegin(RL_QUADS);
    rlColor4f(0.08f,0.06f,0.02f,1);
    rlVertex2f(x+12,y+10); rlVertex2f(x+12+bw,y+10);
    rlVertex2f(x+12+bw,y+18); rlVertex2f(x+12,y+18);
    rlEnd();
    rlBegin(RL_QUADS);
    rlColor4f(sel?0.8f:0.5f,sel?0.6f:0.45f,0.1f,0.9f);
    rlVertex2f(x+12,y+10); rlVertex2f(x+12+bw*p.progreso,y+10);
    rlVertex2f(x+12+bw*p.progreso,y+18); rlVertex2f(x+12,y+18);
    rlEnd();
    Drawing::texto12(x+12+bw+6,y+9,
        std::to_string((int)(p.progreso*100))+"%",CFloat(0.6f,0.55f,0.35f));

    if(sel) Drawing::cursorAnimado(x+5,y+h/2,t);
}

void CargarPartidaScreen::Draw(GameState& gs) {
    float t=gs.tiempo, ox=gs.cargaOffset;
    Background::library(gs);
    drawPanel(50+ox,88,700,428, 0.65f,0.5f,0.08f, 0.05f,0.03f,0.01f,0.90f);
    drawTitulo("CARGAR PARTIDA", 400+ox, 530, t, 0.85f,0.75f,0.15f);
    drawSeparador(65+ox,505,735+ox, 0.55f,0.4f,0.07f);

    if(gs.partidas.empty()){
        Drawing::texto18(230+ox,290,"No hay partidas guardadas.",CFloat(0.5f,0.45f,0.35f));
    } else {
        float startY=478, step=92;
        for(int i=0;i<(int)gs.partidas.size();i++){
            float y=startY-i*step;
            drawTarjetaPartida(gs.partidas[i],65+ox,y-80,670,(i==gs.opcionCargaSel),t);
        }
    }

    drawPanel(78+ox,100,200,30, 0.7f,0.1f,0.05f, 0.12f,0.04f,0.02f);
    Drawing::texto12(96+ox,108,"CARGAR  [ENTER]",CFloat(1,0.9f,0.5f));
    drawPanel(518+ox,100,200,30, 0.35f,0.1f,0.1f, 0.1f,0.04f,0.04f);
    Drawing::texto12(538+ox,108,"BORRAR  [DEL]",  CFloat(0.85f,0.5f,0.5f));

    Drawing::instrucciones(155+ox,58,"W/S: Navegar    ENTER: Cargar    DEL: Borrar    ESC: Volver",t);
}

void CargarPartidaScreen::HandleInput(GameState& gs) {
    if(IsKeyPressed(KEY_W)||IsKeyPressed(KEY_UP))
        { if(--gs.opcionCargaSel<0) gs.opcionCargaSel=(int)gs.partidas.size()-1; }
    if(IsKeyPressed(KEY_S)||IsKeyPressed(KEY_DOWN))
        { if(++gs.opcionCargaSel>=(int)gs.partidas.size()) gs.opcionCargaSel=0; }
    if(IsKeyPressed(KEY_ENTER)&&!gs.partidas.empty()){
        SaveSystem::restaurar(gs,gs.opcionCargaSel);
        transicion(gs, gs.modoActual==MODO_COMPLETO ? JUGAR_IA : JUGAR_IA);
    }
    if(IsKeyPressed(KEY_DELETE)&&!gs.partidas.empty()){
        SaveSystem::borrar(gs,gs.opcionCargaSel);
        if(gs.opcionCargaSel>=(int)gs.partidas.size())
            gs.opcionCargaSel=std::max(0,(int)gs.partidas.size()-1);
    }
    if(IsKeyPressed(KEY_ESCAPE)) transicion(gs,MENU);
}

void CargarPartidaScreen::HandleMouse(GameState& gs) {
    if(!IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) return;
    float mxv=(float)GetMouseX()*800.f/GetScreenWidth(),myv=600.f-(float)GetMouseY()*600.f/GetScreenHeight(),ox=gs.cargaOffset;
    float startY=478,step=92;
    for(int i=0;i<(int)gs.partidas.size();i++){
        float y=startY-i*step;
        if(mxv>65+ox&&mxv<735+ox&&myv>y-80&&myv<y)
            gs.opcionCargaSel=i;
    }
    if(mxv>78+ox&&mxv<278+ox&&myv>100&&myv<130&&!gs.partidas.empty()){
        SaveSystem::restaurar(gs,gs.opcionCargaSel);
        transicion(gs,JUGAR_IA);
    }
    if(mxv>518+ox&&mxv<718+ox&&myv>100&&myv<130&&!gs.partidas.empty()){
        SaveSystem::borrar(gs,gs.opcionCargaSel);
        if(gs.opcionCargaSel>=(int)gs.partidas.size())
            gs.opcionCargaSel=std::max(0,(int)gs.partidas.size()-1);
    }
}

// ============================================================
// PAUSA — overlay que entra desde arriba sin slash.
// Desde aquí se puede guardar la partida actual.
// ============================================================
void PausaScreen::OnEnter(GameState& gs) {
    gs.opcionPausaSel = 0;
    gs.pausaOffset    = 600.f;
    gs.guardadoOk     = false;
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

    float panX=190, panY=110-oy, panW=420, panH=360;
    drawPanel(panX,panY,panW,panH, 0.65f,0.5f,0.08f, 0.07f,0.04f,0.02f,0.97f);
    drawTitulo("PAUSA", 400, panY+panH-30, t, 0.9f,0.8f,0.2f);
    drawSeparador(panX+20,panY+panH-52,panX+panW-20, 0.55f,0.4f,0.07f);

    // Mensaje de guardado correcto (aparece 2 segundos)
    if(gs.guardadoOk){
        float al=std::min(gs.guardadoTimer,0.5f)/0.5f;
        drawPanel(230,panY+panH-62,340,22, 0.3f,0.8f,0.3f, 0.1f,0.25f,0.1f,al*0.9f);
        Drawing::texto12(260,panY+panH-58,"Partida guardada correctamente.",CFloat(0.6f,1.f,0.6f,al));
    }

    for(int i=0;i<(int)gs.opcionesPausa.size();i++){
        float y=panY+panH-96-i*68.f;
        bool sel=(i==gs.opcionPausaSel);

        // "GUARDAR PARTIDA" tiene color especial para destacarlo
        bool esGuardar=(gs.opcionesPausa[i]=="GUARDAR PARTIDA");

        if(sel){
            float bx=sinf(t*0.01f)*3, by=sinf(t*0.01f)*2;
            rlSetLineWidth(2); rlBegin(RL_LINES);
            rlColor4f(esGuardar?0.3f:1.f, esGuardar?0.8f:0.1f, esGuardar?0.3f:0.2f, 1);
            rlVertex2f(panX+30+bx,y+22+by); rlVertex2f(panX+panW-30+bx,y+22+by);
            rlVertex2f(panX+panW-30+bx,y+22+by); rlVertex2f(panX+panW-10+bx,y-22+by);
            rlVertex2f(panX+panW-10+bx,y-22+by); rlVertex2f(panX+50+bx,y-22+by);
            rlVertex2f(panX+50+bx,y-22+by); rlVertex2f(panX+30+bx,y+22+by);
            rlEnd();
            Drawing::panelMenu(y);
            float esc=1.15f+sinf(t*0.005f)*0.04f;
            rlPushMatrix(); rlTranslatef(400,y,0); rlScalef(esc,esc,1);
            float tw=(float)MeasureText(gs.opcionesPausa[i].c_str(),18);
            Drawing::texto18(-tw/2.f-2,-7,gs.opcionesPausa[i],CFloat(0,0,0));
            Drawing::texto18(-tw/2.f,  -5,gs.opcionesPausa[i],
                esGuardar?CFloat(0.4f,1.f,0.4f):WHITE);
            rlPopMatrix();
            Drawing::cursorAnimado(panX+40,y,t);
            // Iconos por opción
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
}

void PausaScreen::HandleInput(GameState& gs) {
    if(IsKeyPressed(KEY_W)||IsKeyPressed(KEY_UP))
        { if(--gs.opcionPausaSel<0) gs.opcionPausaSel=(int)gs.opcionesPausa.size()-1; }
    if(IsKeyPressed(KEY_S)||IsKeyPressed(KEY_DOWN))
        { if(++gs.opcionPausaSel>=(int)gs.opcionesPausa.size()) gs.opcionPausaSel=0; }

    if(IsKeyPressed(KEY_ENTER)){
        const std::string& opt=gs.opcionesPausa[gs.opcionPausaSel];
        if(opt=="CONTINUAR"){
            // Volver al estado de juego sin slash
            gs.estadoActual=gs.estadoAnterior;
        }
        else if(opt=="GUARDAR PARTIDA"){
            // Guardar con nombre automático basado en la fecha
            if(SaveSystem::guardar(gs, gs.nombreGuardado)){
                SaveSystem::cargarTodas(gs);  // refrescar la lista en memoria
                gs.guardadoOk    = true;
                gs.guardadoTimer = 2.f;
            }
        }
        else if(opt=="OPCIONES"){
            gs.estadoActual=gs.estadoAnterior;
            transicion(gs,OPCIONES);
        }
        else if(opt=="MENU PRINCIPAL"){
            transicion(gs,MENU);
        }
    }
    // ESC = continuar
    if(IsKeyPressed(KEY_ESCAPE)) gs.estadoActual=gs.estadoAnterior;
}

void PausaScreen::HandleMouse(GameState& gs) {
    if(!IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) return;
    float mxv=(float)GetMouseX()*800.f/GetScreenWidth(), myv=600.f-(float)GetMouseY()*600.f/GetScreenHeight();
    float oy=gs.pausaOffset, panY=110-oy, panH=360;
    for(int i=0;i<(int)gs.opcionesPausa.size();i++){
        float y=panY+panH-96-i*68.f;
        if(mxv>210&&mxv<590&&myv>y-22&&myv<y+22){
            gs.opcionPausaSel=i;
            // Doble clic o simplemente mover el foco; el ENTER lo ejecuta
        }
    }
}
