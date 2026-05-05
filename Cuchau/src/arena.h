#pragma once
#include "raylib.h"
#include "Pj.h"

class arena
{
public:
    Camera3D camera{ { 0.0f, 25.0f, 35.0f },
       { 0.0f, 0.0f,  0.0f },
       { 0.0f, 1.0f,  0.0f },
        45.0f, CAMERA_PERSPECTIVE };
    
    const float sueloWidth = 40.0, sueloLength = 25.0; //tamaño de la arena
    const float wallHeight = 2.0, wallWidth = 4.0;  //queda bien

    //Comunicacion con el personaje
    Pj character;
    Pj::charSize;
    Vector3 charPos1 = { -sueloWidth * 0.45f, character.charSize / 2, -sueloLength * 0.42f };
    Vector3 charPos2 = { sueloWidth * 0.45f, character.charSize / 2, sueloLength * 0.42f };

    Texture2D texPared{};
    Texture2D texBg{};
    Texture2D texSuelo{};

    Mesh sueloMesh = GenMeshPlane(sueloWidth, sueloLength, 1, 1); //resolucion a 1 en XZ ahorro de recursos
    Model sueloModel = LoadModelFromMesh(sueloMesh);
    Mesh wallMesh = GenMeshPlane(wallWidth, wallHeight, 1, 1);
    Model wallModel = LoadModelFromMesh(wallMesh);
   
//public:
    arena(const char* Pared, const char* Bg, const char* Suelo)
    {
        texPared = LoadTexture(Pared);
        texBg = LoadTexture(Bg);
        texSuelo = LoadTexture(Suelo);
        sueloModel.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = texSuelo;
        wallModel.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = texPared;
    }

    void DibujaArena();
};