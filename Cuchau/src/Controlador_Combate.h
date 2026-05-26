#pragma once
#include "raylib.h"
#include "Personaje.h"
#include "Arena.h"
#include "AI_Facil.h"
#include "AI_Defensiva.h"
#include "AI_Agresiva.h"
#include "Interacciones.h"
#include "Disparo.h"
#include "SaveManager.h"
#include <vector>

//  arena.h — Arena 3D + orquestador del combate
//
//  Combina el renderizado 3D de Cuchau (camara isometrica, suelo, paredes,
//  billboards con painter's algorithm, sombras) con la logica de combate de
//  ARCHON (movimiento, disparos, IA, colisiones, victoria).
//
//  La arena posee:
//    - 2 Personajes (P1 y P2) con rendering 3D y logica de juego
//    - 5 obstaculos con billboard + sombra + colision
//    - IA opcional para P2
//    - Sistema de colisiones (Interacciones)
//    - Vectores de disparos por jugador
//    - Pipeline de renderizado: fondo → 3D (suelo, paredes, bordes, sombras,
//      billboards ordenados por profundidad)

class ControladorCombate
{
    // --- Camara isometrica fija ---
    // Posicion elevada (0, 25, 35) mirando al origen, perspectiva 45°
    Camera3D camera = { { 0.0f, 25.0f, 35.0f },    // position
                        { 0.0f, 0.0f,  0.0f },      // target
                        { 0.0f, 1.0f,  0.0f },      // up
                        45.0f, CAMERA_PERSPECTIVE };

	// --- Texturas y modelos de la arena ---
    Arena arena;    

    // --- Personajes ---
    Personaje P1, P2;

    // --- Obstaculos (creados en heap porque necesitan ventana OpenGL activa) ---
    std::vector<obstaculo*> obstaculos{};    // Vector de punteros para iterar facilmente

    // --- Combate ---
    CombatAI*      ia = nullptr;       // IA para P2 (nullptr si es modo 2 jugadores)
    std::vector<Disparo> Disparos_1{}; // Disparos del jugador 1
    std::vector<Disparo> Disparos_2{}; // Disparos del jugador 2 (o IA)

    // --- Shader de alpha discard (descarta pixeles transparentes del depth buffer) ---
    Shader alphaDiscard{};

    // --- Metodos privados de dibujo ---

    // Renderiza todo el escenario 3D: fondo, suelo, paredes, bordes, sombras, billboards
    void Draw3D();

    // Dibuja la pantalla de victoria con la arena congelada de fondo
    void DrawVictory();
    
	friend class Interacciones; // Interacciones necesita acceso a personajes, disparos y obstaculos para resolver colisiones
public:

    // Constructor: crea la arena con dos personajes, opcionalmente con IA
    //   pj1/pj2: datos de los personajes seleccionados
    //   vsIA: true para modo VS IA, false para 2 jugadores
    //   dificultad: 0=facil, 1=normal, 2=dificil (solo si vsIA=true)
    ControladorCombate(Pj_info pj1, Pj_info pj2, bool vsIA, int dificultad);

    // Actualiza la logica de combate: movimiento, disparos, IA, colisiones
    void Update();

    // Dibuja la arena (combate activo) o la pantalla de victoria
    void Draw();

    // Devuelve true si alguno de los dos personajes tiene vida = 0
    bool IsGameOver() const { return P1.GetVida() == 0 || P2.GetVida() == 0; }

    // Devuelve 0=en curso, 1=P1 gana (P2 muere), 2=P2 gana (P1 muere)
    int  GetWinner()  const {
        if (P2.GetVida() == 0) return 1;
        if (P1.GetVida() == 0) return 2;
        return 0;
    }

    // Nombres de los personajes (para la musica)
    std::string_view GetP1Name() const { return P1.GetNombre(); }
    std::string_view GetP2Name() const { return P2.GetNombre(); }

    // Guardado / Carga de estado
    bool GuardarEstado(bool modoIA, int dificultad) const;
    void CargarEstado(const SaveData& d);

    // Destructor: libera IA, personajes, obstaculos, modelos y texturas
    ~ControladorCombate();
};
