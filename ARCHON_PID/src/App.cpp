#include "App.h"
#include "Drawing.h"
#include "Particles.h"
#include "Background.h"
#include "Screens.h"
#include "rlgl.h"

void App::Run() {
    Init();
    while (!WindowShouldClose()) {
        HandleInput();
        HandleMouse();
        Update();
        Draw();
    }
    CloseWindow();
}

void App::Init() {
    srand((unsigned)time(nullptr));
    InitWindow(800, 600, "ARCHON");
    SetTargetFPS(60);
    gs.init();
    Particles::init(gs);
}

void App::IniciarTransicion(Estado destino) {
    if (gs.slashActivo) return;
    gs.slashActivo = true;
    gs.siguienteEstado = destino;
}

void App::TogglePantallaCompleta() {
    gs.pantallaCompleta = !gs.pantallaCompleta;
    gs.controlesOpciones[2].valor = gs.pantallaCompleta ? 1 : 0;
    ToggleFullscreen();
}

void App::Update() {
    // Animation speed: control index 4, range 1-10
    float velBase = 0.3f;
    float velScale = gs.controlesOpciones[4].valor / 5.0f;
    gs.tiempo += velBase * velScale;

    // Offset slide-in animations
    if (gs.estadoActual == MENU && gs.menuOffset < 0) gs.menuOffset += 2.0f;
    if (gs.estadoActual == JUGAR && gs.jugarOffset < 0) gs.jugarOffset += 2.0f;
    if (gs.estadoActual == ENCICLOPEDIA && gs.encicloOffset < 0) gs.encicloOffset += 3.0f;
    if (gs.estadoActual == OPCIONES && gs.opcionesOffset < 0) gs.opcionesOffset += 3.0f;
    if (gs.estadoActual == MUSICA && gs.musicaOffset < 0) gs.musicaOffset += 3.0f;

    // Particles
    Particles::update(gs);

    // Visualizer bars
    for (int i = 0; i < NUM_BARRAS; i++) {
        if (gs.reproduciendo) {
            float fase = gs.tiempo * 0.02f * (0.5f + i * 0.08f);
            float base = 8 + sinf(fase) * 5 + sinf(fase * 0.7f + i) * 4 + sinf(fase * 1.3f) * 3;
            base *= (float)gs.volumenMusica / 10.0f;
            if (base < 1) base = 1;
            gs.barraTarget[i] = base;
        } else {
            gs.barraTarget[i] = 2.0f;
        }
        gs.barrasViz[i] += (gs.barraTarget[i] - gs.barrasViz[i]) * 0.12f;
    }

    // Music progress auto-advance
    if (gs.reproduciendo && gs.estadoActual == MUSICA) {
        gs.progresoCancion += gs.velocidadProg;
        if (gs.progresoCancion >= 1.0f) {
            gs.progresoCancion = 0.0f;
            if (gs.aleatorio)        gs.cancionActual = rand() % (int)gs.canciones.size();
            else if (!gs.repetir)    gs.cancionActual = (gs.cancionActual + 1) % (int)gs.canciones.size();
            gs.cancionHover = gs.cancionActual;
        }
    }

    // Slash transition
    if (gs.slashActivo) {
        gs.slashX += 12;
        if (gs.slashX > 900) {
            gs.slashActivo = false;
            gs.slashX = -1000.0f;
            gs.estadoActual = gs.siguienteEstado;
            if (gs.estadoActual == MENU)         gs.menuOffset = -400.0f;
            if (gs.estadoActual == JUGAR)        gs.jugarOffset = -600.0f;
            if (gs.estadoActual == ENCICLOPEDIA) gs.encicloOffset = -800.0f;
            if (gs.estadoActual == OPCIONES)     gs.opcionesOffset = -800.0f;
            if (gs.estadoActual == MUSICA)       gs.musicaOffset = -800.0f;
        }
    }
}

