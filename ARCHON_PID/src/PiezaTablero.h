#pragma once
#include "raylib.h"
#include "TiposPj.h"
#include "Pj.h"
#include "PjMapping.h"
#include <vector>
#include <string>
#include <cmath>

/*
 * PiezaTablero - Representa una pieza/personaje en el tablero 9x9
 *
 * Contiene los datos necesarios para el tablero estrategico:
 * posicion (fila/columna), equipo, tipo, rango de movimiento, etc.
 *
 * Los sprites Still se cargan como secuencia animada (igual que en combate)
 * junto con sus sombras correspondientes.
 */
class PiezaTablero
{
    // --- Animacion Still (mismos frames que usa combate) ---
    std::vector<Texture2D> Frames_STILL{};
    std::vector<Texture2D> Frames_RES{};         // Frames resaltados (al seleccionar pieza)
    std::vector<Texture2D> Frames_STILL_shadow{};
    std::vector<Mesh>  shadowMesh_STILL{};
    std::vector<Model> shadow_STILL{};

    // --- Animacion Mov (movimiento entre casillas) ---
    std::vector<Texture2D> Frames_MOV{};
    std::vector<Texture2D> Frames_MOV_shadow{};
    std::vector<Mesh>  shadowMesh_MOV{};
    std::vector<Model> shadow_MOV{};
    int   frameActual_Mov = 0;
    float frameTimer_Mov  = 0.0f;
    float frameSpeed_Mov  = 0.15f;

    bool seleccionado{ false };                   // Si esta pieza esta seleccionada en el tablero
    int   frameActual = 0;
    float frameTimer  = 0.0f;
    float frameSpeed  = 0.35f;   // Mas lento que en combate (0.1) para el tablero

    // --- Estado de movimiento animado ---
    bool enMovimiento{ false };
    float movX{ 0.0f }, movZ{ 0.0f };           // Posicion mundo interpolada durante movimiento
    float movOrigenX{ 0.0f }, movOrigenZ{ 0.0f };
    float movDestinoX{ 0.0f }, movDestinoZ{ 0.0f };
    float movProgreso{ 0.0f };                   // 0.0 = origen, 1.0 = destino
    static constexpr float movVelocidad = 3.0f;  // Velocidad de interpolacion (unidades/segundo)

    Vector2 pos{};              // Posicion en pixeles (calculada al dibujar en 2D)
    float vida;                 // Vida actual (se restaura al morir para el cementerio)
    float vidaMax;              // Vida maxima
    int fila, columna;          // Posicion en la cuadricula 9x9 (fila=Y, columna=X)
    bool team;                  // LUZ (false/0) o OSCURIDAD (true/1)
    tipo_pj ID;                 // Tipo de personaje (para identificar en combate y hechizos)
    unsigned int rangoTablero;  // Alcance de movimiento en casillas
    bool vuela;                 // Si es true, se mueve en area; si false, solo en cruz
    bool imprison{ false };     // Aprisionado por hechizo Imprison (no puede moverse)

