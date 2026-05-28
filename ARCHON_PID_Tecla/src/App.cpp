#include "App.h"
#include "Drawing.h"
#include "Particles.h"
#include "Background.h"
#include "Screens.h"
#include "SaveSystem.h"
#include "CuchauScreen.h"
#include "rlgl.h"

// Devuelve la pantalla activa o nullptr 
Screen* App::GetCurrentScreen() {
    auto it = screens.find((int)gs.estadoActual);
    if (it != screens.end()) return it->second.get();
    return nullptr;
}


void App::Init() {
    srand((unsigned)time(nullptr));
    InitWindow(0, 0, "ARCHON");
    SetExitKey(0);  // Disable ESC auto-close; we handle ESC manually
    SetTargetFPS(60);
    InitAudioDevice();

    // Ensure working directory is the project root (where bin/Resources lives)
    // GetApplicationDirectory() returns exe location — walk up until bin/ is found
    const char* appDir = GetApplicationDirectory();
    ChangeDirectory(appDir);
    if (!DirectoryExists("bin/Resources")) ChangeDirectory("..");
    if (!DirectoryExists("bin/Resources")) ChangeDirectory("..");

    TraceLog(LOG_INFO, "Working directory: %s", GetWorkingDirectory());

    gs.pantallaCompleta = false;
    gs.init();
    Particles::init(gs);

    // Cargar partidas guardadas al arrancar
    SaveSystem::cargarTodas(gs);

    // Cargar y reproducir musica del menu
    musicaGlobal = LoadMusicStream("bin/Resources/AAAudio/Musica/Menugnomo.mp3");
    musicaGlobal.looping = true;
    PlayMusicStream(musicaGlobal);
    SetMusicVolume(musicaGlobal, gs.volumenMusica / 10.0f);
    musicaGlobalCargada = true;
    gs.reproduciendo = true;
    gs.cancionActual = 0;  // Menugnomo es la cancion 0

    // Registrar pantallas por estado
    screens[(int)MODO_JUEGO]           = std::make_unique<ModoJuegoScreen>();
    screens[(int)SELECCION_MODO]       = std::make_unique<SeleccionModoScreen>();
    screens[(int)CONFIG_JUEGO_COMPLETO]= std::make_unique<ConfigJuegoCompletoScreen>();
    screens[(int)SELECCION_EQUIPO]     = std::make_unique<SeleccionEquipoScreen>();
    screens[(int)CONFIG_DIFICULTAD]    = std::make_unique<ConfigDificultadScreen>();
    screens[(int)CARGAR_PARTIDA]       = std::make_unique<CargarPartidaScreen>();
    screens[(int)PAUSA]                = std::make_unique<PausaScreen>();
    screens[(int)CUCHAU_COMBATE]       = std::make_unique<CuchauCombateScreen>();
}

// Instantly finish any ongoing transition, applying the pending state
void App::FinalizarTransicion() {
    if (!gs.slashActivo) return;
    gs.slashActivo  = false;
    gs.slashX       = -1000.f;
    if (gs.estadoActual != gs.siguienteEstado) {
        Estado anterior = gs.estadoActual;
        gs.estadoActual = gs.siguienteEstado;
        switch (gs.estadoActual) {
            case MENU:                gs.menuOffset      = 0.f; break;
            case ENCICLOPEDIA:        gs.encicloOffset   = 0.f; break;
            case OPCIONES:            gs.opcionesOffset  = 0.f; break;
            case MUSICA:              gs.musicaOffset    = 0.f; break;
            case MODO_JUEGO:          gs.modoJuegoOffset = 0.f; break;
            case SELECCION_MODO:      gs.selModoOffset   = 0.f; break;
            case CONFIG_JUEGO_COMPLETO: gs.configJCOffset= 0.f; break;
            case SELECCION_EQUIPO:    gs.selEquipoOffset = 0.f; break;
            case CONFIG_DIFICULTAD:   gs.configDifiOffset= 0.f; break;
            case CARGAR_PARTIDA:      gs.cargaOffset     = 0.f; break;
            default: break;
        }
        // Pausar musica global al entrar en combate, reanudar al salir
        // Al entrar en combate: pausar música global
        if (gs.estadoActual == CUCHAU_COMBATE) PausarMusicaGlobal();
        if (anterior == CUCHAU_COMBATE && gs.estadoActual != CUCHAU_COMBATE) ReanudarMusicaGlobal();
        if (auto* scr = GetCurrentScreen()) scr->OnEnter(gs);
    }
}

