#include "raylib.h"

int main() {
    InitWindow(0, 0, "Tablero");
    SetTargetFPS(60);

    // Cámara con ángulo isométrico
    Camera3D camera{ 0 };
    camera.position = { 0.0f, 25.0f, 35.0f }; //vista frontal elevada
    camera.target   = { 0.0f, 0.0f,  0.0f };
    camera.up       = { 0.0f, 1.0f,  0.0f }; //eje y == arriba
    camera.fovy = 45.0f; //no tocar
    camera.projection = CAMERA_PERSPECTIVE;
   
    Texture2D spriteChar   = LoadTexture("SpriteMH_0001.png");
    Texture2D spriteShadow = LoadTexture("SpriteMH_shadow.png");
    Texture2D texPared     = LoadTexture("paredcalida.png");
    Texture2D texBg        = LoadTexture("ciudadnormal2.png");
    Texture2D texSuelo     = LoadTexture("suelo.png");

    //suelo de la arena
    const float sueloWidth = 40.0, sueloLength = 25.0; //tamaño de la arena

    Mesh sueloMesh   = GenMeshPlane(sueloWidth, sueloLength, 1, 1); //resolucion a 1 en XZ ahorro de recursos
    Model sueloModel = LoadModelFromMesh(sueloMesh);
    sueloModel.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = texSuelo;
    
    //frontal de la arena
    const float wallHeight = 2.0, wallWidth = 4.0;  //queda bien

    Mesh wallMesh   = GenMeshPlane(wallWidth, wallHeight, 1, 1);
    Model wallModel = LoadModelFromMesh(wallMesh);
    wallModel.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = texPared;

    //Personajes en la arena
    const float speed = 10.0f, charSize = 4.0f; //tamaño personajes

    Vector3 charPosIzdo = { -sueloWidth * 0.45f, charSize / 2, -sueloLength * 0.42f };
    Vector3 charPosDcho = { sueloWidth * 0.45f, charSize/2, sueloLength * 0.42f };
    
    // Sombra: plano sobre el suelo con la textura aplicada
    Mesh shadowMesh = GenMeshPlane(charSize, charSize, 1, 1); //dependencia del personaje -> Incluir en la clase
    Model shadowModel = LoadModelFromMesh(shadowMesh);
    shadowModel.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = spriteShadow;

    //Codigo de dibujo
    while (!WindowShouldClose()) {
        float dt = GetFrameTime();

        // Movimiento WASD, flechas
        if (IsKeyDown(KEY_W)) charPosIzdo.z -= speed * dt;
        if (IsKeyDown(KEY_S)) charPosIzdo.z += speed * dt;
        if (IsKeyDown(KEY_A)) charPosIzdo.x -= speed * dt;
        if (IsKeyDown(KEY_D)) charPosIzdo.x += speed * dt;

        if (IsKeyDown(KEY_UP)) charPosDcho.z -= speed * dt;
        if (IsKeyDown(KEY_DOWN)) charPosDcho.z += speed * dt;
        if (IsKeyDown(KEY_LEFT)) charPosDcho.x -= speed * dt;
        if (IsKeyDown(KEY_RIGHT)) charPosDcho.x += speed * dt;

        //Hitbox: limites de la arena
        float margin = charSize / 2.0f;
        float limX = sueloWidth / 2 - margin, limZ = sueloLength / 2 - margin;
        if (charPosIzdo.x < -limX) charPosIzdo.x = -limX;
        if (charPosIzdo.x >  limX) charPosIzdo.x =  limX;
        if (charPosIzdo.z < -limZ) charPosIzdo.z = -limZ;
        if (charPosIzdo.z >  limZ) charPosIzdo.z =  limZ;
        if (charPosDcho.x < -limX) charPosDcho.x = -limX;
        if (charPosDcho.x >  limX) charPosDcho.x =  limX;
        if (charPosDcho.z < -limZ) charPosDcho.z = -limZ;
        if (charPosDcho.z >  limZ) charPosDcho.z =  limZ;

        //Sombra pie con pie
        Vector3 shadowPosDcho = { charPosDcho.x, 0.01f, charPosDcho.z - charSize / 2 }; //altura para no fusionarse con suelo
        Vector3 shadowPosIzdo = { charPosIzdo.x, 0.01f, charPosIzdo.z - charSize/2  };
        
        BeginDrawing();
        ClearBackground(BLACK);
        DrawTexturePro(texBg,
            { 0, 0, (float)texBg.width, (float)texBg.height },
            { 0, 0, (float)GetScreenWidth(), (float)GetScreenHeight() }, //el fondo coincide siempre con la pantalla
            { 0, 0 }, 0.0f, WHITE);

        BeginMode3D(camera);
     
        //Plano del suelo
        DrawModel(sueloModel, { 0.0f, 0.0f, 0.0f }, 1.0f, WHITE);

        // Bordes de la arena
        DrawCube({  0.0f, 0.75f,  12.5f }, 40.0f, 1.5f, 1.5f, { 150, 80, 70, 255 });
        DrawCube({  0.0f, 0.75f, -12.5f }, 40.0f, 1.5f, 1.5f, { 150, 80, 70, 255 });
        DrawCube({ -20.0f, 0.75f,  0.0f },  1.5f, 1.5f, 25.0f, { 150, 80, 70, 255 });
        DrawCube({  20.0f, 0.75f,  0.0f },  1.5f, 1.5f, 25.0f, { 150, 80, 70, 255 });

        //almenas en los bordes
        for (int i = 0; i < 10; i++) {   //dientes frente y fondo
            float x = -18.0f + i * 4.0f;
            DrawCube({ x, 2.25f,  12.5f }, 2.0f, 1.5f, 1.5f, { 180, 80, 70, 255 });
            DrawCube({ x, 2.25f, -12.5f }, 2.0f, 1.5f, 1.5f, { 180, 80, 70, 255 });
        }
        for (int i = 0; i < 6; i++) {    //dientes izquierda y derecha
            float z = -10.0f + i * 4.0f;
            DrawCube({ -20.0f, 2.25f, z }, 1.5f, 1.5f, 2.0f, { 180, 80, 70, 255 });
            DrawCube({  20.0f, 2.25f, z }, 1.5f, 1.5f, 2.0f, { 180, 80, 70, 255 });
        }


        //Pared como matriz de texturas
        for (float col = 0; col < 10; col++) {
            for (int row = 0; row < 5; row++) {
                float x = -sueloWidth / 2 + col * wallWidth + wallWidth / 2;
                float y = -(row * wallHeight + wallHeight / 2);
                DrawModelEx(wallModel, { x, y, sueloLength / 2 }, { 1, 0, 0 }, 90.0f, { 1, 1, 1 }, WHITE);
            }
        }

        //Obstaculos
        DrawCube({ -3, 0.5f,  2 }, 1, 1, 1, DARKGRAY);
        DrawCube({  2, 0.5f,  3 }, 1, 1, 1, DARKGRAY);
        DrawCube({  0, 0.5f, -2 }, 1, 1, 1, DARKGRAY);
        DrawCube({ -2, 0.5f, -3 }, 2, 1, 2, MAROON);
        DrawCylinder({ 3, 0, -1 }, 0.5f, 0.5f, 2, 8, BLUE);

        //Sombra plana: modo multiplicativo == píxeles blancos no afectan al suelo
        //píxeles oscuros lo oscurecen (efecto sombra sin necesitar alpha)
        //white == sin tinte, ademas en BlendMode white significa fondo sin cambios
        BeginBlendMode(BLEND_MULTIPLIED);
        DrawModel(shadowModel, shadowPosDcho, 1.0f, WHITE);
        DrawModel(shadowModel, shadowPosIzdo, 1.0f, WHITE);
        EndBlendMode();

        //Personaje como billboard (siempre mira a la cámara)
        DrawBillboard(camera, spriteChar, charPosIzdo, charSize, WHITE);
        DrawBillboard(camera, spriteChar, charPosDcho, charSize, WHITE);
       
        EndMode3D();
        EndDrawing();
    }

    //UnloadModel libera también la textura asignada al material
    UnloadModel(shadowModel);
    UnloadModel(wallModel);
    UnloadModel(sueloModel);

    UnloadTexture(spriteChar);
    UnloadTexture(texPared);
    UnloadTexture(texBg);
    UnloadTexture(texSuelo);

    CloseWindow();
    return 0;
}
