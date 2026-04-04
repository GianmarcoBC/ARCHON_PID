#pragma once
#include "raylib.h"

class Disparo
{
    Texture2D* Disp; //Referencia a la textura del disparo dependiendo de quién lo haga
    Vector2 pos{};
    Vector2 vel{};
    bool status_{ false }; // Bala activa o no
public:

    //Constructor
    Disparo(Vector2 p, Vector2 v, Texture2D* tx) { 
        pos = p; 
        vel = v;
        Disp = tx; 
        status_ = true; // Activa el disparo al crearlo
    }

    //Actualizaciones del disparo
    void Update();
    void Draw();

    //Estado del disparo
    bool Status()const { return status_; }

    ~Disparo() {
        // No es necesario descargar la textura aquí, ya que es compartida y se maneja en el arquero
    }
};

