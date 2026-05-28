#include "Arena.h"

Arena::Arena()
{
    texBg = LoadTexture("Resources/AAGraficos/ciudadnormal2.png");
    texPared = LoadTexture("Resources/AAGraficos/paredcalida.png");
    texSuelo = LoadTexture("Resources/AAGraficos/suelo.png");

    sueloMesh = GenMeshPlane(sueloWidth, sueloLength, 1, 1);
    sueloModel = LoadModelFromMesh(sueloMesh);
    sueloModel.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = texSuelo;

    wallMesh = GenMeshPlane(wallWidth, wallHeight, 1, 1);
    wallModel = LoadModelFromMesh(wallMesh);
    wallModel.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = texPared;
}

void Arena::DrawFondo() const
{
    DrawTexturePro(texBg,
        { 0, 0, (float)texBg.width, (float)texBg.height },
        { 0, 0, (float)GetScreenWidth(), (float)GetScreenHeight() },
        { 0, 0 }, 0.0f, WHITE);
}

void Arena::Draw(Camera camera) const
{
    DrawModel(sueloModel, { 0.0f, 0.0f, 0.0f }, 1.0f, WHITE);

    for (float col = 0; col < 10; col++) {
        for (int row = 0; row < 6; row++) {
            float x = -sueloWidth / 2 + col * wallWidth + wallWidth / 2;
            float y = -(row * wallHeight + wallHeight / 2);
            DrawModelEx(wallModel, { x, y, sueloLength / 2 },
                { 1, 0, 0 }, 90.0f, { 1, 1, 1 }, WHITE);
        }
    }

    DrawCube({ 0.0f, 0.75f,  12.5f }, 40.0f, 1.5f, 1.5f, { 150, 80, 70, 255 });
    DrawCube({ 0.0f, 0.75f, -12.5f }, 40.0f, 1.5f, 1.5f, { 150, 80, 70, 255 });
    DrawCube({ -20.0f, 0.75f,  0.0f }, 1.5f, 1.5f, 25.0f, { 150, 80, 70, 255 });
    DrawCube({ 20.0f, 0.75f,  0.0f }, 1.5f, 1.5f, 25.0f, { 150, 80, 70, 255 });

    for (int i = 0; i < 10; i++) {
        float x = -18.0f + i * 4.0f;
        DrawCube({ x, 2.25f,  12.5f }, 2.0f, 1.5f, 1.5f, { 180, 80, 70, 255 });
        DrawCube({ x, 2.25f, -12.5f }, 2.0f, 1.5f, 1.5f, { 180, 80, 70, 255 });
    }
    for (int i = 0; i < 6; i++) {
        float z = -10.0f + i * 4.0f;
        DrawCube({ -20.0f, 2.25f, z }, 1.5f, 1.5f, 2.0f, { 180, 80, 70, 255 });
        DrawCube({ 20.0f, 2.25f, z }, 1.5f, 1.5f, 2.0f, { 180, 80, 70, 255 });
    }
}
