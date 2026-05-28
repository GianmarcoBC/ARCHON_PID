#pragma once
#include "raylib.h"
#include "Disparo.h"
#include "Pj_info.h"
#include "Vec2.h"

enum class EstadoAnimacion { MOV, STILL, ATK };

class Personaje
{
    Pj_info Player{};
    float max_vida{};
    float cooldown{};
    cntrl Controles{};
    bool  isPlayer{ true };

    Vector3 pos3d{};
    Vec2    l_dir{ 1.0f, 0.0f };

    static constexpr float Size3D    = 2.0f;
    static constexpr float SPEED_SCALE = 0.02f;

    std::vector<Texture2D> Frames_MOV{};
    std::vector<Texture2D> Frames_MOV_shadow{};
    std::vector<Mesh> shadowMesh_MOV{};
    std::vector<Model> shadow_MOV{};

    std::vector<Texture2D> Frames_STILL{};
    std::vector<Texture2D> Frames_STILL_shadow{};
    std::vector<Mesh> shadowMesh_STILL{};
    std::vector<Model> shadow_STILL{};

    std::vector<Texture2D> Frames_ATK{};
    std::vector<Texture2D> Frames_ATK_shadow{};
    std::vector<Mesh> shadowMesh_ATK{};
    std::vector<Model> shadow_ATK{};

    Texture2D Ataque{};
    Sound efecto_ataque{};

    int   frameActual_Mov = 0;
    int   frameActual_Still = 0;
    int   frameActual_Atk = 0;
    float frameTimer  = 0.0f;
    float frameSpeed = 0.1f;
    bool  atkPlaying = false;
    EstadoAnimacion  est{ EstadoAnimacion::STILL };

    friend class Interacciones;

    void drawshadow(Vector3 shadowpos, const std::vector<Model>& shadowFrames, int frameIndex) const;
    void drawAnimation(Camera camera, const std::vector<Texture2D>& frames, int frameIndex) const;

public:
    Personaje() = default;
    Personaje(Pj_info p, cntrl c, Vector3 po, bool ip);

    void Move(Vec2 dir, float dt);
    void Update(float dt);
    void drawHUD(Camera camera, Color color) const;
    void Draw(Camera camera) const;

    Vec2        GetPos()         const { return { pos3d.x, pos3d.z }; }
    void        SetPos(Vec2 p)         { pos3d.x = p.x; pos3d.z = p.y; }
    Vector3     GetPos3D()       const { return pos3d; }
    void        SetPos3D(Vector3 p)    { pos3d = p; }
    float       GetVida()        const { return Player.vida; }
    void        SetVida(float v)       { Player.vida = v; }
    float       GetMaxVida()     const { return max_vida; }
    Vec2        GetDir()         const { return l_dir; }
    void        SetDir(Vec2 d)         { l_dir = d.unitario(); }
    float       GetVelocidad()   const { return Player.vel * SPEED_SCALE; }
    float       get_Cooldown()   const { return Player.cooldown; }
    bool        get_isPlayer()   const { return isPlayer; }
    std::string_view GetNombre() const { return Player.nombre; }
    float       GetRangoMax()    const { return Player.rango_max; }

    void    pain(float damage);
    void    copy(Personaje& other);
    std::vector<Disparo> Shoot();
    void    PlayAttackSound();

    ~Personaje() {
        for (int i = 0; i < Frames_MOV.size(); i++) {
            UnloadModel(shadow_MOV[i]);
            UnloadTexture(Frames_MOV_shadow[i]);
            UnloadTexture(Frames_MOV[i]);
        }
        for (int i = 0; i < Frames_STILL.size(); i++) {
            UnloadModel(shadow_STILL[i]);
            UnloadTexture(Frames_STILL_shadow[i]);
            UnloadTexture(Frames_STILL[i]);
        }
        for (int i = 0; i < Frames_ATK.size(); i++) {
            UnloadModel(shadow_ATK[i]);
            UnloadTexture(Frames_ATK_shadow[i]);
            UnloadTexture(Frames_ATK[i]);
        }
        UnloadTexture(Ataque);
        UnloadSound(efecto_ataque);
    }
};
