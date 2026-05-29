#include "Personaje.h"
#include <cassert>
#include <cmath>

Personaje::Personaje(Pj_info p, cntrl c, Vector3 po, bool ip):
    Player(p), Controles(c), pos3d(po),
    max_vida(p.vida), cooldown(0.0f), isPlayer(ip)
{
    for (int i = 0; i < Player.Sprites_MOV.size(); i++) {
        Frames_MOV.push_back(LoadTexture(Player.Sprites_MOV[i].data()));
        Frames_MOV_shadow.push_back(LoadTexture(Player.Sprites_MOV_shadow[i].data()));
    }
    for (int i = 0; i < (int)Frames_MOV_shadow.size(); i++) {
        shadowMesh_MOV.push_back(GenMeshPlane(Size3D, Size3D, 1, 1));
        shadow_MOV.push_back(LoadModelFromMesh(shadowMesh_MOV[i]));
        shadow_MOV[i].materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = Frames_MOV_shadow[i];
        // Version flipped (UVs invertidas en X)
        shadowMeshFlip_MOV.push_back(GenMeshPlane(Size3D, Size3D, 1, 1));
        float* tc = (float*)shadowMeshFlip_MOV[i].texcoords;
        for (int v = 0; v < shadowMeshFlip_MOV[i].vertexCount; v++) tc[v * 2] = 1.0f - tc[v * 2];
        UpdateMeshBuffer(shadowMeshFlip_MOV[i], 1, tc, shadowMeshFlip_MOV[i].vertexCount * 2 * sizeof(float), 0);
        shadowFlip_MOV.push_back(LoadModelFromMesh(shadowMeshFlip_MOV[i]));
        shadowFlip_MOV[i].materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = Frames_MOV_shadow[i];
    }

    for (int i = 0; i < (int)Player.Sprites_STILL.size(); i++) {
        Frames_STILL.push_back(LoadTexture(Player.Sprites_STILL[i].data()));
        Frames_STILL_shadow.push_back(LoadTexture(Player.Sprites_STILL_shadow[i].data()));
    }
    for (int i = 0; i < (int)Frames_STILL_shadow.size(); i++) {
        shadowMesh_STILL.push_back(GenMeshPlane(Size3D, Size3D, 1, 1));
        shadow_STILL.push_back(LoadModelFromMesh(shadowMesh_STILL[i]));
        shadow_STILL[i].materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = Frames_STILL_shadow[i];
        shadowMeshFlip_STILL.push_back(GenMeshPlane(Size3D, Size3D, 1, 1));
        float* tc = (float*)shadowMeshFlip_STILL[i].texcoords;
        for (int v = 0; v < shadowMeshFlip_STILL[i].vertexCount; v++) tc[v * 2] = 1.0f - tc[v * 2];
        UpdateMeshBuffer(shadowMeshFlip_STILL[i], 1, tc, shadowMeshFlip_STILL[i].vertexCount * 2 * sizeof(float), 0);
        shadowFlip_STILL.push_back(LoadModelFromMesh(shadowMeshFlip_STILL[i]));
        shadowFlip_STILL[i].materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = Frames_STILL_shadow[i];
    }

    for (int i = 0; i < (int)Player.Sprites_ATK.size(); i++) {
        Frames_ATK.push_back(LoadTexture(Player.Sprites_ATK[i].data()));
        Frames_ATK_shadow.push_back(LoadTexture(Player.Sprites_ATK_shadow[i].data()));
    }
    for (int i = 0; i < (int)Frames_ATK_shadow.size(); i++) {
        shadowMesh_ATK.push_back(GenMeshPlane(Size3D, Size3D, 1, 1));
        shadow_ATK.push_back(LoadModelFromMesh(shadowMesh_ATK[i]));
        shadow_ATK[i].materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = Frames_ATK_shadow[i];
        shadowMeshFlip_ATK.push_back(GenMeshPlane(Size3D, Size3D, 1, 1));
        float* tc = (float*)shadowMeshFlip_ATK[i].texcoords;
        for (int v = 0; v < shadowMeshFlip_ATK[i].vertexCount; v++) tc[v * 2] = 1.0f - tc[v * 2];
        UpdateMeshBuffer(shadowMeshFlip_ATK[i], 1, tc, shadowMeshFlip_ATK[i].vertexCount * 2 * sizeof(float), 0);
        shadowFlip_ATK.push_back(LoadModelFromMesh(shadowMeshFlip_ATK[i]));
        shadowFlip_ATK[i].materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = Frames_ATK_shadow[i];
    }

    Ataque = LoadTexture(Player.Ataque.data());
    efecto_ataque = LoadSound(Player.Efecto_ataque.data());
}

