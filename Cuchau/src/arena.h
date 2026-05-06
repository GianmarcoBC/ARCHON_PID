#pragma once
#include "raylib.h"
#include "Pj.h"
#include "obstaculo.h"

class arena
{
//public:
	// Cámara fija, con vista isométrica
    Camera3D camera{ { 0.0f, 25.0f, 35.0f },
       { 0.0f, 0.0f,  0.0f },
       { 0.0f, 1.0f,  0.0f },
        45.0f, CAMERA_PERSPECTIVE };
    
    const float sueloWidth = 40.0, sueloLength = 25.0; //tamaño de la arena
    const float wallHeight = 2.0, wallWidth = 4.0;  //queda bien

    //Comunicacion con el personaje
    Pj Pers1, Pers2;
    Vector3 charPos1 = { -sueloWidth * 0.45f, Pers1.charSize / 2, -sueloLength * 0.42f };
    Vector3 charPos2 = { sueloWidth * 0.45f, Pers2.charSize / 2, sueloLength * 0.42f };
    //Sombra pie con pie
    Vector3 shadowPosDcho{ charPos2.x, 0.01f, charPos2.z - Pers2.charSize / 2 }; //altura para no fusionarse con suelo
    Vector3 shadowPosIzdo{ charPos1.x, 0.01f,  charPos1.z - Pers1.charSize / 2 };

    //Comunicacion con los obstaculos
    obstaculo fuente{ "fuente.png", "fuente_shadow.png", { 0.0f,   3.5f,  0.0f }, 7 },
        palos1{ "obstaculo.png", "obstaculo_shadow.png", { -10.0f, 1.5f,  6.0f }, 3 },
        palos2{ "obstaculo.png", "obstaculo_shadow.png", {  10.0f, 1.5f,  6.0f }, 3 },
        palos3{ "obstaculo.png", "obstaculo_shadow.png", { -10.0f, 1.5f, -6.0f }, 3 },
        palos4{ "obstaculo.png", "obstaculo_shadow.png", {  10.0f, 1.5f, -6.0f }, 3 };

    Texture2D texBg{};
    Texture2D texPared{};
    Texture2D texSuelo{};
    Texture2D GameOver{};

    Mesh sueloMesh = GenMeshPlane(sueloWidth, sueloLength, 1, 1); //resolucion a 1 en XZ ahorro de recursos
    Model sueloModel = LoadModelFromMesh(sueloMesh);
    Mesh wallMesh = GenMeshPlane(wallWidth, wallHeight, 1, 1);
    Model wallModel = LoadModelFromMesh(wallMesh);

public:
    arena(const char* Bg, const char* Pared, const char* Suelo, Pj p1, Pj p2)
        : Pers1(p1), Pers2(p2)
    {
        texBg = LoadTexture(Bg);
        texPared = LoadTexture(Pared);
        texSuelo = LoadTexture(Suelo);
        sueloModel.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = texSuelo;
        wallModel.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = texPared;
    }

    void DibujaArena();
    void UpdateArena(Pj pers);
    void UnloadArena();
};