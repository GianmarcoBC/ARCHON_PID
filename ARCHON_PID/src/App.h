#pragma once
#include "GameState.h"
#include "Screen.h"
#include <memory>
#include <unordered_map>

class App {
public:
    void Run();

private:
    GameState gs;
    // Cada estado nuevo tiene su Screen registrado aquí
    std::unordered_map<int, std::unique_ptr<Screen>> screens;

    void Init();
    void Update();
    void Draw();
    void HandleInput();
    void HandleMouse();
    void IniciarTransicion(Estado destino);
    void TogglePantallaCompleta();
    Screen* GetCurrentScreen();
};
