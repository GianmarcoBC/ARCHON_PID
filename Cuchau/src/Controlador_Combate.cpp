#include "Controlador_Combate.h"
#include <vector>
#include <algorithm>

//  CONSTRUCTOR — Inicializa la arena, personajes, obstaculos e IA
//
//  P1 empieza en la esquina inferior izquierda (-35% del ancho/largo)
//  P2 empieza en la esquina superior derecha (+35% del ancho/largo)
//  La Y de ambos es 2.0 (mitad del charSize) para que el billboard
//  quede apoyado sobre el suelo.

ControladorCombate::ControladorCombate(Pj_info pj1, Pj_info pj2, bool vsIA, int dificultad)
    : P1(pj1,
          { KEY_W, KEY_S, KEY_A, KEY_D },                               // Controles P1: WASD
          { -arena.getSueloWidth() * 0.35f, 1.0f, -arena.getSueloLength() * 0.35f },         // Posicion inicial P1
          true)                                                          // Es jugador humano
    , P2(pj2,
          vsIA ? cntrl{ 0, 0, 0, 0 } : cntrl{ KEY_UP, KEY_DOWN, KEY_LEFT, KEY_RIGHT },  // Sin teclas si es IA
          {  arena.getSueloWidth() * 0.35f, 1.0f,  arena.getSueloLength() * 0.35f },         // Posicion inicial P2
          !vsIA)                                                         // Es jugador solo si NO es IA
{
    // --- Crear obstaculos (en heap porque requieren contexto OpenGL) ---
    // Fuente central: billboard 7, hitbox 5x3 (base ancha, poco profunda)
    obstaculos.push_back(new obstaculo("bin/Resources/AAGraficos/fuente.png", "bin/Resources/AAGraficos/fuente_shadow.png",
        { 0.0f, 3.5f, 0.0f }, 7, 2.5, 1.5));
    // 4 vallas en las esquinas: billboard 3, hitbox 3x1 (anchas y poco profundas)
    obstaculos.push_back(new obstaculo("bin/Resources/AAGraficos/obstaculo.png", "bin/Resources/AAGraficos/obstaculo_shadow.png",
        { -10.0f, 1.5f, 6.0f }, 3, 3.5, 0.5));
    obstaculos.push_back(new obstaculo("bin/Resources/AAGraficos/obstaculo.png", "bin/Resources/AAGraficos/obstaculo_shadow.png",
        { 10.0f, 1.5f, 6.0f }, 3, 3.5, 0.5));
    obstaculos.push_back(new obstaculo("bin/Resources/AAGraficos/obstaculo.png", "bin/Resources/AAGraficos/obstaculo_shadow.png",
        { -10.0f, 1.5f, -6.0f }, 3, 3.5, 0.5));
    obstaculos.push_back(new obstaculo("bin/Resources/AAGraficos/obstaculo.png", "bin/Resources/AAGraficos/obstaculo_shadow.png",
        { 10.0f, 1.5f, -6.0f }, 3, 3.5, 0.5));

    // --- Cargar shader de alpha discard ---
    alphaDiscard = LoadShader(0, "bin/Resources/alpha_discard.fs");

    // --- Copia si es Shapeshifter ---
	if ("ShapeShifter" == pj1.nombre) {
        P1.copy(P2);
	}
	else if ("ShapeShifter" == pj2.nombre) {
		P2.copy(P1);
	}


    // --- Crear IA si es modo VS IA ---
    if (vsIA) {
        Rectangle arenaBounds = { -arena.getSueloWidth() / 2, -arena.getSueloLength() / 2,   };
        switch (dificultad) {
        case 0:  ia = new AI_Facil(P2, P1, arenaBounds); break;
        case 1:  ia = new AI_Defensiva(P2, P1, arenaBounds); break;
        case 2:  ia = new AI_Agresiva(P2, P1, arenaBounds); break;
        default: ia = new AI_Facil(P2, P1, arenaBounds); break;
        }
    }
}

