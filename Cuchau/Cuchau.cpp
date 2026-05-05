#include "raylib.h"
#include "arena.h"

int main() {
    InitWindow(0, 0, "Tablero");
    SetTargetFPS(60);

    arena miarena("paredcalida.png", "ciudadnormal2.png", "suelo.png");
    Pj pj("SpriteMH_0001.png", "SpriteMH_shadow.png");
      
    //Codigo de dibujo
    while (!WindowShouldClose()) {
        float dt = GetFrameTime();

        // Movimiento WASD, flechas
        if (IsKeyDown(KEY_W)) miarena.charPos1.z -= pj.speed * dt;
        if (IsKeyDown(KEY_S)) miarena.charPos1.z += pj.speed * dt;
        if (IsKeyDown(KEY_A)) miarena.charPos1.x -= pj.speed * dt;
        if (IsKeyDown(KEY_D)) miarena.charPos1.x += pj.speed * dt;

        if (IsKeyDown(KEY_UP)) miarena.charPos2.z -= pj.speed * dt;
        if (IsKeyDown(KEY_DOWN)) miarena.charPos2.z += pj.speed * dt;
        if (IsKeyDown(KEY_LEFT)) miarena.charPos2.x -= pj.speed * dt;
        if (IsKeyDown(KEY_RIGHT)) miarena.charPos2.x += pj.speed * dt;

        //Hitbox: limites de la arena
        // Clamp con margen para que no se salga la mitad del personaje
        float margin = pj.charSize / 2.0f;
        float limX = miarena.sueloWidth / 2 - margin, limZ = miarena.sueloLength / 2 - margin;
        if (miarena.charPos1.x < -limX) miarena.charPos1.x = -limX;
        if (miarena.charPos1.x >  limX) miarena.charPos1.x =  limX;
        if (miarena.charPos1.z < -limZ) miarena.charPos1.z = -limZ;
        if (miarena.charPos1.z >  limZ) miarena.charPos1.z =  limZ;
        if (miarena.charPos2.x < -limX) miarena.charPos2.x = -limX;
        if (miarena.charPos2.x >  limX) miarena.charPos2.x =  limX;
        if (miarena.charPos2.z < -limZ) miarena.charPos2.z = -limZ;
        if (miarena.charPos2.z >  limZ) miarena.charPos2.z =  limZ;

        //Sombra pie con pie
        Vector3 shadowPosDcho = { miarena.charPos2.x, 0.01f, miarena.charPos2.z - pj.charSize / 2 }; //altura para no fusionarse con suelo
        Vector3 shadowPosIzdo = { miarena.charPos1.x, 0.01f,  miarena.charPos1.z - pj.charSize/2  };
        
        BeginDrawing();
        ClearBackground(BLACK);
        miarena.DibujaArena();

        //Sombra plana: modo multiplicativo == píxeles blancos no afectan al suelo
        //píxeles oscuros lo oscurecen (efecto sombra sin necesitar alpha)
        //white == sin tinte, ademas en BlendMode white significa fondo sin cambios
        BeginBlendMode(BLEND_MULTIPLIED);
        DrawModel(pj.shadow, shadowPosDcho, 1.0f, WHITE);
        DrawModel(pj.shadow, shadowPosIzdo, 1.0f, WHITE);
        EndBlendMode();

        //Personaje como billboard: dibujar de más lejos a más cerca para evitar que el depth buffer tape al más lejano
        
            Vector3 cam = miarena.camera.position;
			// Evita solapamiento de personajes: dibujar primero el más lejano
            auto dist2 = [&](Vector3 p) {
                float dx = p.x - cam.x, dy = p.y - cam.y, dz = p.z - cam.z;
                return dx*dx + dy*dy + dz*dz;
            };
            if (dist2(miarena.charPos1) >= dist2(miarena.charPos2)) {
                DrawBillboard(miarena.camera, pj.spriteChar, miarena.charPos1, pj.charSize, WHITE);
                DrawBillboard(miarena.camera, pj.spriteChar, miarena.charPos2, pj.charSize, WHITE);
            } else {
                DrawBillboard(miarena.camera, pj.spriteChar, miarena.charPos2, pj.charSize, WHITE);
                DrawBillboard(miarena.camera, pj.spriteChar, miarena.charPos1, pj.charSize, WHITE);
            }
        

        EndMode3D();
        EndDrawing();
    }

    //UnloadModel libera también la textura asignada al material
    UnloadModel(pj.shadow);
    UnloadModel(miarena.wallModel);
    UnloadModel(miarena.sueloModel);

    UnloadTexture(pj.spriteChar);
    UnloadTexture(miarena.texPared);
    UnloadTexture(miarena.texBg);
    UnloadTexture(miarena.texSuelo);

    CloseWindow();
    return 0;
}
