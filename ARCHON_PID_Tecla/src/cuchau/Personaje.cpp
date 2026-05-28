#include "Personaje.h"
#include <cassert>
#include <cmath>

// ============================================================================
//  Constructor — Inicializa un personaje con sus recursos graficos y de audio
//
//  Carga los 3 frames de animacion, los 3 frames de sombra, la textura del
//  proyectil y el sonido de ataque. Genera 3 modelos planos para las sombras.
// ============================================================================

Personaje::Personaje(Pj_info p, cntrl c, Vector3 po, bool ip): 
    Player(p),
    Controles(c), 
    pos3d(po),
	max_vida(p.vida),   // Guardar vida maxima antes de que se reduzca
	cooldown(0.0f), // Guardar cooldown maximo antes de que se reduzca
	isPlayer(ip)
{
	//======= MOVIMIENTO =======
    // Cargar los frames de animacion de y sombra del personaje
	for (int i = 0; i < Player.Sprites_MOV.size(); i++) {
		Frames_MOV.push_back(LoadTexture(Player.Sprites_MOV[i].data()));
		Frames_MOV_shadow.push_back(LoadTexture(Player.Sprites_MOV_shadow[i].data()));
	}

    // Generar modelos planos para las sombras (uno por frame de animacion)
    // Cada sombra es un plano de charSize x charSize con la textura de sombra
    for (int i = 0; i < Frames_MOV_shadow.size(); i++) {
        shadowMesh_MOV.push_back(GenMeshPlane(Size3D, Size3D, 1, 1));
        shadow_MOV.push_back(LoadModelFromMesh(shadowMesh_MOV[i]));
        shadow_MOV[i].materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = Frames_MOV_shadow[i];
    }

	//======== QUIETO =======
    // Cargar los frames de animacion de y sombra del personaje
    for (int i = 0; i < Player.Sprites_STILL.size(); i++) {
        Frames_STILL.push_back(LoadTexture(Player.Sprites_STILL[i].data()));
        Frames_STILL_shadow.push_back(LoadTexture(Player.Sprites_STILL_shadow[i].data()));
    }

    // Generar modelos planos para las sombras (uno por frame de animacion)
    // Cada sombra es un plano de charSize x charSize con la textura de sombra
    for (int i = 0; i < Frames_STILL_shadow.size(); i++) {
        shadowMesh_STILL.push_back(GenMeshPlane(Size3D, Size3D, 1, 1));
        shadow_STILL.push_back(LoadModelFromMesh(shadowMesh_STILL[i]));
        shadow_STILL[i].materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = Frames_STILL_shadow[i];
    }

	//======== ATAQUE =======
    // Cargar los frames de animacion de movimiento y sombra del personaje
    for (int i = 0; i < Player.Sprites_ATK.size(); i++) {
        Frames_ATK.push_back(LoadTexture(Player.Sprites_ATK[i].data()));
        Frames_ATK_shadow.push_back(LoadTexture(Player.Sprites_ATK_shadow[i].data()));
    }

    // Generar modelos planos para las sombras (uno por frame de animacion)
    // Cada sombra es un plano de charSize x charSize con la textura de sombra
    for (int i = 0; i < Frames_ATK_shadow.size(); i++) {
        shadowMesh_ATK.push_back(GenMeshPlane(Size3D, Size3D, 1, 1));
        shadow_ATK.push_back(LoadModelFromMesh(shadowMesh_ATK[i]));
        shadow_ATK[i].materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = Frames_ATK_shadow[i];
    }


    // Cargar textura del proyectil y sonido de ataque
    Ataque = LoadTexture(Player.Ataque.data());
    efecto_ataque = LoadSound(Player.Efecto_ataque.data());


}

void Personaje::Move(Vec2 dir, float dt)
{
    float spd = Player.vel * SPEED_SCALE;  // Velocidad escalada a unidades 3D

    switch (est)
    {   
    case EstadoAnimacion::MOV:
        pos3d.x += dir.x * spd * dt;
        pos3d.z += dir.y * spd * dt;
        l_dir = dir.unitario();

        frameTimer += dt;
        if (frameTimer >= frameSpeed) {
            frameTimer = 0.0f;
            frameActual_Mov = (frameActual_Mov + 1) % Frames_MOV.size();
        }
        break;
    case EstadoAnimacion::STILL:
        frameTimer += dt;
        if (frameTimer >= frameSpeed) {
            frameTimer = 0.0f;
            frameActual_Still = (frameActual_Still + 1) % Frames_STILL.size();
        }
        break;
    case EstadoAnimacion::ATK:
        pos3d.x += dir.x * spd * dt;
        pos3d.z += dir.y * spd * dt;
        l_dir = dir.unitario();

        frameTimer += dt;
        if (frameTimer >= frameSpeed) {
            frameTimer = 0.0f;
            frameActual_Atk++;
            if (frameActual_Atk >= (int)Frames_ATK.size()) {
                // Animación terminada → volver al estado que corresponde
                frameActual_Atk = 0;
                atkPlaying = false;
                est = (dir.x != 0 || dir.y != 0)
                    ? EstadoAnimacion::MOV
                    : EstadoAnimacion::STILL;
            }
        }
        break;
    default:
        frameActual_Mov = 0;
        frameActual_Still = 0;
        frameActual_Atk = 0;
        frameTimer = 0.0f;
        break;
    }
}

