#include "App.h"
#include "Drawing.h"
#include "Particles.h"
#include "Background.h"
#include "Screens.h"
#include "SaveSystem.h"
#include "rlgl.h"

// Devuelve la pantalla activa o nullptr 
Screen* App::GetCurrentScreen() {
    auto it = screens.find((int)gs.estadoActual);
    if (it != screens.end()) return it->second.get();
    return nullptr;
}

void App::Init() {
    srand((unsigned)time(nullptr));
    InitWindow(800, 600, "ARCHON");
    SetTargetFPS(60);
    gs.init();
    Particles::init(gs);

    // Cargar partidas guardadas al arrancar
    SaveSystem::cargarTodas(gs);

    // Registrar pantallas por estado
    screens[(int)MODO_JUEGO]           = std::make_unique<ModoJuegoScreen>();
    screens[(int)SELECCION_MODO]       = std::make_unique<SeleccionModoScreen>();
    screens[(int)CONFIG_JUEGO_COMPLETO]= std::make_unique<ConfigJuegoCompletoScreen>();
    screens[(int)SELECCION_EQUIPO]     = std::make_unique<SeleccionEquipoScreen>();
    screens[(int)CONFIG_DIFICULTAD]    = std::make_unique<ConfigDificultadScreen>();
    screens[(int)CARGAR_PARTIDA]       = std::make_unique<CargarPartidaScreen>();
    screens[(int)PAUSA]                = std::make_unique<PausaScreen>();
}

void App::IniciarTransicion(Estado destino) {
    if (gs.slashActivo) return;
    // La pausa no necesita slash — entra deslizándose desde arriba
    if (destino == PAUSA) {
        gs.estadoAnterior = gs.estadoActual;
        gs.estadoActual   = PAUSA;
        gs.pausaOffset    = 600.f;
        if (auto* scr = GetCurrentScreen()) scr->OnEnter(gs);
        return;
    }
    gs.estadoAnterior  = gs.estadoActual;
    gs.slashActivo     = true;
    gs.siguienteEstado = destino;
}

void App::TogglePantallaCompleta() {
    gs.pantallaCompleta = !gs.pantallaCompleta;
    gs.controlesOpciones[2].valor = gs.pantallaCompleta ? 1 : 0;
    ToggleFullscreen();
}

