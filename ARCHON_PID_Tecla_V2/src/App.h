#pragma once
#include "GameState.h"
#include "Screen.h"
#include "raylib.h"
#include <memory>
#include <unordered_map>

class App {
public:
    void Run();

private:
    GameState gs;
    // Cada estado nuevo tiene su Screen registrado aquí
    std::unordered_map<int, std::unique_ptr<Screen>> screens;

    // Musica global (menu + sala de rolitas)
    Music musicaGlobal{};
    bool  musicaGlobalCargada = false;

    void Init();
    void Update();
    void Draw();
    void HandleInput();
    void HandleMouse();
    bool animSkippedThisFrame = false;  // prevents click from acting after skipping animation
    void IniciarTransicion(Estado destino);
    void FinalizarTransicion();
    void TogglePantallaCompleta();
    Screen* GetCurrentScreen();

    void CambiarCancionGlobal(int idx);   // Cambia la cancion global (idx en canciones[])
    void PausarMusicaGlobal();
    void ReanudarMusicaGlobal();
};
