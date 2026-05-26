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

    // Cargar los frames de animacion y sombra del personaje
	for (int i = 0; i < Player.Sprites.size(); i++) {
		Frames.push_back(LoadTexture(Player.Sprites[i].data()));
		Frames_shadow.push_back(LoadTexture(Player.Sprites_shadow[i].data()));
	}

    // Cargar textura del proyectil y sonido de ataque
    Ataque = LoadTexture(Player.Ataque.data());
    efecto_ataque = LoadSound(Player.Efecto_ataque.data());

    // Generar modelos planos para las sombras (uno por frame de animacion)
    // Cada sombra es un plano de charSize x charSize con la textura de sombra
    for (int i = 0; i < Frames_shadow.size(); i++) {
        shadowMesh.push_back(GenMeshPlane(Size3D, Size3D, 1, 1));
        shadow.push_back(LoadModelFromMesh(shadowMesh[i]));
        shadow[i].materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = Frames_shadow[i];
    }
}

void Personaje::Move(Vec2 dir, float dt)
{
    moviendose = (dir.x != 0 || dir.y != 0);
    float spd = Player.vel * SPEED_SCALE;  // Velocidad escalada a unidades 3D

    if (moviendose) {
        pos3d.x += dir.x * spd * dt;
        pos3d.z += dir.y * spd * dt;
        l_dir = dir.unitario();

        frameTimer += dt;
        if (frameTimer >= Player.frameSpeed) {
            frameTimer = 0.0f;
            frameActual = (frameActual + 1) % Frames.size();
        }
    }
    else {
        frameActual = 0;
        frameTimer = 0.0f;
    }
}

//  Update — Procesa input de teclado y actualiza animacion
//
//  Solo se ejecuta si isPlayer==true (la IA controla el movimiento externamente).
//  Lee las teclas asignadas, mueve en el plano XZ, actualiza la direccion de
//  apuntado y avanza la animacion de sprites.

void Personaje::Update(float dt)
{
    if (!isPlayer) return;   // IA controla externamente
    Vec2 dir = { 0, 0 };
    // Leer teclas y mover en el plano XZ
    // Cada tecla actualiza la direccion de apuntado (l_dir)
    if (IsKeyDown(Controles.left))  { dir.x = -1; }
    if (IsKeyDown(Controles.right)) { dir.x = 1; }
    if (IsKeyDown(Controles.up))    { dir.y = -1; }
    if (IsKeyDown(Controles.down))  { dir.y = 1; }

    Move(dir, dt);
	if (cooldown > 0.0f) cooldown -= dt;  // Reducir cooldown
	else cooldown = 0.0f;
}

void Personaje::Draw(Camera camera) const
{
    const Texture2D& texActual = Frames[frameActual]; // Usa el frame actual
    float w = (float)texActual.width;
    float h = (float)texActual.height;
    float srcW = (l_dir.x < 0) ? -w : w;       // Espejo si va a la izquierda

    drawshadow({ pos3d.x, 0.01f, pos3d.z - Size3D / 2 });

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

//  drawshadow — Dibuja la sombra del personaje en el suelo
//
//  Usa BLEND_MULTIPLIED para que la sombra se mezcle con el suelo
//  oscureciendolo sin taparlo completamente.

void Personaje::drawshadow(Vector3 shadowpos) const
{
    BeginBlendMode(BLEND_MULTIPLIED);
    DrawModel(shadow[frameActual], shadowpos, 1.0f, WHITE);
    EndBlendMode();
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

//  Shoot — Crea un disparo desde la posicion del personaje
//
//  El proyectil aparece desplazado medio Size3D en la direccion de apuntado
//  (para que salga del borde del billboard, no del centro).
//  La velocidad del proyectil es la direccion de apuntado * velocidad de ataque.

std::vector<Disparo> Personaje::Shoot()
{
    std::vector<Disparo> disparos{};
    float offset = Size3D / 2.0f;

    if (cooldown <= 0.0f) {
        cooldown = Player.cooldown;
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
        return disparos;

    }
}

//  PlayAttackSound — Reproduce el efecto de sonido de ataque

void Personaje::PlayAttackSound()
{
    SetSoundVolume(efecto_ataque, 3.5f);
    PlaySound(efecto_ataque);
}