    static constexpr float pieceSize3D = 2.4f; // Tamanio del billboard y sombra en unidades 3D

public:
    PiezaTablero(PjBoard p, int ifila, int icolumna)
    {
        vida = 100.0f;
        vidaMax = 100.0f;
        fila = ifila;
        columna = icolumna;
        team = p.team;
        ID = p.ID;
        rangoTablero = p.rangoTablero;
        vuela = p.vuela;

        // Cargar frames Still, Res y sombras desde Pj_info
        const Pj_info& info = getCombatInfo(ID);
        for (int i = 0; i < (int)info.Sprites_STILL.size(); i++) {
            Frames_STILL.push_back(LoadTexture(info.Sprites_STILL[i].data()));
            Frames_STILL_shadow.push_back(LoadTexture(info.Sprites_STILL_shadow[i].data()));

            // Derivar ruta Res: reemplazar "Still" por "Res" en la ruta del sprite
            std::string resPath(info.Sprites_STILL[i].data());
            size_t pos = resPath.find("Still");
            if (pos != std::string::npos) resPath.replace(pos, 5, "Res");
            Frames_RES.push_back(LoadTexture(resPath.c_str()));
        }
        for (int i = 0; i < (int)Frames_STILL_shadow.size(); i++) {
            shadowMesh_STILL.push_back(GenMeshPlane(pieceSize3D, pieceSize3D, 1, 1));
            // Invertir UVs horizontalmente para el equipo oscuro
            if (team) {
                float* tc = (float*)shadowMesh_STILL[i].texcoords;
                for (int v = 0; v < shadowMesh_STILL[i].vertexCount; v++)
                    tc[v * 2] = 1.0f - tc[v * 2];
                UpdateMeshBuffer(shadowMesh_STILL[i], 1, tc, shadowMesh_STILL[i].vertexCount * 2 * sizeof(float), 0);
            }
            shadow_STILL.push_back(LoadModelFromMesh(shadowMesh_STILL[i]));
            shadow_STILL[i].materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = Frames_STILL_shadow[i];
        }

        // Cargar frames Mov y sombras
        for (int i = 0; i < (int)info.Sprites_MOV.size(); i++) {
            Frames_MOV.push_back(LoadTexture(info.Sprites_MOV[i].data()));
            Frames_MOV_shadow.push_back(LoadTexture(info.Sprites_MOV_shadow[i].data()));
        }
        for (int i = 0; i < (int)Frames_MOV_shadow.size(); i++) {
            shadowMesh_MOV.push_back(GenMeshPlane(pieceSize3D, pieceSize3D, 1, 1));
            if (team) {
                float* tc = (float*)shadowMesh_MOV[i].texcoords;
                for (int v = 0; v < shadowMesh_MOV[i].vertexCount; v++)
                    tc[v * 2] = 1.0f - tc[v * 2];
                UpdateMeshBuffer(shadowMesh_MOV[i], 1, tc, shadowMesh_MOV[i].vertexCount * 2 * sizeof(float), 0);
            }
            shadow_MOV.push_back(LoadModelFromMesh(shadowMesh_MOV[i]));
            shadow_MOV[i].materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = Frames_MOV_shadow[i];
        }

        // Desfasar animacion: cada pieza arranca en un frame y timer distintos
        if (!Frames_STILL.empty()) {
            int offset = ifila * 9 + icolumna;
            frameActual = offset % (int)Frames_STILL.size();
            frameTimer = fmodf(offset * 0.07f, frameSpeed);
        }
    }

    // Constructor para piezas temporales con equipo dinámico (elemental)
    PiezaTablero(PjBoard p, int ifila, int icolumna, equipo eq)
        : PiezaTablero(p, ifila, icolumna)   // delega en el constructor base
    {
        team = (eq == OSCURIDAD);            // sobreescribe el equipo tras la carga
    }

    // --- Getters y setters ---
    void set_fila_columna(int fil, int col) { fila = fil; columna = col; }
    int get_fila() { return fila; }
    int get_columna() { return columna; }
    bool get_equipo() { return team; }
    tipo_pj get_ID() { return ID; }
    unsigned int get_rangoTablero() { return rangoTablero; }
    bool get_vuela() { return vuela; }
    void heal() { vida = vidaMax; }
    void set_imprison(bool aux) { imprison = aux; }
    bool get_imprison() { return imprison; }
    void set_seleccionado(bool s) { seleccionado = s; }
    bool get_seleccionado() { return seleccionado; }
    bool get_enMovimiento() { return enMovimiento; }

    // Inicia animacion de movimiento desde casilla actual hasta destino
    void iniciarMovimiento(int filaDestino, int colDestino, float cellSize);

    // Actualiza interpolacion del movimiento. Devuelve true cuando termina.
    bool UpdateMovimiento(float dt);

    // Avanza la animacion Still (llamar cada frame desde la logica)
    void UpdateAnimacion(float dt);

    // Dibuja la pieza en el tablero 2D, escalada a 64x64 (usado para cementerio)
    void DrawT(const int paramx, const int paramy);

    // Dibuja la pieza como billboard 3D animado en el tablero
    void Draw3D(Camera camera, float cellSize);

    // Dibuja la sombra animada de la pieza en el suelo
    void DrawShadow3D(float cellSize);

    ~PiezaTablero() {
        for (int i = 0; i < (int)Frames_STILL.size(); i++) {
            UnloadTexture(Frames_STILL[i]);
            UnloadTexture(Frames_RES[i]);
            UnloadTexture(Frames_STILL_shadow[i]);
            UnloadModel(shadow_STILL[i]);
        }
        for (int i = 0; i < (int)Frames_MOV.size(); i++) {
            UnloadTexture(Frames_MOV[i]);
            UnloadTexture(Frames_MOV_shadow[i]);
            UnloadModel(shadow_MOV[i]);
        }
    }
};
