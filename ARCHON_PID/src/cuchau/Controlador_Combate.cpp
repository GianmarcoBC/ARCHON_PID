#include "Controlador_Combate.h"
#include <vector>
#include <algorithm>

ControladorCombate::ControladorCombate(Pj_info pj1, Pj_info pj2, bool vsIA, int dificultad)
    : P1(pj1,
          { KEY_W, KEY_S, KEY_A, KEY_D },
          { -arena.getSueloWidth() * 0.35f, 1.0f, -arena.getSueloLength() * 0.35f },
          true)
    , P2(pj2,
          vsIA ? cntrl{ 0, 0, 0, 0 } : cntrl{ KEY_UP, KEY_DOWN, KEY_LEFT, KEY_RIGHT },
          {  arena.getSueloWidth() * 0.35f, 1.0f,  arena.getSueloLength() * 0.35f },
          !vsIA)
{
    obstaculos.push_back(new obstaculo("Resources/AAGraficos/fuente.png", "Resources/AAGraficos/fuente_shadow.png",
        { 0.0f, 3.5f, 0.0f }, 7, 2.5, 1.5));
    obstaculos.push_back(new obstaculo("Resources/AAGraficos/obstaculo.png", "Resources/AAGraficos/obstaculo_shadow.png",
        { -10.0f, 1.5f, 6.0f }, 3, 3.5, 0.5));
    obstaculos.push_back(new obstaculo("Resources/AAGraficos/obstaculo.png", "Resources/AAGraficos/obstaculo_shadow.png",
        { 10.0f, 1.5f, 6.0f }, 3, 3.5, 0.5));
    obstaculos.push_back(new obstaculo("Resources/AAGraficos/obstaculo.png", "Resources/AAGraficos/obstaculo_shadow.png",
        { -10.0f, 1.5f, -6.0f }, 3, 3.5, 0.5));
    obstaculos.push_back(new obstaculo("Resources/AAGraficos/obstaculo.png", "Resources/AAGraficos/obstaculo_shadow.png",
        { 10.0f, 1.5f, -6.0f }, 3, 3.5, 0.5));

    alphaDiscard = LoadShader(0, "Resources/alpha_discard.fs");

    if ("ShapeShifter" == pj1.nombre) {
        P1.copy(P2);
    }
    else if ("ShapeShifter" == pj2.nombre) {
        P2.copy(P1);
    }

    if (vsIA) {
        Rectangle arenaBounds = { -arena.getSueloWidth() / 2, -arena.getSueloLength() / 2,
                                   arena.getSueloWidth(),       arena.getSueloLength() };
        switch (dificultad) {
        case 0:  ia = new AI_Facil(P2, P1, arenaBounds); break;
        case 1:  ia = new AI_Defensiva(P2, P1, arenaBounds); break;
        case 2:  ia = new AI_Agresiva(P2, P1, arenaBounds); break;
        default: ia = new AI_Facil(P2, P1, arenaBounds); break;
        }
    }
}

void ControladorCombate::Update()
{
    if (IsGameOver()) return;

    float dt = GetFrameTime();

    P1.Update(dt);

    if (IsKeyPressed(KEY_SPACE)) {
        auto nuevos1 = P1.Shoot();
        Disparos_1.insert(Disparos_1.end(), nuevos1.begin(), nuevos1.end());
    }

    if (ia != nullptr) {
        P2.Update(dt);
        bool dispara = ia->Update(dt, Disparos_1);
        if (dispara) {
            auto nuevos2 = P2.Shoot();
            Disparos_2.insert(Disparos_2.end(), nuevos2.begin(), nuevos2.end());
        }
    }
    else {
        P2.Update(dt);
        if (IsKeyPressed(KEY_RIGHT_CONTROL)) {
            auto nuevos2 = P2.Shoot();
            Disparos_2.insert(Disparos_2.end(), nuevos2.begin(), nuevos2.end());
        }
    }

    for (auto& d : Disparos_1) d.Update(dt, arena.getSueloWidth() / 2, arena.getSueloLength() / 2);
    for (auto& d : Disparos_2) d.Update(dt, arena.getSueloWidth() / 2, arena.getSueloLength() / 2);

    Interacciones::DisparosContraPersonaje(Disparos_1, P1, P2);
    Interacciones::DisparosContraPersonaje(Disparos_2, P2, P1);
    Interacciones::DisparosContraObstaculos(Disparos_1, obstaculos);
    Interacciones::DisparosContraObstaculos(Disparos_2, obstaculos);

    Interacciones::PersonajesContraObstaculos(P1, P2, obstaculos);
    Interacciones::PersonajeContraPersonaje(P1, P2);
    Interacciones::ClampArena(P1, arena);
    Interacciones::ClampArena(P2, arena);
}

void ControladorCombate::Draw()
{
    if (IsGameOver()) {
        DrawVictory();
        return;
    }
    Draw3D();
}

void ControladorCombate::Draw3D()
{
    arena.DrawFondo();

    BeginMode3D(camera);
    arena.Draw(camera);

    BeginShaderMode(alphaDiscard);

    for (auto* obs : obstaculos) obs->Draw(camera);
    P1.Draw(camera);
    P2.Draw(camera);
    for (const auto& d : Disparos_1) d.Draw(camera);
    for (const auto& d : Disparos_2) d.Draw(camera);

    EndShaderMode();
    EndMode3D();

    P1.drawHUD(camera, SKYBLUE);
    P2.drawHUD(camera, RED);
}

void ControladorCombate::DrawVictory()
{
    Draw3D();
    DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), { 0, 0, 0, 160 });

    int W = GetScreenWidth();
    int H = GetScreenHeight();

    const char* msg = (GetWinner() == 1) ? "JUGADOR 1 GANA!" : "JUGADOR 2 GANA!";
    Color       col = (GetWinner() == 1) ? SKYBLUE : RED;

    DrawText(msg, W / 2 - MeasureText(msg, 50) / 2 + 3, H / 2 - 47, 50, BLACK);
    DrawText(msg, W / 2 - MeasureText(msg, 50) / 2, H / 2 - 50, 50, col);

    const char* sub = "PULSA ENTER PARA VOLVER AL TABLERO";
    if ((int)(GetTime() * 2) % 2 == 0)
        DrawText(sub, W / 2 - MeasureText(sub, 22) / 2, H / 2 + 20, 22, YELLOW);
}

ControladorCombate::~ControladorCombate()
{
    UnloadShader(alphaDiscard);
    delete ia;
    for (auto* obs : obstaculos) delete obs;
}