void App::Update() {
    float vel = gs.controlesOpciones[4].valor / 5.f;
    gs.tiempo += 0.3f * vel;

    // Cada pantalla desliza desde fuera al centro al entrar
    if (gs.estadoActual == MENU               && gs.menuOffset      < 0) gs.menuOffset      += 2.f;
    if (gs.estadoActual == ENCICLOPEDIA       && gs.encicloOffset   < 0) gs.encicloOffset   += 3.f;
    if (gs.estadoActual == OPCIONES           && gs.opcionesOffset  < 0) gs.opcionesOffset  += 3.f;
    if (gs.estadoActual == MUSICA             && gs.musicaOffset    < 0) gs.musicaOffset    += 3.f;
    if (gs.estadoActual == MODO_JUEGO         && gs.modoJuegoOffset < 0) gs.modoJuegoOffset += 2.f;
    if (gs.estadoActual == SELECCION_MODO     && gs.selModoOffset   < 0) gs.selModoOffset   += 2.f;
    if (gs.estadoActual == CONFIG_JUEGO_COMPLETO && gs.configJCOffset < 0) gs.configJCOffset += 3.f;
    if (gs.estadoActual == SELECCION_EQUIPO   && gs.selEquipoOffset < 0) gs.selEquipoOffset += 3.f;
    if (gs.estadoActual == CONFIG_DIFICULTAD  && gs.configDifiOffset< 0) gs.configDifiOffset+= 3.f;
    if (gs.estadoActual == CARGAR_PARTIDA     && gs.cargaOffset     < 0) gs.cargaOffset     += 3.f;
    // La pausa baja desde arriba (positivo → 0)
    if (gs.estadoActual == PAUSA && gs.pausaOffset > 0)
        gs.pausaOffset = std::max(0.f, gs.pausaOffset - 18.f);

    // Timer para el mensaje de "Guardado OK"
    if (gs.guardadoOk) {
        gs.guardadoTimer -= GetFrameTime();
        if (gs.guardadoTimer <= 0.f) gs.guardadoOk = false;
    }

    if (auto* scr = GetCurrentScreen()) scr->Update(gs);

    Particles::update(gs);

    // Visualizador del reproductor de música
    for (int i = 0; i < NUM_BARRAS; i++) {
        if (gs.reproduciendo) {
            float f = gs.tiempo * 0.02f * (0.5f + i * 0.08f);
            float b = 8 + sinf(f)*5 + sinf(f*0.7f+i)*4 + sinf(f*1.3f)*3;
            b *= gs.volumenMusica / 10.f;
            gs.barraTarget[i] = std::max(1.f, b);
        } else gs.barraTarget[i] = 2.f;
        gs.barrasViz[i] += (gs.barraTarget[i] - gs.barrasViz[i]) * 0.12f;
    }
    if (gs.reproduciendo && gs.estadoActual == MUSICA) {
        gs.progresoCancion += gs.velocidadProg;
        if (gs.progresoCancion >= 1.f) {
            gs.progresoCancion = 0.f;
            if (gs.aleatorio)     gs.cancionActual = rand() % (int)gs.canciones.size();
            else if (!gs.repetir) gs.cancionActual = (gs.cancionActual+1) % (int)gs.canciones.size();
            gs.cancionHover = gs.cancionActual;
        }
    }

    // Slash de transición
    if (gs.slashActivo) {
        gs.slashX += 12;
        if (gs.slashX > 900) {
            gs.slashActivo  = false;
            gs.slashX       = -1000.f;
            gs.estadoActual = gs.siguienteEstado;
            // Resetear el offset de la pantalla destino para que entre deslizando
            switch (gs.estadoActual) {
                case MENU:                gs.menuOffset      = -400.f; break;
                case ENCICLOPEDIA:        gs.encicloOffset   = -800.f; break;
                case OPCIONES:            gs.opcionesOffset  = -800.f; break;
                case MUSICA:              gs.musicaOffset    = -800.f; break;
                case MODO_JUEGO:          gs.modoJuegoOffset = -600.f; break;
                case SELECCION_MODO:      gs.selModoOffset   = -600.f; break;
                case CONFIG_JUEGO_COMPLETO: gs.configJCOffset= -800.f; break;
                case SELECCION_EQUIPO:    gs.selEquipoOffset = -800.f; break;
                case CONFIG_DIFICULTAD:   gs.configDifiOffset= -800.f; break;
                case CARGAR_PARTIDA:      gs.cargaOffset     = -800.f; break;
                default: break;
            }
            if (auto* scr = GetCurrentScreen()) scr->OnEnter(gs);
        }
    }
}

void App::Draw() {
    BeginDrawing();
    ClearBackground(BLACK);
    Drawing::setupProjection();

    switch (gs.estadoActual) {
        case MENU:         Screens::menuPrincipal(gs); break;
        case OPCIONES:     Screens::menuOpciones(gs);  break;
        case MUSICA:       Screens::menuMusica(gs);    break;
        case ENCICLOPEDIA: Screens::enciclopedia(gs);  break;
        default:
            if (auto* scr = GetCurrentScreen()) scr->Draw(gs);
            break;
    }

    Screens::espadaSlash(gs);
    EndDrawing();
}

