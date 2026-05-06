#pragma once
#include "raylib.h"

class obstaculo
{
public:
    Texture2D sprite{};
    Texture2D spriteShadow{};
    Mesh shadowMesh{};
    Model shadow{};
    //Rectangle hitbox;
    Vector3 pos{};
    Vector3 shadowpos{};
    float size{};

    void drawshadow();
    void UnloadObstaculo();

    obstaculo(const char* textura, const char* sombra, Vector3 posicion, float tam)
    {
        pos = posicion;
        size = tam;
        shadowpos = { posicion.x, 0.01f, posicion.z - tam / 1.5f };
        sprite = LoadTexture(textura);
        spriteShadow = LoadTexture(sombra);

        float aspect = (float)sprite.width / (float)sprite.height;
        shadowMesh = GenMeshPlane(tam * aspect, 1.5f*tam, 1, 1);
        shadow = LoadModelFromMesh(shadowMesh);
        shadow.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = spriteShadow;
    }
};