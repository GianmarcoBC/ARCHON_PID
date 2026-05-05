#pragma once
#include "raylib.h"
#include "Vec2.h"

class Disparo
{
    Texture2D* Disp; //Referencia a la textura del disparo dependiendo de quién lo haga
    Vec2 pos{};
    Vec2 vel{};
    bool status_{ false }; // Bala activa o no
    bool fromPlayer{ true }; // Indica si el disparo es del jugador o de la CPU
public:

    //Constructor
    Disparo(Vec2 p, Vec2 v, Texture2D* tx, bool fp = true) {
        pos = p; 
        vel = v;
        Disp = tx; 
        status_ = true; // Activa el disparo al crearlo
        fromPlayer = fp; // Indica si el disparo es del jugador o de la CPU
    }

    //Actualizaciones del disparo
    void Update(float dt);
    void Draw();


    //Getters y setters
    void setStatus(bool s) { status_ = s;}
    bool getStatus()const { return status_; }
    void setFromPlayer(bool fp) { fromPlayer = fp; }
    bool isFromPlayer()const { return fromPlayer; }
    Vec2 GetPos()const { return pos; }
    Vec2 GetVel()const { return vel; } // Asumiendo que la velocidad es solo en el eje x


    ~Disparo() {
        // No es necesario descargar la textura aquí, ya que es compartida y se maneja en el arquero
    }
};