//  Update — Procesa input de teclado y actualiza animacion
//
//  Solo se ejecuta si isPlayer==true (la IA controla el movimiento externamente).
//  Lee las teclas asignadas, mueve en el plano XZ, actualiza la direccion de
//  apuntado y avanza la animacion de sprites.

void Personaje::Update(float dt)
{
    if (cooldown > 0.0f) cooldown -= dt;  // Reducir cooldown
    else cooldown = 0.0f;
    
    if (!isPlayer) return;   // IA controla externamente

    Vec2 dir = { 0, 0 };
    // Leer teclas y mover en el plano XZ
    // Cada tecla actualiza la direccion de apuntado (l_dir)
    if (IsKeyDown(Controles.left))  { dir.x = -1; }
    if (IsKeyDown(Controles.right)) { dir.x = 1; }
    if (IsKeyDown(Controles.up))    { dir.y = -1; }
    if (IsKeyDown(Controles.down))  { dir.y = 1; }

    // Solo cambia estado si NO hay ataque en curso
    if (!atkPlaying) {
        est = (dir.x != 0 || dir.y != 0)
            ? EstadoAnimacion::MOV
            : EstadoAnimacion::STILL;
    }

    Move(dir, dt);

}

void Personaje::Draw(Camera camera) const
{
    switch (est) {
	case EstadoAnimacion::MOV:
		drawAnimation(camera, Frames_MOV, frameActual_Mov);
        drawshadow({ pos3d.x, 0.01f, pos3d.z - Size3D / 2 }, shadow_MOV, frameActual_Mov);
		break;
	case EstadoAnimacion::STILL:
		drawAnimation(camera, Frames_STILL, frameActual_Still);
		drawshadow({ pos3d.x, 0.01f, pos3d.z - Size3D / 2 }, shadow_STILL, frameActual_Still);
		break;
	case EstadoAnimacion::ATK:
		drawAnimation(camera, Frames_ATK, frameActual_Atk);
		drawshadow({ pos3d.x, 0.01f, pos3d.z - Size3D / 2 }, shadow_ATK, frameActual_Atk);
		break;
    }

}

//  drawshadow — Dibuja la sombra del personaje en el suelo
//
//  Usa BLEND_MULTIPLIED para que la sombra se mezcle con el suelo
//  oscureciendolo sin taparlo completamente.

void Personaje::drawshadow(Vector3 shadowpos, const std::vector<Model>& shadowFrames, int frameIndex) const
{
    BeginBlendMode(BLEND_MULTIPLIED);
    DrawModel(shadowFrames[frameIndex], shadowpos, 1.0f, WHITE);
    EndBlendMode();
}

void Personaje::drawAnimation(Camera camera, const std::vector<Texture2D>& frames, int frameIndex) const
{
    if (frames.empty()) return; // Sin frames, no dibujar
    const Texture2D& texActual = frames[frameIndex]; // Usa el frame actual
    float w = (float)texActual.width;
    float h = (float)texActual.height;
    float srcW = (l_dir.x < 0) ? -w : w;       // Espejo si va a la izquierda

    DrawBillboardPro(
        camera,
        texActual,
        { 0, 0, srcW, h },
        pos3d,
        { 0, 1, 0 },
        { Size3D, Size3D },
        { Size3D / 2, Size3D / 2 },
        0.0f,
        WHITE);
}

//  drawHUD — Dibuja la barra de vida, el cooldown y el nombre del personaje sobre su cabeza
//
//  Proyecta la posicion 3D del personaje a coordenadas 2D de pantalla usando la camara.

