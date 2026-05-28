#pragma once
#include "raylib.h"

class obstaculo
{
    Texture2D sprite{};
    Texture2D spriteShadow{};
    Mesh shadowMesh{};
    Model shadow{};
    Vector3 pos{};
    Vector3 shadowpos{};
    float size{};
    float colCX{}, colCZ{};
    float colHW{}, colHL{};
    bool  solido{ true };

    friend class Interacciones;

    void drawshadow() const;

public:
    obstaculo() = default;

    obstaculo(const char* textura, const char* sombra, Vector3 posicion,
        float tam, float colW = 0, float colL = 0, bool sol = true) :
        pos(posicion), size(tam), solido(sol),
        shadowpos(posicion.x, 0.01f, posicion.z - tam / 1.5f)
    {
        sprite = LoadTexture(textura);
        spriteShadow = LoadTexture(sombra);
        float aspect = (float)sprite.width / (float)sprite.height;
        shadowMesh = GenMeshPlane(tam * aspect, 1.5f * tam, 1, 1);
        shadow = LoadModelFromMesh(shadowMesh);
        shadow.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = spriteShadow;
        colCX = posicion.x;
        colCZ = posicion.z;
        colHW = colW;
        colHL = colL;
    }

    ~obstaculo() {
        UnloadModel(shadow);
        UnloadTexture(sprite);
        UnloadTexture(spriteShadow);
    }

    void Draw(Camera camera) const;

    float getColCX() const { return colCX; }
    float getColCZ() const { return colCZ; }
    float getColHW() const { return colHW; }
    float getColHL() const { return colHL; }
    bool  isSolido() const { return solido; }
};