// ====================================================================
//  UPDATE — Logica completa de un frame de combate
//
//  Secuencia:
//    1. Input de P1 + disparo con cooldown
//    2. IA decide o input de P2 + disparo con cooldown
//    3. Actualizar posicion de todos los disparos
//    4. Colision disparos contra personajes (aplica dano)
//    5. Colision personajes contra obstaculos (empuje)
//    6. Colision personaje contra personaje (empuje mutuo)
//    7. Limitar personajes dentro de la arena (clamp)
// ====================================================================

void ControladorCombate::Update()
{
    if (IsGameOver()) return;  // No actualizar si alguien ya murio

    float dt = GetFrameTime();

    // ── Player 1: movimiento y disparo ───────────────────────────────
    P1.Update(dt);

    
    if (IsKeyPressed(KEY_SPACE)) {
        auto nuevos1 = P1.Shoot();
        Disparos_1.insert(Disparos_1.end(), nuevos1.begin(), nuevos1.end());
    }


    // ── Player 2 / IA ────────────────────────────────────────────────
    if (ia != nullptr) {
        // Modo IA: la IA mueve a P2 y decide si disparar
        bool dispara = ia->Update(dt, Disparos_1);

        if (dispara) {
            auto nuevos2 = P2.Shoot();
            Disparos_2.insert(Disparos_2.end(), nuevos2.begin(), nuevos2.end());
        }

    }
    else {
        // Modo 2 jugadores: P2 usa teclado (flechas + Ctrl derecho)
        P2.Update(dt);

        if (IsKeyPressed(KEY_RIGHT_CONTROL)) {
            auto nuevos2 = P2.Shoot();
            Disparos_2.insert(Disparos_2.end(), nuevos2.begin(), nuevos2.end());
        }

    }

    // ── Actualizar posicion de disparos y desactivar los que salen ───
    for (auto& d : Disparos_1) d.Update(dt, arena.getSueloWidth() / 2, arena.getSueloLength() / 2);
    for (auto& d : Disparos_2) d.Update(dt, arena.getSueloWidth() / 2, arena.getSueloLength() / 2);

    // ── Colision disparos vs personajes (aplica dano) ────────────────
    Interacciones::DisparosContraPersonaje(Disparos_1, P1, P2);   // Disparos de P1 → danan a P2
    Interacciones::DisparosContraPersonaje(Disparos_2, P2, P1);   // Disparos de P2 → danan a P1

    // ── Colisiones fisicas ───────────────────────────────────────────
    Interacciones::PersonajesContraObstaculos(P1, P2, obstaculos);  // Empuje vs obstaculos
    Interacciones::PersonajeContraPersonaje(P1, P2);                          // Empuje mutuo
    Interacciones::ClampArena(P1, arena);   // Limitar P1 dentro de la arena
    Interacciones::ClampArena(P2, arena);   // Limitar P2 dentro de la arena
}

// ====================================================================
//  DRAW — Dibuja el frame completo
//
//  Si el combate termino, muestra la pantalla de victoria.
//  Si no, dibuja la arena 3D y el HUD 2D encima.
// ====================================================================

void ControladorCombate::Draw()
{
    if (IsGameOver()) {
        DrawVictory();
        return;
    }

    Draw3D();
}

// ====================================================================
//  DRAW 3D — Pipeline de renderizado completo de la arena
//
//  Orden de renderizado:
//    1. Fondo 2D (imagen de ciudad a pantalla completa)
//    2. BeginMode3D → espacio 3D
//    3. Suelo (plano texturizado de 40x25)
//    4. Pared trasera (grid de 10x6 paneles rotados 90° en X)
//    5. Bordes de la arena (cubos marrones en los 4 lados)
//    6. Almenas (cubos decorativos encima de los bordes)
//    7. Sombras de personajes y obstaculos (BLEND_MULTIPLIED)
//    8. Billboards: obstaculos + personajes + disparos activos
//       ordenados por profundidad (painter's algorithm, lejos→cerca)
//    9. EndMode3D
// ====================================================================

