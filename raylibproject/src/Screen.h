#pragma once
#include "GameState.h"

// ============================================================
// SCREEN — clase base abstracta para todas las pantallas
// Cada pantalla es responsable de su propio dibujado, input
// y actualización. App las despacha por polimorfismo.
// ============================================================
class Screen {
public:
    virtual ~Screen() = default;

    // Dibuja la pantalla completa (fondo + UI)
    virtual void Draw(GameState& gs)       = 0;

    // Procesa input de teclado
    virtual void HandleInput(GameState& gs) = 0;

    // Procesa input de ratón
    virtual void HandleMouse(GameState& gs) = 0;

    // Actualiza animaciones propias de la pantalla
    virtual void Update(GameState& gs) {}

    // Llamado cuando se entra en esta pantalla
    virtual void OnEnter(GameState& gs)    {}

protected:
    // Helpers comunes a todas las pantallas
    // (accesibles desde cualquier subclase)
    static float yFlip(float y) { return 600.0f - (float)GetMouseY() * 600.0f / GetScreenHeight(); }
    static float mx()           { return (float)GetMouseX() * 800.0f / GetScreenWidth(); }
    static float my()           { return 600.0f - (float)GetMouseY() * 600.0f / GetScreenHeight(); }
};
