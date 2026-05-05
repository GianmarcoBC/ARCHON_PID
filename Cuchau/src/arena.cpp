#include "arena.h"

void arena::DibujaArena()
{
    DrawTexturePro(texBg,
        { 0, 0, (float)texBg.width, (float)texBg.height },
        { 0, 0, (float)GetScreenWidth(), (float)GetScreenHeight() }, //el fondo coincide siempre con la pantalla
        { 0, 0 }, 0.0f, WHITE);

    BeginMode3D(camera);
    //Plano del suelo
    DrawModel(sueloModel, { 0.0f, 0.0f, 0.0f }, 1.0f, WHITE);

    //Pared como matriz de texturas
    for (float col = 0; col < 10; col++) {
        for (int row = 0; row < 5; row++) {
            float x = -sueloWidth / 2 + col * wallWidth + wallWidth / 2;
            float y = -(row * wallHeight + wallHeight / 2);
            DrawModelEx(wallModel, { x, y, sueloLength / 2 }, { 1, 0, 0 }, 90.0f, { 1, 1, 1 }, WHITE);
        }
    }

    //Bordes de la arena
    DrawCube({ 0.0f, 0.75f,  12.5f }, 40.0f, 1.5f, 1.5f, { 150, 80, 70, 255 });
    DrawCube({ 0.0f, 0.75f, -12.5f }, 40.0f, 1.5f, 1.5f, { 150, 80, 70, 255 });
    DrawCube({ -20.0f, 0.75f,  0.0f }, 1.5f, 1.5f, 25.0f, { 150, 80, 70, 255 });
    DrawCube({ 20.0f, 0.75f,  0.0f }, 1.5f, 1.5f, 25.0f, { 150, 80, 70, 255 });
    //almenas en los bordes
    for (int i = 0; i < 10; i++) {   //dientes frente y fondo
        float x = -18.0f + i * 4.0f;
        DrawCube({ x, 2.25f,  12.5f }, 2.0f, 1.5f, 1.5f, { 180, 80, 70, 255 });
        DrawCube({ x, 2.25f, -12.5f }, 2.0f, 1.5f, 1.5f, { 180, 80, 70, 255 });
    }
    for (int i = 0; i < 6; i++) {    //dientes izquierda y derecha
        float z = -10.0f + i * 4.0f;
        DrawCube({ -20.0f, 2.25f, z }, 1.5f, 1.5f, 2.0f, { 180, 80, 70, 255 });
        DrawCube({ 20.0f, 2.25f, z }, 1.5f, 1.5f, 2.0f, { 180, 80, 70, 255 });
    }

    //Obstaculos
    DrawCube({ -3, 0.5f,  2 }, 1, 1, 1, DARKGRAY);
    DrawCube({ 2, 0.5f,  3 }, 1, 1, 1, DARKGRAY);
    DrawCube({ 0, 0.5f, -2 }, 1, 1, 1, DARKGRAY);
    DrawCube({ -2, 0.5f, -3 }, 2, 1, 2, MAROON);
    DrawCylinder({ 3, 0, -1 }, 0.5f, 0.5f, 2, 8, BLUE);
}