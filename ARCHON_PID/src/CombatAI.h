// CombatAI.h
#pragma once
#include "Personaje.h"
#include "Disparo.h"
#include "Vec2.h"
#include "raylib.h"
#include <cmath>
#include <cstdlib>
#include <vector>

class CombatAI {

    enum Estado { Acercar, Rodear, Esquivar };

    struct Accion { // Acción de movimiento y disparo calculada por la IA.
        Vec2 d;
        bool disparar;
    };

    //Referencias
    Personaje& Jugador;
    Personaje& IA;
    Rectangle  arena;

    //Umbrales derivados
    float distancia_combate_optima; //Distancia a la que la IA se siente cómoda.
    float rango_seguro; // Margen adicional para decidir acercarse o alejarse.
    float rango_esquiva; // Distancia máxima al eje del disparo para intentar esquivar.
    float aimbot; // Umbral de puntería para decidir disparar.

    //Estado
    Estado estado;
    float sentido_giro; //Sentido actual de giro para rodear (1 o -1). Cambia periódicamente para ser impredecible.
    float cont_giro; // Contador para cambiar el sentido de giro
    float cambio_giro; // Intervalo aleatorio para cambiar el sentido de giro (en frames)

    //Buffer de reacción
    Accion ultima_accion; // Acción calculada más recientemente
    float  cooldown_IA; // Tiempo restante tras ejecutar la última acción antes de recalcular. Simula un delay de reacción.
    float  delay_IA; // Configurable: frames de delay entre decisiones

    //Helpers
    float Pared_cerc(Vec2 pos) const; // Calcula la distancia a la pared más cercana.
    void evitar_pared(Vec2& v, Vec2 pos, Vec2 lim_x, Vec2 lim_y, float margin); // Ajusta un valor de dirección para alejarse de los bordes del arena.

    //Detección y esquiva de proyectiles 
    bool debe_esquivar(const std::vector<Disparo>& disparos, Vec2& outDir);
    

    //Lógica de decisión
    Accion decide(const std::vector<Disparo>& disparos);

public:
    //Constructor
    CombatAI(Personaje& IA, Personaje& Jugador, Rectangle arena)
        : IA(IA), Jugador(Jugador), arena(arena),
        estado(Acercar), sentido_giro(1.0f), cont_giro(0.0f),
        ultima_accion{ 0, 0, false }, cooldown_IA(0.0f), delay_IA(0.0f)
    {
        float lado_corto = fminf(arena.width, arena.height);
        distancia_combate_optima = lado_corto * 0.38f;  //Escalable con la arena.
        rango_seguro = distancia_combate_optima * 0.25f;
        rango_esquiva = 30.0f;        //Depende del Sprite.
        //Patrones predefinidos.
        aimbot = 0.93f;   
        cambio_giro = 60.0f * (1.8f + (rand() % 100) / 72.0f);
        delay_IA = 60.0f * 0.12f;
    }

    //Llamar una vez por frame.
    //Devuelve true si la IA debe disparar este frame.
    bool Update(float dt, const std::vector<Disparo>& disparos);

    void SetReactionDelay(float frames) { delay_IA = frames; }
};