void Personaje::Move(Vec2 dir, float dt)
{
    float spd = Player.vel * SPEED_SCALE;

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

void Personaje::Update(float dt)
{
    if (cooldown > 0.0f) cooldown -= dt;
    else cooldown = 0.0f;

    if (!isPlayer) return;

    Vec2 dir = { 0, 0 };
    if (IsKeyDown(Controles.left))  { dir.x = -1; }
    if (IsKeyDown(Controles.right)) { dir.x = 1; }
    if (IsKeyDown(Controles.up))    { dir.y = -1; }
    if (IsKeyDown(Controles.down))  { dir.y = 1; }

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
        drawshadow({ pos3d.x, 0.01f, pos3d.z - Size3D / 2 }, shadow_MOV, shadowFlip_MOV, frameActual_Mov);
        break;
    case EstadoAnimacion::STILL:
        drawAnimation(camera, Frames_STILL, frameActual_Still);
        drawshadow({ pos3d.x, 0.01f, pos3d.z - Size3D / 2 }, shadow_STILL, shadowFlip_STILL, frameActual_Still);
        break;
    case EstadoAnimacion::ATK:
        drawAnimation(camera, Frames_ATK, frameActual_Atk);
        drawshadow({ pos3d.x, 0.01f, pos3d.z - Size3D / 2 }, shadow_ATK, shadowFlip_ATK, frameActual_Atk);
        break;
    }
}

void Personaje::drawshadow(Vector3 shadowpos, const std::vector<Model>& shadowFrames,
                           const std::vector<Model>& shadowFramesFlip, int frameIndex) const
{
    BeginBlendMode(BLEND_MULTIPLIED);
    if (l_dir.x < 0) {
        DrawModel(shadowFramesFlip[frameIndex], shadowpos, 1.0f, WHITE);
    } else {
        DrawModel(shadowFrames[frameIndex], shadowpos, 1.0f, WHITE);
    }
    EndBlendMode();
}

void Personaje::drawAnimation(Camera camera, const std::vector<Texture2D>& frames, int frameIndex) const
{
    if (frames.empty()) return;
    const Texture2D& texActual = frames[frameIndex];
    float w = (float)texActual.width;
    float h = (float)texActual.height;
    float srcW = (l_dir.x < 0) ? -w : w;

    DrawBillboardPro(camera, texActual, { 0, 0, srcW, h }, pos3d,
        { 0, 1, 0 }, { Size3D, Size3D }, { Size3D / 2, Size3D / 2 }, 0.0f, WHITE);
}

void Personaje::drawHUD(Camera camera, Color color) const
{
    Vector2 sp1 = GetWorldToScreen(pos3d, camera);
    float barW = 60, barH = 6;

    float hp1 = Player.vida / max_vida;
    DrawRectangle((int)(sp1.x - barW / 2), (int)(sp1.y - 50), (int)barW, (int)barH, DARKGRAY);
    DrawRectangle((int)(sp1.x - barW / 2), (int)(sp1.y - 50), (int)(barW * hp1), (int)barH, color);
    DrawText(TextFormat("%.0f", Player.vida), (int)(sp1.x - 16), (int)(sp1.y - 66), 16, WHITE);

    float cd1 = cooldown / Player.cooldown;
    DrawRectangle((int)(sp1.x - barW / 2), (int)(sp1.y - 75), (int)barW, (int)barH, DARKGRAY);
    DrawRectangle((int)(sp1.x - barW / 2), (int)(sp1.y - 75), (int)(barW * cd1), (int)barH, color);
    DrawText(TextFormat("%.1f", cooldown), (int)(sp1.x - 16), (int)(sp1.y - 86), 16, WHITE);

    DrawText(Player.nombre.data(),
        (int)(sp1.x) - MeasureText(Player.nombre.data(), 16) / 2,
        (int)(sp1.y - 100), 16, color);
}

void Personaje::pain(float damage)
{
    if (Player.vida > damage) Player.vida -= damage;
    else Player.vida = 0;
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

std::vector<Disparo> Personaje::Shoot()
{
    std::vector<Disparo> disparos{};
    float offset = Size3D / 2.0f;

    if (cooldown > 0.0f) return {};

    cooldown = Player.cooldown;
    atkPlaying = true;
    frameActual_Atk = 0;
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

void Personaje::PlayAttackSound()
{
    SetSoundVolume(efecto_ataque, 3.5f);
    PlaySound(efecto_ataque);
}
