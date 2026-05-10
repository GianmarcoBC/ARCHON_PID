#pragma once
#include "GameState.h"

class App {
public:
    void Run();

private:
    GameState gs;

    void Init();
    void Update();
    void Draw();
    void HandleInput();
    void HandleMouse();
    void IniciarTransicion(Estado destino);
    void TogglePantallaCompleta();
};