void App::IniciarTransicion(Estado destino) {
    // If mid-animation, finish it instantly before starting the new one
    if (gs.slashActivo) FinalizarTransicion();
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

    // Actualizar musica global
    if (musicaGlobalCargada) {
        UpdateMusicStream(musicaGlobal);
        SetMusicVolume(musicaGlobal, gs.volumenMusica / 10.0f);

        // Sincronizar progreso real
        float len = GetMusicTimeLength(musicaGlobal);
        float played = GetMusicTimePlayed(musicaGlobal);
        if (len > 0) gs.progresoCancion = played / len;

        // Actualizar duracion real en la cancion actual
        if (gs.cancionActual >= 0 && gs.cancionActual < (int)gs.canciones.size() && len > 0) {
            int mins = (int)len / 60;
            int secs = (int)len % 60;
            gs.canciones[gs.cancionActual].duracion = std::to_string(mins) + ":" + (secs < 10 ? "0" : "") + std::to_string(secs);
        }

        // Detectar fin de cancion y avanzar
        if (gs.reproduciendo && gs.progresoCancion >= 0.99f) {
            int siguiente = gs.cancionActual;
            if (gs.aleatorio)     siguiente = rand() % (int)gs.canciones.size();
            else if (!gs.repetir) siguiente = (gs.cancionActual + 1) % (int)gs.canciones.size();
            CambiarCancionGlobal(siguiente);
        }
    }

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

    // Slash de transición
    if (gs.slashActivo) {
        gs.slashX += 30;
        // Switch state at midpoint so the new screen loads while slash exits
        if (gs.slashX >= 0 && gs.estadoActual != gs.siguienteEstado) {
            Estado anterior = gs.estadoActual;
            gs.estadoActual = gs.siguienteEstado;
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
            // Pausar musica global al entrar en combate, reanudar al salir
            if (gs.estadoActual == CUCHAU_COMBATE) PausarMusicaGlobal();
            if (anterior == CUCHAU_COMBATE && gs.estadoActual != CUCHAU_COMBATE) ReanudarMusicaGlobal();
            if (auto* scr = GetCurrentScreen()) scr->OnEnter(gs);
        }
        // End animation when slash exits screen
        if (gs.slashX > 900) {
            gs.slashActivo = false;
            gs.slashX      = -1000.f;
        }
    }
}

void App::Draw() {
    BeginDrawing();
    ClearBackground(BLACK);

    // Cuchau sets its own projection — skip ARCHON's 800x600 ortho
    if (gs.estadoActual != CUCHAU_COMBATE)
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

    // Restore ARCHON projection for the slash overlay if needed
    if (gs.estadoActual == CUCHAU_COMBATE && gs.slashActivo) {
        rlDrawRenderBatchActive();
        rlMatrixMode(RL_PROJECTION);
        rlLoadIdentity();
        rlOrtho(0, 800, 0, 600, -1, 1);
        rlMatrixMode(RL_MODELVIEW);
        rlLoadIdentity();
    }
    
    Screens::espadaSlash(gs);
    EndDrawing();
}