void App::HandleInput() {
    if (IsKeyPressed(KEY_F11)) { TogglePantallaCompleta(); return; }
    // P activa la pausa desde estados de juego activo
    if (IsKeyPressed(KEY_P) &&
        (gs.estadoActual == JUGAR_PVP || gs.estadoActual == JUGAR_IA)) {
        IniciarTransicion(PAUSA);
        return;
    }
    if (gs.slashActivo) return;

    if (auto* scr = GetCurrentScreen()) {
        scr->HandleInput(gs);
        return;
    }

    // --- Input legacy para pantallas que no usan herencia ---
    if (gs.estadoActual == MENU) {
        if (IsKeyPressed(KEY_W)||IsKeyPressed(KEY_UP)) {
            if (--gs.opcionMenuSel < 0) gs.opcionMenuSel = (int)gs.opcionesMenu.size()-1;
        }
        if (IsKeyPressed(KEY_S)||IsKeyPressed(KEY_DOWN)) {
            if (++gs.opcionMenuSel >= (int)gs.opcionesMenu.size()) gs.opcionMenuSel = 0;
        }
        if (IsKeyPressed(KEY_ENTER)) {
            if (gs.opcionMenuSel == 0) IniciarTransicion(MODO_JUEGO);
            if (gs.opcionMenuSel == 1) IniciarTransicion(CARGAR_PARTIDA);
            if (gs.opcionMenuSel == 2) IniciarTransicion(OPCIONES);
            if (gs.opcionMenuSel == 3) IniciarTransicion(ENCICLOPEDIA);
        }
    }
    else if (gs.estadoActual == OPCIONES) {
        if (IsKeyPressed(KEY_W)||IsKeyPressed(KEY_UP)) {
            if (--gs.opcionOpcionesSel < 0) gs.opcionOpcionesSel = (int)gs.controlesOpciones.size()-1;
        }
        if (IsKeyPressed(KEY_S)||IsKeyPressed(KEY_DOWN)) {
            if (++gs.opcionOpcionesSel >= (int)gs.controlesOpciones.size()) gs.opcionOpcionesSel = 0;
        }
        if (IsKeyPressed(KEY_A)||IsKeyPressed(KEY_LEFT)) {
            auto& op = gs.controlesOpciones[gs.opcionOpcionesSel];
            int ant = op.valor;
            op.valor = std::max(op.minV, op.valor-1);
            if (gs.opcionOpcionesSel == 2 && op.valor != ant) TogglePantallaCompleta();
        }
        if (IsKeyPressed(KEY_D)||IsKeyPressed(KEY_RIGHT)) {
            auto& op = gs.controlesOpciones[gs.opcionOpcionesSel];
            int ant = op.valor;
            op.valor = std::min(op.maxV, op.valor+1);
            if (gs.opcionOpcionesSel == 2 && op.valor != ant) TogglePantallaCompleta();
        }
        if (IsKeyPressed(KEY_ENTER)) IniciarTransicion(MUSICA);
        if (IsKeyPressed(KEY_ESCAPE)) IniciarTransicion(MENU);
    }
    else if (gs.estadoActual == MUSICA) {
        if (IsKeyPressed(KEY_W)||IsKeyPressed(KEY_UP)) {
            if (--gs.cancionHover < 0) gs.cancionHover = (int)gs.canciones.size()-1;
            gs.cancionActual = gs.cancionHover; gs.progresoCancion = 0; gs.reproduciendo = true;
        }
        if (IsKeyPressed(KEY_S)||IsKeyPressed(KEY_DOWN)) {
            if (++gs.cancionHover >= (int)gs.canciones.size()) gs.cancionHover = 0;
            gs.cancionActual = gs.cancionHover; gs.progresoCancion = 0; gs.reproduciendo = true;
        }
        if (IsKeyPressed(KEY_SPACE)) gs.reproduciendo = !gs.reproduciendo;
        if (IsKeyPressed(KEY_R))     gs.repetir       = !gs.repetir;
        if (IsKeyPressed(KEY_X))     gs.aleatorio     = !gs.aleatorio;
        if (IsKeyPressed(KEY_A)||IsKeyPressed(KEY_LEFT)) {
            gs.volumenMusica = std::max(0, gs.volumenMusica-1);
            gs.controlesOpciones[0].valor = gs.volumenMusica;
        }
        if (IsKeyPressed(KEY_D)||IsKeyPressed(KEY_RIGHT)) {
            gs.volumenMusica = std::min(10, gs.volumenMusica+1);
            gs.controlesOpciones[0].valor = gs.volumenMusica;
        }
        if (IsKeyPressed(KEY_ESCAPE)) IniciarTransicion(OPCIONES);
    }
    else if (gs.estadoActual == ENCICLOPEDIA) {
        if (IsKeyPressed(KEY_A)||IsKeyPressed(KEY_LEFT)) {
            if (--gs.paginaLibro < 0) gs.paginaLibro = gs.TOTAL_PAGINAS-1;
        }
        if (IsKeyPressed(KEY_D)||IsKeyPressed(KEY_RIGHT)) {
            if (++gs.paginaLibro >= gs.TOTAL_PAGINAS) gs.paginaLibro = 0;
        }
        if (IsKeyPressed(KEY_ESCAPE)) IniciarTransicion(MENU);
    }
}

