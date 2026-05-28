#pragma once
#include "Personaje.h"
#include "Disparo.h"
#include "Vec2.h"
#include "raylib.h"
#include <cmath>
#include <cstdlib>
#include <vector>
#include "Arena.h"

class CombatAI {
protected:
    enum Estado { Acercar, Huir, Esquivar };

    struct Accion {
        Vec2 d;
        bool disparar;
    };

    Personaje& Jugador;
    Personaje& IA;
    Rectangle  arena;

    float distancia_combate_optima;
    float rango_seguro;
    float rango_esquiva;
    float aimbot;

    Estado estado;
    float  sentido_giro;
    float  cont_giro;
    float  cambio_giro;

    Accion ultima_accion;
    float  cooldown_IA;
    float  delay_IA;

    float Pared_cerc(Vec2 pos) const;
    bool  debe_esquivar(const std::vector<Disparo>& disparos, Vec2& outDir);
    virtual Accion decide(const std::vector<Disparo>& disparos, float dt) = 0;

    CombatAI(Personaje& IA, Personaje& Jugador, Rectangle arena);

public:
    virtual ~CombatAI() = default;
    bool Update(float dt, const std::vector<Disparo>& disparos);
};