void App::HandleInput() {
    animSkippedThisFrame = false;
    if (IsKeyPressed(KEY_F11)) { TogglePantallaCompleta(); return; }
    // P o ESC activa la pausa desde estados de juego activo
    if ((IsKeyPressed(KEY_P) || IsKeyPressed(KEY_ESCAPE)) &&
        (gs.estadoActual == JUGAR_PVP || gs.estadoActual == JUGAR_IA)) {
        IniciarTransicion(PAUSA);
        return;
    }
    // SPACE, ENTER, ESC or left click skip all transition animations (slash + slide-in offsets)
    bool skipAnim = IsKeyPressed(KEY_SPACE) || IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_ESCAPE) || IsMouseButtonPressed(MOUSE_BUTTON_LEFT);
    if (skipAnim && (gs.slashActivo || gs.menuOffset < 0 || gs.encicloOffset < 0 ||
        gs.opcionesOffset < 0 || gs.musicaOffset < 0 || gs.modoJuegoOffset < 0 ||
        gs.selModoOffset < 0 || gs.configJCOffset < 0 || gs.selEquipoOffset < 0 ||
        gs.configDifiOffset < 0 || gs.cargaOffset < 0 || gs.pausaOffset > 0)) {
        if (gs.slashActivo) FinalizarTransicion();
        gs.menuOffset = gs.encicloOffset = gs.opcionesOffset = gs.musicaOffset = 0.f;
        gs.modoJuegoOffset = gs.selModoOffset = gs.configJCOffset = 0.f;
        gs.selEquipoOffset = gs.configDifiOffset = gs.cargaOffset = 0.f;
        gs.pausaOffset = 0.f;
        // SPACE and mouse click only skip; ENTER/ESC fall through to act on the now-visible screen
        if (IsKeyPressed(KEY_SPACE) || IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            animSkippedThisFrame = true;
            return;
        }
    }
    if (gs.slashActivo) return;

    if (auto* scr = GetCurrentScreen()) {
        scr->HandleInput(gs);
        return;
    }

    // --- Navegación por teclado para pantallas legacy ---

    if (gs.estadoActual == MENU) {
        if (IsKeyPressed(KEY_W) || IsKeyPressed(KEY_UP)) {
            if (--gs.opcionMenuSel < 0)
                gs.opcionMenuSel = (int)gs.opcionesMenu.size() - 1;
        }
        if (IsKeyPressed(KEY_S) || IsKeyPressed(KEY_DOWN)) {
            if (++gs.opcionMenuSel >= (int)gs.opcionesMenu.size())
                gs.opcionMenuSel = 0;
        }
        if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_KP_ENTER)) {
            if (gs.opcionMenuSel == 0) IniciarTransicion(MODO_JUEGO);
            if (gs.opcionMenuSel == 1) IniciarTransicion(CARGAR_PARTIDA);
            if (gs.opcionMenuSel == 2) IniciarTransicion(OPCIONES);
            if (gs.opcionMenuSel == 3) IniciarTransicion(MUSICA);
            if (gs.opcionMenuSel == 4) IniciarTransicion(ENCICLOPEDIA);
            if (gs.opcionMenuSel == 5) CloseWindow();
        }
    }
    else if (gs.estadoActual == OPCIONES) {
        if (IsKeyPressed(KEY_W) || IsKeyPressed(KEY_UP)) {
            if (--gs.opcionOpcionesSel < 0)
                gs.opcionOpcionesSel = (int)gs.controlesOpciones.size() - 1;
        }
        if (IsKeyPressed(KEY_S) || IsKeyPressed(KEY_DOWN)) {
            if (++gs.opcionOpcionesSel >= (int)gs.controlesOpciones.size())
                gs.opcionOpcionesSel = 0;
        }
        if (IsKeyPressed(KEY_A) || IsKeyPressed(KEY_LEFT)) {
            auto& op = gs.controlesOpciones[gs.opcionOpcionesSel];
            if (gs.opcionOpcionesSel == 2) {
                // Pantalla Completa: dejar que TogglePantallaCompleta lo gestione todo
                if (op.valor == 1) TogglePantallaCompleta();
            }
            else {
                int ant = op.valor;
                op.valor = std::max(op.minV, op.valor - 1);
                if (gs.opcionOpcionesSel == 0) {
                    gs.volumenMusica = op.valor;
                    if (musicaGlobalCargada) SetMusicVolume(musicaGlobal, gs.volumenMusica / 10.f);
                }
            }
        }
        if (IsKeyPressed(KEY_D) || IsKeyPressed(KEY_RIGHT)) {
            auto& op = gs.controlesOpciones[gs.opcionOpcionesSel];
            if (gs.opcionOpcionesSel == 2) {
                // Pantalla Completa: dejar que TogglePantallaCompleta lo gestione todo
                if (op.valor == 0) TogglePantallaCompleta();
            }
            else {
                int ant = op.valor;
                op.valor = std::min(op.maxV, op.valor + 1);
                if (gs.opcionOpcionesSel == 0) {
                    gs.volumenMusica = op.valor;
                    if (musicaGlobalCargada) SetMusicVolume(musicaGlobal, gs.volumenMusica / 10.f);
                }
            }
        }
        if (IsKeyPressed(KEY_ESCAPE)) IniciarTransicion(MENU);
    }
    else if (gs.estadoActual == MUSICA) {
        if (IsKeyPressed(KEY_W) || IsKeyPressed(KEY_UP)) {
            int prev = gs.cancionActual - 1;
            if (prev < 0) prev = (int)gs.canciones.size() - 1;
            CambiarCancionGlobal(prev);
        }
        if (IsKeyPressed(KEY_S) || IsKeyPressed(KEY_DOWN)) {
            int next = (gs.cancionActual + 1) % (int)gs.canciones.size();
            CambiarCancionGlobal(next);
        }
        if (IsKeyPressed(KEY_SPACE)) {
            gs.reproduciendo = !gs.reproduciendo;
            if (gs.reproduciendo) ReanudarMusicaGlobal();
            else PausarMusicaGlobal();
        }
        if (IsKeyPressed(KEY_R)) gs.repetir  = !gs.repetir;
        if (IsKeyPressed(KEY_X)) gs.aleatorio = !gs.aleatorio;
        if (IsKeyPressed(KEY_A) || IsKeyPressed(KEY_LEFT)) {
            gs.volumenMusica = std::max(0, gs.volumenMusica - 1);
            gs.controlesOpciones[0].valor = gs.volumenMusica;
            if (musicaGlobalCargada) SetMusicVolume(musicaGlobal, gs.volumenMusica / 10.f);
        }
        if (IsKeyPressed(KEY_D) || IsKeyPressed(KEY_RIGHT)) {
            gs.volumenMusica = std::min(10, gs.volumenMusica + 1);
            gs.controlesOpciones[0].valor = gs.volumenMusica;
            if (musicaGlobalCargada) SetMusicVolume(musicaGlobal, gs.volumenMusica / 10.f);
        }
        if (IsKeyPressed(KEY_ESCAPE)) IniciarTransicion(MENU);
    }
    else if (gs.estadoActual == ENCICLOPEDIA) {
        if (IsKeyPressed(KEY_A) || IsKeyPressed(KEY_LEFT)) {
            if (--gs.paginaLibro < 0) gs.paginaLibro = gs.TOTAL_PAGINAS - 1;
        }
        if (IsKeyPressed(KEY_D) || IsKeyPressed(KEY_RIGHT)) {
            if (++gs.paginaLibro >= gs.TOTAL_PAGINAS) gs.paginaLibro = 0;
        }
        if (IsKeyPressed(KEY_ESCAPE)) IniciarTransicion(MENU);
    }
}