void ControladorCombate::Draw3D()
{
	arena.DrawFondo(); // Dibuja el fondo 2D (ciudad)


    BeginMode3D(camera);

    arena.Draw(camera);  // Dibuja fondo, suelo, paredes y bordes


    BeginShaderMode(alphaDiscard);

    for (auto* obs : obstaculos) {
        obs->Draw(camera);
    }

    P1.Draw(camera);
    P2.Draw(camera);

	for (const auto& d : Disparos_1) d.Draw(camera);
	for (const auto& d : Disparos_2) d.Draw(camera);

    EndShaderMode();

    EndMode3D();

    P1.drawHUD(camera, SKYBLUE);
    P2.drawHUD(camera, RED);
}

//  PANTALLA DE VICTORIA
//
//  Dibuja la arena congelada (ultimo frame del combate) como fondo,
//  superpone un overlay oscuro semitransparente, y muestra:
//    - Texto grande "JUGADOR 1/2 GANA!" con sombra
//    - Mensaje parpadeante "PULSA ENTER PARA VOLVER AL MENU"

void ControladorCombate::DrawVictory()
{
    // Dibuja la arena congelada de fondo
    Draw3D();

    // Overlay oscuro semitransparente
    DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), { 0, 0, 0, 160 });

    int W = GetScreenWidth();
    int H = GetScreenHeight();

    // Texto del ganador (con color del jugador correspondiente)
    const char* msg = (GetWinner() == 1) ? "JUGADOR 1 GANA!" : "JUGADOR 2 GANA!";
    Color       col = (GetWinner() == 1) ? SKYBLUE : RED;

    // Texto con sombra negra (desplazada 3px abajo-derecha)
    DrawText(msg, W / 2 - MeasureText(msg, 50) / 2 + 3, H / 2 - 47, 50, BLACK);
    DrawText(msg, W / 2 - MeasureText(msg, 50) / 2, H / 2 - 50, 50, col);

    // Mensaje parpadeante para volver al menu
    const char* sub = "PULSA ENTER PARA VOLVER AL MENU";
    if ((int)(GetTime() * 2) % 2 == 0)
        DrawText(sub, W / 2 - MeasureText(sub, 22) / 2, H / 2 + 20, 22, YELLOW);
}

//  GUARDADO / CARGA — Persiste y restaura el estado del combate

bool ControladorCombate::GuardarEstado(bool modoIA, int dificultad) const
{
    SaveData d;
    d.nombreP1   = P1.GetNombre().data();
    d.vidaP1     = P1.GetVida();
    d.posP1x     = P1.GetPos().x;
    d.posP1y     = P1.GetPos().y;
    d.nombreP2   = P2.GetNombre().data();
    d.vidaP2     = P2.GetVida();
    d.posP2x     = P2.GetPos().x;
    d.posP2y     = P2.GetPos().y;
    d.modoIA     = modoIA;
    d.dificultad = dificultad;
    return SaveManager::Guardar(d);
}

void ControladorCombate::CargarEstado(const SaveData& d)
{
    P1.SetVida(d.vidaP1);
    P1.SetPos({ d.posP1x, d.posP1y });
    P2.SetVida(d.vidaP2);
    P2.SetPos({ d.posP2x, d.posP2y });
}

//  DESTRUCTOR — Libera todos los recursos
//
//  Libera en orden:
//    1. IA (delete)
//    2. Personajes (texturas, modelos, sonidos)
//    3. Obstaculos (texturas, modelos) + delete
//    4. Modelos de suelo y pared
//    5. Texturas de la arena

ControladorCombate::~ControladorCombate()
{
    UnloadShader(alphaDiscard);
    delete ia;
    for (auto* obs : obstaculos) {
        delete obs;
    }
}
