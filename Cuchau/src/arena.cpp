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
        for (int row = 0; row < 6; row++) {
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

    Pers1.drawshadow(shadowPosDcho);
    Pers2.drawshadow(shadowPosIzdo);

    fuente.drawshadow();
    palos1.drawshadow();
    palos2.drawshadow();
    palos3.drawshadow();
    palos4.drawshadow();
    // Painter's algorithm: todos los billboards ordenados de lejos a cerca
    struct Billboard { Texture2D tex; Vector3 pos; float size; };

    Vector3 cam = camera.position;
    auto dist2 = [&](Vector3 p) {
        float dx = p.x - cam.x, dy = p.y - cam.y, dz = p.z - cam.z;
        return dx * dx + dy * dy + dz * dz;
    };

    Billboard billboards[] = {
        { fuente.sprite, fuente.pos, fuente.size },
        { palos1.sprite, palos1.pos, palos1.size },
        { palos2.sprite, palos2.pos, palos2.size },
        { palos3.sprite, palos3.pos, palos3.size },
        { palos4.sprite, palos4.pos, palos4.size },
        { Pers1.spriteChar, charPos1, Pers1.charSize },
        { Pers2.spriteChar, charPos2, Pers2.charSize },
    };

    // Insertion sort de lejos a cerca
    int n = sizeof(billboards) / sizeof(billboards[0]);
    for (int i = 1; i < n; i++) {
        Billboard key = billboards[i];
        int j = i - 1;
        while (j >= 0 && dist2(billboards[j].pos) < dist2(key.pos)) {
            billboards[j + 1] = billboards[j];
            j--;
        }
        billboards[j + 1] = key;
    }

    for (const auto& b : billboards)
        DrawBillboard(camera, b.tex, b.pos, b.size, WHITE);
    }

void arena::UpdateArena(Pj pers)
{
    float dt = GetFrameTime();

    // Movimiento WASD, flechas
    if (IsKeyDown(KEY_W)) charPos1.z -= pers.speed * dt;
    if (IsKeyDown(KEY_S)) charPos1.z += pers.speed * dt;
    if (IsKeyDown(KEY_A)) charPos1.x -= pers.speed * dt;
    if (IsKeyDown(KEY_D)) charPos1.x += pers.speed * dt;

    if (IsKeyDown(KEY_UP)) charPos2.z -= pers.speed * dt;
    if (IsKeyDown(KEY_DOWN)) charPos2.z += pers.speed * dt;
    if (IsKeyDown(KEY_LEFT)) charPos2.x -= pers.speed * dt;
    if (IsKeyDown(KEY_RIGHT)) charPos2.x += pers.speed * dt;

    //Hitbox: limites de la arena
    // Clamp con margen para que no se salga la mitad del personaje
    float margin = pers.charSize / 2.0f;
    float limX = sueloWidth / 2 - margin, limZ = sueloLength / 2 - margin;
    if (charPos1.x < -limX) charPos1.x = -limX;
    if (charPos1.x > limX) charPos1.x = limX;
    if (charPos1.z < -limZ) charPos1.z = -limZ;
    if (charPos1.z > limZ) charPos1.z = limZ;
    if (charPos2.x < -limX) charPos2.x = -limX;
    if (charPos2.x > limX) charPos2.x = limX;
    if (charPos2.z < -limZ) charPos2.z = -limZ;
    if (charPos2.z > limZ) charPos2.z = limZ;

    shadowPosDcho = { charPos2.x, 0.01f, charPos2.z - Pers2.charSize / 2 };//altura para no fusionarse con suelo
    shadowPosIzdo = { charPos1.x, 0.01f,  charPos1.z - Pers1.charSize / 2 };

}
void arena::UnloadArena()
{
    Pers1.UnloadPj();
    Pers2.UnloadPj();

    fuente.UnloadObstaculo();
    palos1.UnloadObstaculo();
    palos2.UnloadObstaculo();
    palos3.UnloadObstaculo();
    palos4.UnloadObstaculo();

    UnloadModel(wallModel);
    UnloadModel(sueloModel);
 
    UnloadTexture(texPared);
    UnloadTexture(texBg);
    UnloadTexture(texSuelo);
}