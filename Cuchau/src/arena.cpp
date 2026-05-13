#include "arena.h"
#include <vector>
#include <algorithm>

// ====================================================================
//  CONSTRUCTOR — Inicializa la arena, personajes, obstaculos e IA
//
//  P1 empieza en la esquina inferior izquierda (-35% del ancho/largo)
//  P2 empieza en la esquina superior derecha (+35% del ancho/largo)
//  La Y de ambos es 2.0 (mitad del charSize) para que el billboard
//  quede apoyado sobre el suelo.
// ====================================================================

arena::arena(Pj_info pj1, Pj_info pj2, bool vsIA, int dificultad)
    : P1(pj1,
          { KEY_W, KEY_S, KEY_A, KEY_D },                               // Controles P1: WASD
          { -sueloWidth * 0.35f, 2.0f, -sueloLength * 0.35f },         // Posicion inicial P1
          true)                                                          // Es jugador humano
    , P2(pj2,
          vsIA ? cntrl{ 0, 0, 0, 0 } : cntrl{ KEY_UP, KEY_DOWN, KEY_LEFT, KEY_RIGHT },  // Sin teclas si es IA
          {  sueloWidth * 0.35f, 2.0f,  sueloLength * 0.35f },         // Posicion inicial P2
          !vsIA)                                                         // Es jugador solo si NO es IA
    , interaccion({ -sueloWidth / 2, -sueloLength / 2, sueloWidth, sueloLength })  // Limites de la arena XZ
{
    // --- Cargar texturas de la arena ---
    texBg       = LoadTexture("bin/Resources/AAGraficos/ciudadnormal2.png");     // Fondo 2D (ciudad)
    texPared    = LoadTexture("bin/Resources/AAGraficos/paredcalida.png");       // Textura de los paneles de la pared trasera
    texSuelo    = LoadTexture("bin/Resources/AAGraficos/suelo.png");             // Textura del suelo
    texGameOver = LoadTexture("bin/Resources/AAGraficos/the-dark-background-minimalism-tmnt-teenage-mutant-ninja-turtles-wallpaper-preview.png");

    // --- Generar geometria del suelo ---
    sueloMesh  = GenMeshPlane(sueloWidth, sueloLength, 1, 1);  // Plano de 40x25 unidades
    sueloModel = LoadModelFromMesh(sueloMesh);
    sueloModel.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = texSuelo;

    // --- Generar geometria de la pared (un panel individual) ---
    wallMesh  = GenMeshPlane(wallWidth, wallHeight, 1, 1);  // Plano de 4x2 unidades
    wallModel = LoadModelFromMesh(wallMesh);
    wallModel.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = texPared;

    // --- Crear obstaculos (en heap porque requieren contexto OpenGL) ---
    // Fuente central: posicion (0, 3.5, 0), billboard tamano 7, colision 5x5
    fuente = new obstaculo("bin/Resources/AAGraficos/fuente.png", "bin/Resources/AAGraficos/fuente_shadow.png",
                           { 0.0f, 3.5f, 0.0f }, 7, 5, 5);
    // 4 postes en las esquinas interiores: tamano 3, colision 2x2
    palos1 = new obstaculo("bin/Resources/AAGraficos/obstaculo.png", "bin/Resources/AAGraficos/obstaculo_shadow.png",
                           { -10.0f, 1.5f, 6.0f }, 3, 2, 2);
    palos2 = new obstaculo("bin/Resources/AAGraficos/obstaculo.png", "bin/Resources/AAGraficos/obstaculo_shadow.png",
                           {  10.0f, 1.5f, 6.0f }, 3, 2, 2);
    palos3 = new obstaculo("bin/Resources/AAGraficos/obstaculo.png", "bin/Resources/AAGraficos/obstaculo_shadow.png",
                           { -10.0f, 1.5f, -6.0f }, 3, 2, 2);
    palos4 = new obstaculo("bin/Resources/AAGraficos/obstaculo.png", "bin/Resources/AAGraficos/obstaculo_shadow.png",
                           {  10.0f, 1.5f, -6.0f }, 3, 2, 2);

    // Array para iterar sobre todos los obstaculos
    obstaculos[0] = fuente;
    obstaculos[1] = palos1;
    obstaculos[2] = palos2;
    obstaculos[3] = palos3;
    obstaculos[4] = palos4;

    // --- Crear IA si es modo VS IA ---
    if (vsIA) {
        Rectangle arenaBounds = { -sueloWidth / 2, -sueloLength / 2, sueloWidth, sueloLength };
        ia = new CombatAI(P2, P1, arenaBounds);
        // Delays de reaccion por dificultad: facil ~250ms, normal ~120ms, dificil ~40ms
        float delays[3] = { 60.0f * 0.25f, 60.0f * 0.12f, 60.0f * 0.04f };
        ia->SetReactionDelay(delays[dificultad]);
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

void arena::Update()
{
    if (IsGameOver()) return;  // No actualizar si alguien ya murio

    float dt = GetFrameTime();

    // ── Player 1: movimiento y disparo ───────────────────────────────
    P1.Update(dt);

    if (cooldown1 <= 0.0f) {
        if (IsKeyPressed(KEY_SPACE)) {
            Disparos_1.push_back(P1.Shoot());   // Crear disparo
            P1.PlayAttackSound();                // Sonido de ataque
            cooldown1 = P1.get_Cooldown();       // Reiniciar cooldown
        }
    } else {
        cooldown1 -= dt;  // Reducir cooldown
    }

    // ── Player 2 / IA ────────────────────────────────────────────────
    if (ia != nullptr) {
        // Modo IA: la IA mueve a P2 y decide si disparar
        bool dispara = ia->Update(dt, Disparos_1);
        if (cooldown2 <= 0.0f) {
            if (dispara) {
                Disparos_2.push_back(P2.Shoot());
                P2.PlayAttackSound();
                cooldown2 = P2.get_Cooldown();
            }
        } else {
            cooldown2 -= dt;
        }
    }
    else {
        // Modo 2 jugadores: P2 usa teclado (flechas + Ctrl derecho)
        P2.Update(dt);
        if (cooldown2 <= 0.0f) {
            if (IsKeyPressed(KEY_RIGHT_CONTROL)) {
                Disparos_2.push_back(P2.Shoot());
                P2.PlayAttackSound();
                cooldown2 = P2.get_Cooldown();
            }
        } else {
            cooldown2 -= dt;
        }
    }

    // ── Actualizar posicion de disparos y desactivar los que salen ───
    for (auto& d : Disparos_1) d.Update(dt, sueloWidth / 2, sueloLength / 2);
    for (auto& d : Disparos_2) d.Update(dt, sueloWidth / 2, sueloLength / 2);

    // ── Colision disparos vs personajes (aplica dano) ────────────────
    interaccion.DisparosContraPersonaje(Disparos_1, P1, P2);   // Disparos de P1 → danan a P2
    interaccion.DisparosContraPersonaje(Disparos_2, P2, P1);   // Disparos de P2 → danan a P1

    // ── Colisiones fisicas ───────────────────────────────────────────
    interaccion.PersonajesContraObstaculos(P1, P2, obstaculos, NUM_OBS);  // Empuje vs obstaculos
    interaccion.PersonajeContraPersonaje(P1, P2);                          // Empuje mutuo
    interaccion.ClampArena(P1);   // Limitar P1 dentro de la arena
    interaccion.ClampArena(P2);   // Limitar P2 dentro de la arena
}

// ====================================================================
//  DRAW — Dibuja el frame completo
//
//  Si el combate termino, muestra la pantalla de victoria.
//  Si no, dibuja la arena 3D y el HUD 2D encima.
// ====================================================================

void arena::Draw()
{
    if (IsGameOver()) {
        DrawVictory();
        return;
    }

    Draw3D();
    DrawHUD();
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

void arena::Draw3D()
{
    // 1. Fondo 2D (dibujado antes de Mode3D, cubre toda la pantalla)
    DrawTexturePro(texBg,
        { 0, 0, (float)texBg.width, (float)texBg.height },
        { 0, 0, (float)GetScreenWidth(), (float)GetScreenHeight() },
        { 0, 0 }, 0.0f, WHITE);

    BeginMode3D(camera);

    // 2. Suelo texturizado
    DrawModel(sueloModel, { 0.0f, 0.0f, 0.0f }, 1.0f, WHITE);

    // 3. Pared trasera: muro de texturas (10 columnas x 6 filas de paneles)
    //    Cada panel es un plano rotado 90° en X para que quede vertical
    for (float col = 0; col < 10; col++) {
        for (int row = 0; row < 6; row++) {
            float x = -sueloWidth / 2 + col * wallWidth + wallWidth / 2;
            float y = -(row * wallHeight + wallHeight / 2);
            DrawModelEx(wallModel, { x, y, sueloLength / 2 },
                        { 1, 0, 0 }, 90.0f, { 1, 1, 1 }, WHITE);
        }
    }

    // 4. Bordes de la arena (cubos marrones en los 4 lados)
    DrawCube({  0.0f, 0.75f,  12.5f }, 40.0f, 1.5f, 1.5f, { 150, 80, 70, 255 });  // Borde trasero
    DrawCube({  0.0f, 0.75f, -12.5f }, 40.0f, 1.5f, 1.5f, { 150, 80, 70, 255 });  // Borde frontal
    DrawCube({ -20.0f, 0.75f,  0.0f },  1.5f, 1.5f, 25.0f, { 150, 80, 70, 255 }); // Borde izquierdo
    DrawCube({  20.0f, 0.75f,  0.0f },  1.5f, 1.5f, 25.0f, { 150, 80, 70, 255 }); // Borde derecho

    // 5. Almenas decorativas encima de los bordes (cubos mas claros)
    for (int i = 0; i < 10; i++) {
        float x = -18.0f + i * 4.0f;
        DrawCube({ x, 2.25f,  12.5f }, 2.0f, 1.5f, 1.5f, { 180, 80, 70, 255 });  // Almenas traseras
        DrawCube({ x, 2.25f, -12.5f }, 2.0f, 1.5f, 1.5f, { 180, 80, 70, 255 });  // Almenas frontales
    }
    for (int i = 0; i < 6; i++) {
        float z = -10.0f + i * 4.0f;
        DrawCube({ -20.0f, 2.25f, z }, 1.5f, 1.5f, 2.0f, { 180, 80, 70, 255 }); // Almenas izquierdas
        DrawCube({  20.0f, 2.25f, z }, 1.5f, 1.5f, 2.0f, { 180, 80, 70, 255 }); // Almenas derechas
    }

    // ── 6. Sombras (BLEND_MULTIPLIED para oscurecer el suelo) ────────
    float cs = P1.GetCharSize();
    // Sombra de P1: en el suelo (y=0.01), desplazada medio charSize en Z
    Vector3 shadowPos1 = { P1.GetPos3D().x, 0.01f, P1.GetPos3D().z - cs / 2 };
    Vector3 shadowPos2 = { P2.GetPos3D().x, 0.01f, P2.GetPos3D().z - cs / 2 };
    P1.drawshadow(shadowPos1);
    P2.drawshadow(shadowPos2);

    // Sombras de los 5 obstaculos
    fuente->drawshadow();
    palos1->drawshadow();
    palos2->drawshadow();
    palos3->drawshadow();
    palos4->drawshadow();

    // ── 7-8. Billboards con Painter's algorithm ─────────────────────
    // Estructura temporal para ordenar todos los billboards por profundidad
    struct Billboard { Texture2D tex; Vector3 pos; float size; };

    // Calcular la direccion de vista de la camara (para proyeccion de profundidad)
    Vector3 cam = camera.position;
    float fx = camera.target.x - cam.x;
    float fy = camera.target.y - cam.y;
    float fz = camera.target.z - cam.z;

    // Funcion lambda que calcula la profundidad de un punto (producto punto con direccion de vista)
    auto depth = [&](Vector3 p) {
        return (p.x - cam.x) * fx + (p.y - cam.y) * fy + (p.z - cam.z) * fz;
    };

    // Recolectar todos los billboards: obstaculos + personajes + disparos
    std::vector<Billboard> billboards;
    billboards.reserve(7 + Disparos_1.size() + Disparos_2.size());

    // Obstaculos (5 billboards)
    billboards.push_back({ palos1->sprite, palos1->pos, palos1->size });
    billboards.push_back({ palos2->sprite, palos2->pos, palos2->size });
    billboards.push_back({ palos3->sprite, palos3->pos, palos3->size });
    billboards.push_back({ palos4->sprite, palos4->pos, palos4->size });
    billboards.push_back({ fuente->sprite, fuente->pos, fuente->size });

    // Personajes (2 billboards)
    billboards.push_back({ P1.getCurrentFrame(), P1.GetPos3D(), P1.GetCharSize() });
    billboards.push_back({ P2.getCurrentFrame(), P2.GetPos3D(), P2.GetCharSize() });

    // Disparos activos (variable cantidad)
    for (const auto& d : Disparos_1)
        if (d.getStatus())
            billboards.push_back({ *d.getTexture(), d.GetPos3D(), Disparo::size });
    for (const auto& d : Disparos_2)
        if (d.getStatus())
            billboards.push_back({ *d.getTexture(), d.GetPos3D(), Disparo::size });

    // Insertion sort: ordenar de lejos a cerca (los mas lejanos se dibujan primero)
    // Esto implementa el painter's algorithm para transparencia correcta
    int n = (int)billboards.size();
    for (int i = 1; i < n; i++) {
        Billboard key = billboards[i];
        int j = i - 1;
        while (j >= 0 && depth(billboards[j].pos) < depth(key.pos)) {
            billboards[j + 1] = billboards[j];
            j--;
        }
        billboards[j + 1] = key;
    }

    // Dibujar todos los billboards en orden (lejos → cerca)
    for (const auto& b : billboards)
        DrawBillboard(camera, b.tex, b.pos, b.size, WHITE);

    EndMode3D();
}

// ====================================================================
//  HUD — Barras de vida y nombres sobre los personajes
//
//  Usa GetWorldToScreen() para proyectar las posiciones 3D de los
//  personajes a coordenadas 2D de pantalla. Dibuja sobre cada uno:
//    - Barra de vida (fondo gris + barra de color proporcional a la vida)
//    - Valor numerico de vida
//    - Nombre del personaje
// ====================================================================

void arena::DrawHUD()
{
    // Proyectar posiciones 3D a 2D
    Vector2 sp1 = GetWorldToScreen(P1.GetPos3D(), camera);
    Vector2 sp2 = GetWorldToScreen(P2.GetPos3D(), camera);

    float barW = 60, barH = 6;  // Dimensiones de la barra de vida

    // --- HUD de P1 (color verde/azul) ---
    float hp1 = P1.GetVida() / P1.GetMaxVida();  // Porcentaje de vida
    DrawRectangle((int)(sp1.x - barW / 2), (int)(sp1.y - 50), (int)barW, (int)barH, DARKGRAY);        // Fondo
    DrawRectangle((int)(sp1.x - barW / 2), (int)(sp1.y - 50), (int)(barW * hp1), (int)barH, GREEN);   // Vida actual
    DrawText(TextFormat("%.0f", P1.GetVida()),
             (int)(sp1.x - 16), (int)(sp1.y - 66), 16, WHITE);    // Numero de vida
    DrawText(P1.GetNombre(),
             (int)(sp1.x) - MeasureText(P1.GetNombre(), 14) / 2,
             (int)(sp1.y - 80), 14, SKYBLUE);                     // Nombre

    // --- HUD de P2 (color rojo) ---
    float hp2 = P2.GetVida() / P2.GetMaxVida();
    DrawRectangle((int)(sp2.x - barW / 2), (int)(sp2.y - 50), (int)barW, (int)barH, DARKGRAY);
    DrawRectangle((int)(sp2.x - barW / 2), (int)(sp2.y - 50), (int)(barW * hp2), (int)barH, RED);
    DrawText(TextFormat("%.0f", P2.GetVida()),
             (int)(sp2.x - 16), (int)(sp2.y - 66), 16, WHITE);
    DrawText(P2.GetNombre(),
             (int)(sp2.x) - MeasureText(P2.GetNombre(), 14) / 2,
             (int)(sp2.y - 80), 14, RED);
}

// ====================================================================
//  PANTALLA DE VICTORIA
//
//  Dibuja la arena congelada (ultimo frame del combate) como fondo,
//  superpone un overlay oscuro semitransparente, y muestra:
//    - Texto grande "JUGADOR 1/2 GANA!" con sombra
//    - Mensaje parpadeante "PULSA ENTER PARA VOLVER AL MENU"
// ====================================================================

void arena::DrawVictory()
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

// ====================================================================
//  GUARDADO / CARGA — Persiste y restaura el estado del combate
// ====================================================================

bool arena::GuardarEstado(bool modoIA, int dificultad) const
{
    SaveData d;
    d.nombreP1   = P1.GetNombre();
    d.vidaP1     = P1.GetVida();
    d.posP1x     = P1.GetPos().x;
    d.posP1y     = P1.GetPos().y;
    d.nombreP2   = P2.GetNombre();
    d.vidaP2     = P2.GetVida();
    d.posP2x     = P2.GetPos().x;
    d.posP2y     = P2.GetPos().y;
    d.modoIA     = modoIA;
    d.dificultad = dificultad;
    return SaveManager::Guardar(d);
}

void arena::CargarEstado(const SaveData& d)
{
    P1.SetVida(d.vidaP1);
    P1.SetPos({ d.posP1x, d.posP1y });
    P2.SetVida(d.vidaP2);
    P2.SetPos({ d.posP2x, d.posP2y });
}

// ====================================================================
//  DESTRUCTOR — Libera todos los recursos
//
//  Libera en orden:
//    1. IA (delete)
//    2. Personajes (texturas, modelos, sonidos)
//    3. Obstaculos (texturas, modelos) + delete
//    4. Modelos de suelo y pared
//    5. Texturas de la arena
// ====================================================================

arena::~arena()
{
    delete ia;

    P1.UnloadPersonaje();
    P2.UnloadPersonaje();

    fuente->UnloadObstaculo(); delete fuente;
    palos1->UnloadObstaculo(); delete palos1;
    palos2->UnloadObstaculo(); delete palos2;
    palos3->UnloadObstaculo(); delete palos3;
    palos4->UnloadObstaculo(); delete palos4;

    UnloadModel(wallModel);
    UnloadModel(sueloModel);

    UnloadTexture(texPared);
    UnloadTexture(texBg);
    UnloadTexture(texSuelo);
    UnloadTexture(texGameOver);
}