void App::HandleMouse() {
    if (!IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) return;
    if (gs.slashActivo) return;

    if (auto* scr = GetCurrentScreen()) {
        scr->HandleMouse(gs);
        return;
    }

    float mxv = (float)GetMouseX();
    float myv = 600.f - (float)GetMouseY();

    if (gs.estadoActual == MENU) {
        for (int i = 0; i < (int)gs.opcionesMenu.size(); i++) {
            float oy = 350.f - i*70.f;
            if (mxv>250 && mxv<640 && myv>oy-25 && myv<oy+25) {
                gs.opcionMenuSel = i;
                if (i==0) IniciarTransicion(MODO_JUEGO);
                if (i==1) IniciarTransicion(CARGAR_PARTIDA);
                if (i==2) IniciarTransicion(OPCIONES);
                if (i==3) IniciarTransicion(ENCICLOPEDIA);
            }
        }
    }
    else if (gs.estadoActual == OPCIONES) {
        if (mxv>115 && mxv<685 && myv>448 && myv<484) { IniciarTransicion(MUSICA); return; }
        for (int i = 0; i < (int)gs.controlesOpciones.size(); i++) {
            float y = 435.f - i*46.f;
            if (mxv>320 && mxv<620 && myv>y-4 && myv<y+14) {
                gs.opcionOpcionesSel = i;
                auto& op = gs.controlesOpciones[i];
                int ant = op.valor;
                float t2 = (mxv-320.f)/300.f;
                op.valor = std::max(op.minV, std::min(op.maxV, (int)(t2*(op.maxV-op.minV)+op.minV)));
                if (i==2 && op.valor!=ant) TogglePantallaCompleta();
            }
        }
    }
    else if (gs.estadoActual == MUSICA) {
        float lX=370, lY=475;
        for (int i = 0; i < (int)gs.canciones.size(); i++) {
            float fy = lY - 30.f - i*30.f;
            if (mxv>lX && mxv<lX+365 && myv>fy-8 && myv<fy+14) {
                gs.cancionActual = i; gs.cancionHover = i;
                gs.progresoCancion = 0; gs.reproduciendo = true;
            }
        }
        if (mxv>60&&mxv<350&&myv>196&&myv<204)
            gs.progresoCancion = std::max(0.f,std::min(1.f,(mxv-60.f)/290.f));
        float cX=200, cY=165, dx, dy;
        dx=mxv-cX;    dy=myv-cY; if(dx*dx+dy*dy<484) gs.reproduciendo=!gs.reproduciendo;
        dx=mxv-(cX-84);dy=myv-cY; if(dx*dx+dy*dy<256){if(--gs.cancionActual<0)gs.cancionActual=(int)gs.canciones.size()-1;gs.cancionHover=gs.cancionActual;gs.progresoCancion=0;}
        dx=mxv-(cX+84);dy=myv-cY; if(dx*dx+dy*dy<256){if(++gs.cancionActual>=(int)gs.canciones.size())gs.cancionActual=0;gs.cancionHover=gs.cancionActual;gs.progresoCancion=0;}
        dx=mxv-(cX-118);dy=myv-cY; if(dx*dx+dy*dy<144) gs.repetir=!gs.repetir;
        dx=mxv-(cX+118);dy=myv-cY; if(dx*dx+dy*dy<144) gs.aleatorio=!gs.aleatorio;
        if(mxv>88&&mxv<268&&myv>122&&myv<140){
            gs.volumenMusica=std::max(0,std::min(10,(int)((mxv-88.f)/180.f*10.f)));
            gs.controlesOpciones[0].valor=gs.volumenMusica;
        }
    }
    else if (gs.estadoActual == ENCICLOPEDIA) {
        if (mxv<300) { if(--gs.paginaLibro<0) gs.paginaLibro=gs.TOTAL_PAGINAS-1; }
        if (mxv>500) { if(++gs.paginaLibro>=gs.TOTAL_PAGINAS) gs.paginaLibro=0; }
    }
}

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