void App::Draw() {
    BeginDrawing();
    ClearBackground(BLACK);
    Drawing::setupProjection();

    switch (gs.estadoActual) {
    case MENU:         Screens::menuPrincipal(gs); break;
    case JUGAR:        Screens::menuJugar(gs);     break;
    case OPCIONES:     Screens::menuOpciones(gs);  break;
    case MUSICA:       Screens::menuMusica(gs);    break;
    case ENCICLOPEDIA: Screens::enciclopedia(gs);  break;
    default: break;
    }

    Screens::espadaSlash(gs);
    EndDrawing();
}

void App::HandleInput() {
    if (gs.slashActivo) {
        // F11 always works
        if (IsKeyPressed(KEY_F11)) TogglePantallaCompleta();
        return;
    }

    if (IsKeyPressed(KEY_F11)) {
        TogglePantallaCompleta();
        return;
    }

    if (gs.estadoActual == MENU) {
        if (IsKeyPressed(KEY_W) || IsKeyPressed(KEY_UP)) {
            gs.opcionMenuSel--;
            if (gs.opcionMenuSel < 0) gs.opcionMenuSel = (int)gs.opcionesMenu.size() - 1;
        }
        if (IsKeyPressed(KEY_S) || IsKeyPressed(KEY_DOWN)) {
            gs.opcionMenuSel++;
            if (gs.opcionMenuSel >= (int)gs.opcionesMenu.size()) gs.opcionMenuSel = 0;
        }
        if (IsKeyPressed(KEY_ENTER)) {
            if (gs.opcionMenuSel == 0) IniciarTransicion(JUGAR);
            if (gs.opcionMenuSel == 1) IniciarTransicion(OPCIONES);
            if (gs.opcionMenuSel == 2) IniciarTransicion(ENCICLOPEDIA);
        }
    }
    else if (gs.estadoActual == JUGAR) {
        if (IsKeyPressed(KEY_W) || IsKeyPressed(KEY_UP)) {
            gs.opcionJugarSel--;
            if (gs.opcionJugarSel < 0) gs.opcionJugarSel = (int)gs.opcionesJugar.size() - 1;
        }
        if (IsKeyPressed(KEY_S) || IsKeyPressed(KEY_DOWN)) {
            gs.opcionJugarSel++;
            if (gs.opcionJugarSel >= (int)gs.opcionesJugar.size()) gs.opcionJugarSel = 0;
        }
        if (IsKeyPressed(KEY_ENTER)) IniciarTransicion(gs.opcionJugarSel == 0 ? JUGAR_PVP : JUGAR_IA);
        if (IsKeyPressed(KEY_ESCAPE)) IniciarTransicion(MENU);
    }
    else if (gs.estadoActual == OPCIONES) {
        if (IsKeyPressed(KEY_W) || IsKeyPressed(KEY_UP)) {
            gs.opcionOpcionesSel--;
            if (gs.opcionOpcionesSel < 0) gs.opcionOpcionesSel = (int)gs.controlesOpciones.size() - 1;
        }
        if (IsKeyPressed(KEY_S) || IsKeyPressed(KEY_DOWN)) {
            gs.opcionOpcionesSel++;
            if (gs.opcionOpcionesSel >= (int)gs.controlesOpciones.size()) gs.opcionOpcionesSel = 0;
        }
        if (IsKeyPressed(KEY_A) || IsKeyPressed(KEY_LEFT)) {
            OpcionControl& op = gs.controlesOpciones[gs.opcionOpcionesSel];
            int anterior = op.valor;
            op.valor = std::max(op.minV, op.valor - 1);
            if (gs.opcionOpcionesSel == 2 && op.valor != anterior)
                TogglePantallaCompleta();
        }
        if (IsKeyPressed(KEY_D) || IsKeyPressed(KEY_RIGHT)) {
            OpcionControl& op = gs.controlesOpciones[gs.opcionOpcionesSel];
            int anterior = op.valor;
            op.valor = std::min(op.maxV, op.valor + 1);
            if (gs.opcionOpcionesSel == 2 && op.valor != anterior)
                TogglePantallaCompleta();
        }
        if (IsKeyPressed(KEY_ENTER)) IniciarTransicion(MUSICA);
        if (IsKeyPressed(KEY_ESCAPE)) IniciarTransicion(MENU);
    }
    else if (gs.estadoActual == MUSICA) {
        if (IsKeyPressed(KEY_W) || IsKeyPressed(KEY_UP)) {
            gs.cancionHover--;
            if (gs.cancionHover < 0) gs.cancionHover = (int)gs.canciones.size() - 1;
            gs.cancionActual = gs.cancionHover;
            gs.progresoCancion = 0; gs.reproduciendo = true;
        }
        if (IsKeyPressed(KEY_S) || IsKeyPressed(KEY_DOWN)) {
            gs.cancionHover++;
            if (gs.cancionHover >= (int)gs.canciones.size()) gs.cancionHover = 0;
            gs.cancionActual = gs.cancionHover;
            gs.progresoCancion = 0; gs.reproduciendo = true;
        }
        if (IsKeyPressed(KEY_SPACE)) gs.reproduciendo = !gs.reproduciendo;
        if (IsKeyPressed(KEY_R)) gs.repetir = !gs.repetir;
        if (IsKeyPressed(KEY_X)) gs.aleatorio = !gs.aleatorio;
        if (IsKeyPressed(KEY_A) || IsKeyPressed(KEY_LEFT)) {
            gs.volumenMusica = std::max(0, gs.volumenMusica - 1);
            gs.controlesOpciones[0].valor = gs.volumenMusica;
        }
        if (IsKeyPressed(KEY_D) || IsKeyPressed(KEY_RIGHT)) {
            gs.volumenMusica = std::min(10, gs.volumenMusica + 1);
            gs.controlesOpciones[0].valor = gs.volumenMusica;
        }
        if (IsKeyPressed(KEY_ESCAPE)) IniciarTransicion(OPCIONES);
    }
    else if (gs.estadoActual == ENCICLOPEDIA) {
        if (IsKeyPressed(KEY_A) || IsKeyPressed(KEY_LEFT)) {
            gs.paginaLibro--;
            if (gs.paginaLibro < 0) gs.paginaLibro = gs.TOTAL_PAGINAS - 1;
        }
        if (IsKeyPressed(KEY_D) || IsKeyPressed(KEY_RIGHT)) {
            gs.paginaLibro++;
            if (gs.paginaLibro >= gs.TOTAL_PAGINAS) gs.paginaLibro = 0;
        }
        if (IsKeyPressed(KEY_ESCAPE)) IniciarTransicion(MENU);
    }
}

