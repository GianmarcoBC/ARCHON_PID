#include "Arena.h"

namespace Archon_PID {

    Arena::Arena()
    {
        texBg = LoadTexture("bin/Resources/AAGraficos/ciudadnormal2.png");     // Fondo 2D (ciudad)
        texPared = LoadTexture("bin/Resources/AAGraficos/paredcalida.png");       // Textura de los paneles de la pared trasera
        texSuelo = LoadTexture("bin/Resources/AAGraficos/suelo.png");             // Textura del suelo
        texGameOver = LoadTexture("bin/Resources/AAGraficos/the-dark-background-minimalism-tmnt-teenage-mutant-ninja-turtles-wallpaper-preview.png");

        // --- Generar geometria del suelo ---
        sueloMesh = GenMeshPlane(sueloWidth, sueloLength, 1, 1);  // Plano de 40x25 unidades
        sueloModel = LoadModelFromMesh(sueloMesh);
        sueloModel.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = texSuelo;

        // --- Generar geometria de la pared (un panel individual) ---
        wallMesh = GenMeshPlane(wallWidth, wallHeight, 1, 1);  // Plano de 4x2 unidades
        wallModel = LoadModelFromMesh(wallMesh);
        wallModel.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = texPared;
    }

    void Arena::DrawFondo() const
    {
        // 1. Fondo 2D (dibujado antes de Mode3D, cubre toda la pantalla)
        DrawTexturePro(texBg,
            { 0, 0, (float)texBg.width, (float)texBg.height },
            { 0, 0, (float)GetScreenWidth(), (float)GetScreenHeight() },
            { 0, 0 }, 0.0f, WHITE);
    }

    void Arena::Draw(Camera camera) const
    {
        // 2. Suelo texturizado
        DrawModel(sueloModel, { 0.0f, 0.0f, 0.0f }, 1.0f, WHITE);

        // 3. Pared trasera: muro de texturas (10 columnas x 6 filas de paneles)
        //    Cada panel es un plano rotado 90° en X para que quede vertical
        for (float col = 0; col < 10; col++) {
            for (int row = 0; row < 6; row++) {
                float x = -sueloWidth / 2 + col * wallWidth + wallWidth / 2;
                float y = -(row * wallHeight + wallHeight / 2);
                DrawModelEx(wallModel, { x, y, sueloLength / 2 },
                    { 1, 0, 0 }, 90.0f, { 1, 1, 1 }, WHITE);
            }
        }

        // 4. Bordes de la arena (cubos marrones en los 4 lados)
        DrawCube({ 0.0f, 0.75f,  12.5f }, 40.0f, 1.5f, 1.5f, { 150, 80, 70, 255 });  // Borde trasero
        DrawCube({ 0.0f, 0.75f, -12.5f }, 40.0f, 1.5f, 1.5f, { 150, 80, 70, 255 });  // Borde frontal
        DrawCube({ -20.0f, 0.75f,  0.0f }, 1.5f, 1.5f, 25.0f, { 150, 80, 70, 255 }); // Borde izquierdo
        DrawCube({ 20.0f, 0.75f,  0.0f }, 1.5f, 1.5f, 25.0f, { 150, 80, 70, 255 }); // Borde derecho

        // 5. Almenas decorativas encima de los bordes (cubos mas claros)
        for (int i = 0; i < 10; i++) {
            float x = -18.0f + i * 4.0f;
            DrawCube({ x, 2.25f,  12.5f }, 2.0f, 1.5f, 1.5f, { 180, 80, 70, 255 });  // Almenas traseras
            DrawCube({ x, 2.25f, -12.5f }, 2.0f, 1.5f, 1.5f, { 180, 80, 70, 255 });  // Almenas frontales
        }
        for (int i = 0; i < 6; i++) {
            float z = -10.0f + i * 4.0f;
            DrawCube({ -20.0f, 2.25f, z }, 1.5f, 1.5f, 2.0f, { 180, 80, 70, 255 }); // Almenas izquierdas
            DrawCube({ 20.0f, 2.25f, z }, 1.5f, 1.5f, 2.0f, { 180, 80, 70, 255 }); // Almenas derechas
        }

    }

}