void App::HandleMouse() {
    if (gs.slashActivo) return;
    // If a click just skipped the animation this frame, don't act on it
    if (animSkippedThisFrame) return;

    bool clicked = IsMouseButtonPressed(MOUSE_BUTTON_LEFT);

    if (auto* scr = GetCurrentScreen()) {
        scr->HandleMouse(gs);
        return;
    }

    float mxv = (float)GetMouseX() * 800.f / GetScreenWidth();
    float myv = 600.f - (float)GetMouseY() * 600.f / GetScreenHeight();

    if (gs.estadoActual == MENU) {
        for (int i = 0; i < (int)gs.opcionesMenu.size(); i++) {
            float oy = 370.f - i*60.f;
            if (mxv>250 && mxv<640 && myv>oy-25 && myv<oy+25) {
                gs.opcionMenuSel = i;
                if (clicked) {
                    if (i==0) IniciarTransicion(MODO_JUEGO);
                    if (i==1) IniciarTransicion(CARGAR_PARTIDA);
                    if (i==2) IniciarTransicion(OPCIONES);
                    if (i==3) IniciarTransicion(MUSICA);
                    if (i==4) IniciarTransicion(ENCICLOPEDIA);
                    if (i==5) CloseWindow();
                }
            }
        }
    }
    else if (gs.estadoActual == OPCIONES) {
        // Hover
        for (int i = 0; i < (int)gs.controlesOpciones.size(); i++) {
            float y = 435.f - i * 46.f;
            if (mxv > 115 && mxv < 685 && myv > y - 24 && myv < y + 16)
                gs.opcionOpcionesSel = i;
        }
        if (clicked) {
            if (mxv > 40 && mxv < 135 && myv > 543 && myv < 589) { IniciarTransicion(MENU); return; }
            for (int i = 0; i < (int)gs.controlesOpciones.size(); i++) {
                float y = 435.f - i * 46.f;
                if (mxv > 320 && mxv < 620 && myv > y - 4 && myv < y + 14) {
                    gs.opcionOpcionesSel = i;
                    auto& op = gs.controlesOpciones[i];
                    int ant = op.valor;
                    float t2 = (mxv - 320.f) / 300.f;
                    op.valor = std::max(op.minV, std::min(op.maxV,
                        (int)(t2 * (op.maxV - op.minV) + op.minV)));
                    // Aplicar efectos inmediatos al cambiar con el ratón
                    if (i == 0) {
                        gs.volumenMusica = op.valor;
                        if (musicaGlobalCargada)
                            SetMusicVolume(musicaGlobal, gs.volumenMusica / 10.f);
                    }
                    if (i == 2 && op.valor != ant) TogglePantallaCompleta();
                }
            }
        }
    }
    else if (gs.estadoActual == MUSICA) {
        // Hover on song list
        float lX=370, lY=475;
        for (int i = 0; i < (int)gs.canciones.size(); i++) {
            float fy = lY - 30.f - i*30.f;
            if (mxv>lX && mxv<lX+365 && myv>fy-8 && myv<fy+14) {
                gs.cancionHover = i;
            }
        }
        if (clicked) {
            // Back button
            if (mxv>40 && mxv<135 && myv>543 && myv<589) { IniciarTransicion(MENU); return; }
            // Song list click
            for (int i = 0; i < (int)gs.canciones.size(); i++) {
                float fy = lY - 30.f - i*30.f;
                if (mxv>lX && mxv<lX+365 && myv>fy-8 && myv<fy+14) {
                    CambiarCancionGlobal(i);
                }
            }
            // Seek bar
            if (mxv>60&&mxv<350&&myv>196&&myv<204) {
                float pos = std::max(0.f,std::min(1.f,(mxv-60.f)/290.f));
                if (musicaGlobalCargada) {
                    float len = GetMusicTimeLength(musicaGlobal);
                    SeekMusicStream(musicaGlobal, pos * len);
                }
            }
            float cX=200, cY=165, dx, dy;
            // Play/Pause (center)
            dx=mxv-cX; dy=myv-cY;
            if(dx*dx+dy*dy<484) {
                gs.reproduciendo=!gs.reproduciendo;
                if (gs.reproduciendo) ReanudarMusicaGlobal();
                else PausarMusicaGlobal();
            }
            // Backwards (prev track, cX-50)
            dx=mxv-(cX-50);dy=myv-cY;
            if(dx*dx+dy*dy<256){
                int prev = gs.cancionActual - 1;
                if (prev < 0) prev = (int)gs.canciones.size() - 1;
                CambiarCancionGlobal(prev);
            }
            // Forwards (next track, cX+50)
            dx=mxv-(cX+50);dy=myv-cY;
            if(dx*dx+dy*dy<256){
                int next = (gs.cancionActual + 1) % (int)gs.canciones.size();
                CambiarCancionGlobal(next);
            }
            // Repeat (cX-100)
            dx=mxv-(cX-100);dy=myv-cY; if(dx*dx+dy*dy<144) gs.repetir=!gs.repetir;
            // Random (cX+100)
            dx=mxv-(cX+100);dy=myv-cY; if(dx*dx+dy*dy<144) gs.aleatorio=!gs.aleatorio;
            if(mxv>88&&mxv<268&&myv>122&&myv<140){
                gs.volumenMusica=std::max(0,std::min(10,(int)((mxv-88.f)/180.f*10.f)));
                gs.controlesOpciones[0].valor=gs.volumenMusica;
            }
        }
    }
    else if (gs.estadoActual == ENCICLOPEDIA) {
        if (clicked) {
            // Back button
            if (mxv>40 && mxv<135 && myv>543 && myv<589) { IniciarTransicion(MENU); return; }
            if (mxv<300) { if(--gs.paginaLibro<0) gs.paginaLibro=gs.TOTAL_PAGINAS-1; }
            if (mxv>500) { if(++gs.paginaLibro>=gs.TOTAL_PAGINAS) gs.paginaLibro=0; }
        }
    }
}

void App::CambiarCancionGlobal(int idx) {
    if (idx < 0 || idx >= (int)gs.canciones.size()) return;
    if (musicaGlobalCargada) {
        StopMusicStream(musicaGlobal);
        UnloadMusicStream(musicaGlobal);
    }
    musicaGlobal = LoadMusicStream(gs.canciones[idx].archivo.c_str());
    musicaGlobal.looping = true;
    PlayMusicStream(musicaGlobal);
    SetMusicVolume(musicaGlobal, gs.volumenMusica / 10.0f);
    musicaGlobalCargada = true;
    gs.cancionActual = idx;
    gs.cancionHover = idx;
    gs.progresoCancion = 0;
    gs.reproduciendo = true;
}

void App::PausarMusicaGlobal() {
    if (musicaGlobalCargada) PauseMusicStream(musicaGlobal);
}

void App::ReanudarMusicaGlobal() {
    if (!musicaGlobalCargada) return;
    if (!IsMusicStreamPlaying(musicaGlobal)) {
        PlayMusicStream(musicaGlobal);
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

    if (musicaGlobalCargada) UnloadMusicStream(musicaGlobal);
    CloseAudioDevice();
    CloseWindow();
}