void Personaje::drawHUD(Camera camera, Color color) const
{
    // Proyectar posiciones 3D a 2D
    Vector2 sp1 = GetWorldToScreen(pos3d, camera);

    float barW = 60, barH = 6;  // Dimensiones de la barra de vida y del cooldown

    // --- Vida ---
    float hp1 = Player.vida / max_vida;  // Porcentaje de vida
    DrawRectangle((int)(sp1.x - barW / 2), (int)(sp1.y - 50), (int)barW, (int)barH, DARKGRAY);        // Fondo
    DrawRectangle((int)(sp1.x - barW / 2), (int)(sp1.y - 50), (int)(barW * hp1), (int)barH, color);   // Vida actual
    DrawText(TextFormat("%.0f", Player.vida),
        (int)(sp1.x - 16), (int)(sp1.y - 66), 16, WHITE);    // Numero de vida

    // --- Cooldown ---
    float cd1 = cooldown / Player.cooldown;  // Porcentaje de cooldown
    DrawRectangle((int)(sp1.x - barW / 2), (int)(sp1.y - 75), (int)barW, (int)barH, DARKGRAY);        // Fondo
    DrawRectangle((int)(sp1.x - barW / 2), (int)(sp1.y - 75), (int)(barW * cd1), (int)barH, color);   // Cooldown actual
    DrawText(TextFormat("%.1f", cooldown),
        (int)(sp1.x - 16), (int)(sp1.y - 86), 16, WHITE);    // Numero de cooldown

    DrawText(Player.nombre.data(),
        (int)(sp1.x) - MeasureText(Player.nombre.data(), 16) / 2,
        (int)(sp1.y - 100), 16, color);                     // Nombre

}


//  pain — Aplica dano al personaje
//
//  Reduce la vida por la cantidad de dano recibido. La vida no baja de 0.

void Personaje::pain(float damage)
{
    if (Player.vida > damage)
        Player.vida -= damage;
    else
        Player.vida = 0;
}

void Personaje::copy(Personaje& other)
{
	Player.Ataque = other.Player.Ataque;
	Player.attack_speed = other.Player.attack_speed;
	Player.cooldown = other.Player.cooldown;
	Player.Efecto_ataque = other.Player.Efecto_ataque;
	Player.fuerza = other.Player.fuerza;
	Player.rango_max = other.Player.rango_max;
	Player.vel = other.Player.vel;
	Player.vida = other.Player.vida;
	Player.tipoAtaque = other.Player.tipoAtaque;

    max_vida = other.max_vida;
    cooldown = other.cooldown;
    Ataque = other.Ataque;
    efecto_ataque = other.efecto_ataque;
}

//  Shoot — Crea un disparo desde la posicion del personaje
//
//  El proyectil aparece desplazado medio Size3D en la direccion de apuntado
//  (para que salga del borde del billboard, no del centro).
//  La velocidad del proyectil es la direccion de apuntado * velocidad de ataque.

std::vector<Disparo> Personaje::Shoot()
{
    std::vector<Disparo> disparos{};
    float offset = Size3D / 2.0f;

    if (cooldown > 0.0f) return {};

    cooldown = Player.cooldown;
    atkPlaying = true;               // Dispara la animación
    frameActual_Atk = 0;             // Siempre empieza desde el frame 0
    est = EstadoAnimacion::ATK;
    frameTimer = 0.0f;
    if (Player.tipoAtaque == TipoAtaque::Area) {
        const Vec2 dirs[8] = {
            {1,0},{-1,0},{0,1},{0,-1},
            {0.707f,0.707f},{-0.707f,0.707f},
            {0.707f,-0.707f},{-0.707f,-0.707f}
        };
        for (const auto& d : dirs) {
            Vec2    pos2d = GetPos() + d * offset;
            Vector3 origin = { pos2d.x, Size3D / 2.0f, pos2d.y };
            Vec2    vel = d * Player.attack_speed * SPEED_SCALE;
            disparos.push_back(Disparo(origin, vel, &Ataque, isPlayer, Player.rango_max));
        }
    }
    else {
        float rangoMax = (Player.tipoAtaque == TipoAtaque::CuerpoACuerpo) ? Player.rango_max : 0.0f;
        Vec2    pos2d = GetPos() + l_dir * offset;
        Vector3 origin = { pos2d.x, Size3D / 2.0f, pos2d.y };
        Vec2    vel = l_dir * Player.attack_speed * SPEED_SCALE;
        disparos.push_back(Disparo(origin, vel, &Ataque, isPlayer, rangoMax));
    }

    PlayAttackSound();
    return disparos;

}

//  PlayAttackSound — Reproduce el efecto de sonido de ataque

void Personaje::PlayAttackSound()
{
    SetSoundVolume(efecto_ataque, 3.5f);
    PlaySound(efecto_ataque);
}