void App::HandleMouse() {
    if (!IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) return;
    if (gs.slashActivo) return;

    float mx = (float)GetMouseX();
    float my = 600.0f - (float)GetMouseY(); // Y-flip for Y-up coords

    if (gs.estadoActual == MENU) {
        for (int i = 0; i < (int)gs.opcionesMenu.size(); i++) {
            float oy = 350.0f - i * 70.0f;
            if (mx > 250 && mx < 640 && my > oy - 25 && my < oy + 25) {
                gs.opcionMenuSel = i;
                if (i == 0) IniciarTransicion(JUGAR);
                if (i == 1) IniciarTransicion(OPCIONES);
                if (i == 2) IniciarTransicion(ENCICLOPEDIA);
            }
        }
    }
    else if (gs.estadoActual == JUGAR) {
        for (int i = 0; i < (int)gs.opcionesJugar.size(); i++) {
            float oy = 390.0f - i * 100.0f;
            if (mx > 150 && mx < 700 && my > oy - 32 && my < oy + 32) {
                gs.opcionJugarSel = i;
                IniciarTransicion(i == 0 ? JUGAR_PVP : JUGAR_IA);
            }
        }
    }
    else if (gs.estadoActual == OPCIONES) {
        // Music button
        if (mx > 115 && mx < 685 && my > 448 && my < 484) {
            IniciarTransicion(MUSICA);
            return;
        }
        // Option sliders
        for (int i = 0; i < (int)gs.controlesOpciones.size(); i++) {
            float y = 435.0f - i * 46.0f;
            if (mx > 320 && mx < 620 && my > y - 4 && my < y + 14) {
                gs.opcionOpcionesSel = i;
                OpcionControl& op = gs.controlesOpciones[i];
                int anterior = op.valor;
                float t2 = (mx - 320.0f) / 300.0f;
                op.valor = (int)(t2 * (op.maxV - op.minV) + op.minV);
                op.valor = std::max(op.minV, std::min(op.maxV, op.valor));
                if (i == 2 && op.valor != anterior)
                    TogglePantallaCompleta();
            }
        }
    }
    else if (gs.estadoActual == MUSICA) {
        // Song list clicks
        float listX = 370, listY = 475;
        for (int i = 0; i < (int)gs.canciones.size(); i++) {
            float fy = listY - 30.0f - i * 30.0f;
            if (mx > listX && mx < listX + 365 && my > fy - 8 && my < fy + 14) {
                gs.cancionActual = i; gs.cancionHover = i;
                gs.progresoCancion = 0; gs.reproduciendo = true;
            }
        }
        // Progress bar click
        if (mx > 60 && mx < 350 && my > 196 && my < 204) {
            gs.progresoCancion = (mx - 60.0f) / 290.0f;
            gs.progresoCancion = std::max(0.0f, std::min(1.0f, gs.progresoCancion));
        }
        // Transport controls
        float ctrlCX = 200, ctrlY = 165;
        float dx2 = mx - ctrlCX, dy2 = my - ctrlY;
        if (dx2 * dx2 + dy2 * dy2 < 22 * 22) gs.reproduciendo = !gs.reproduciendo;
        dx2 = mx - (ctrlCX - 42 * 2); dy2 = my - ctrlY;
        if (dx2 * dx2 + dy2 * dy2 < 16 * 16) {
            gs.cancionActual--; if (gs.cancionActual < 0) gs.cancionActual = (int)gs.canciones.size() - 1;
            gs.cancionHover = gs.cancionActual; gs.progresoCancion = 0;
        }
        dx2 = mx - (ctrlCX + 42 * 2); dy2 = my - ctrlY;
        if (dx2 * dx2 + dy2 * dy2 < 16 * 16) {
            gs.cancionActual++; if (gs.cancionActual >= (int)gs.canciones.size()) gs.cancionActual = 0;
            gs.cancionHover = gs.cancionActual; gs.progresoCancion = 0;
        }
        // Repeat button
        dx2 = mx - (ctrlCX - 42 * 3 + 8); dy2 = my - ctrlY;
        if (dx2 * dx2 + dy2 * dy2 < 12 * 12) gs.repetir = !gs.repetir;
        // Random button
        dx2 = mx - (ctrlCX + 42 * 3 - 8); dy2 = my - ctrlY;
        if (dx2 * dx2 + dy2 * dy2 < 12 * 12) gs.aleatorio = !gs.aleatorio;
        // Volume slider
        if (mx > 88 && mx < 268 && my > 122 && my < 140) {
            gs.volumenMusica = (int)((mx - 88.0f) / 180.0f * 10.0f);
            gs.volumenMusica = std::max(0, std::min(10, gs.volumenMusica));
            gs.controlesOpciones[0].valor = gs.volumenMusica;
        }
    }
    else if (gs.estadoActual == ENCICLOPEDIA) {
        if (mx < 300) { gs.paginaLibro--; if (gs.paginaLibro < 0) gs.paginaLibro = gs.TOTAL_PAGINAS - 1; }
        if (mx > 500) { gs.paginaLibro++; if (gs.paginaLibro >= gs.TOTAL_PAGINAS) gs.paginaLibro = 0; }
    }
}
