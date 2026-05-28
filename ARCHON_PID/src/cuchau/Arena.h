#pragma once
#include "raylib.h"
#include "obstaculo.h"
#include <vector>

class Arena
{
    static constexpr float sueloWidth = 40.0f;
    static constexpr float sueloLength = 25.0f;
    static constexpr float wallHeight = 2.0f;
    static constexpr float wallWidth = 4.0f;

    Texture2D texBg{};
    Texture2D texPared{};
    Texture2D texSuelo{};

    Mesh  sueloMesh{};
    Model sueloModel{};
    Mesh  wallMesh{};
    Model wallModel{};

    friend class Interacciones;

public:
    Arena();

    float getSueloWidth()  const { return sueloWidth; }
    float getSueloLength() const { return sueloLength; }

    void DrawFondo() const;
    void Draw(Camera camera) const;

    ~Arena() {
        UnloadTexture(texBg);
        UnloadTexture(texPared);
        UnloadTexture(texSuelo);
        UnloadModel(sueloModel);
        UnloadModel(wallModel);
    }
};
