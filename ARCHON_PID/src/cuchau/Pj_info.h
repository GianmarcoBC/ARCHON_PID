#pragma once
#include <string_view>
#include <vector>

// Constantes de balance
inline constexpr float Velocidad_media = 500.0f, Velocidad_baja = 200.0f;
inline constexpr float Vida_muyalta = 2000.0f, Vida_alta = 1000.0f, Vida_media = 500.0f, Vida_baja = 200.0f;
inline constexpr float Fuerza_muyalta = 80.0f, Fuerza_alta = 50.0f, Fuerza_media = 10.0f, Fuerza_baja = 5.0f;
inline constexpr float Cooldown_alto = 5.0f, Cooldown_medio = 3.0f, Cooldown_bajo = 1.0f, Cooldown_nulo = 0.25f;
inline constexpr float AttackSpeed_muyalto = 1000.0f, AttackSpeed_alto = 800.0f, AttackSpeed_medio = 500.0f, AttackSpeed_bajo = 200.0f;
inline constexpr float Rango_inf = 0.0f, Rango_medio = 6.0f, Rango_bajo = 2.0f;

struct cntrl { int up, down, left, right; };

enum class TipoAtaque { Rango, CuerpoACuerpo, Area };

struct Pj_info
{
    std::string_view nombre;
    std::vector<std::string_view> Sprites_MOV;
    std::vector<std::string_view> Sprites_MOV_shadow;
    std::vector<std::string_view> Sprites_STILL;
    std::vector<std::string_view> Sprites_STILL_shadow;
    std::vector<std::string_view> Sprites_ATK;
    std::vector<std::string_view> Sprites_ATK_shadow;
    std::string_view Ataque;
    std::string_view Efecto_ataque;
    float vel;
    float vida;
    float fuerza;
    float cooldown;
    float attack_speed;
    float rango_max;
    TipoAtaque tipoAtaque;
    bool vuela;
};

// === EQUIPO CLARO ===

inline const Pj_info PjC_MH = {
    "MH",
    {"Resources/MH/SpriteMHMov_0001.png", "Resources/MH/SpriteMHMov_0002.png"},
    {"Resources/MH/SpriteMHMov_0001shadow.png", "Resources/MH/SpriteMHMov_0002shadow.png"},
    {"Resources/MH/SpriteMHStill_0001.png", "Resources/MH/SpriteMHStill_0002.png", "Resources/MH/SpriteMHStill_0003.png", "Resources/MH/SpriteMHStill_0004.png", "Resources/MH/SpriteMHStill_0005.png"},
    {"Resources/MH/SpriteMHStill_0001shadow.png", "Resources/MH/SpriteMHStill_0002shadow.png", "Resources/MH/SpriteMHStill_0003shadow.png", "Resources/MH/SpriteMHStill_0004shadow.png", "Resources/MH/SpriteMHStill_0005shadow.png"},
    {"Resources/MH/SpriteMHATK_0001.png", "Resources/MH/SpriteMHATK_0002.png", "Resources/MH/SpriteMHATK_0003.png"},
    {"Resources/MH/SpriteMHATK_0001shadow.png", "Resources/MH/SpriteMHATK_0002shadow.png", "Resources/MH/SpriteMHATK_0003shadow.png"},
    "Resources/MH/MHATK.png", "Resources/MH/MHFX.wav",
    Velocidad_media, Vida_media, Fuerza_alta, Cooldown_medio, AttackSpeed_medio, Rango_inf, TipoAtaque::Rango, true
};

inline const Pj_info PjC_Phoenix = {
    "Phoenix",
    {"Resources/Phoenix/SpritePhoenixMov_0001.png", "Resources/Phoenix/SpritePhoenixMov_0002.png", "Resources/Phoenix/SpritePhoenixMov_0003.png"},
    {"Resources/Phoenix/SpritePhoenixMov_0001shadow.png", "Resources/Phoenix/SpritePhoenixMov_0002shadow.png", "Resources/Phoenix/SpritePhoenixMov_0003shadow.png"},
    {"Resources/Phoenix/SpritePhoenixStill_0001.png", "Resources/Phoenix/SpritePhoenixStill_0002.png", "Resources/Phoenix/SpritePhoenixStill_0003.png"},
    {"Resources/Phoenix/SpritePhoenixStill_0001shadow.png", "Resources/Phoenix/SpritePhoenixStill_0002shadow.png", "Resources/Phoenix/SpritePhoenixStill_0003shadow.png"},
    {"Resources/Phoenix/SpritePhoenixATK_0001.png", "Resources/Phoenix/SpritePhoenixATK_0002.png", "Resources/Phoenix/SpritePhoenixATK_0003.png", "Resources/Phoenix/SpritePhoenixATK_0004.png"},
    {"Resources/Phoenix/SpritePhoenixATK_0001shadow.png", "Resources/Phoenix/SpritePhoenixATK_0002shadow.png", "Resources/Phoenix/SpritePhoenixATK_0003shadow.png", "Resources/Phoenix/SpritePhoenixATK_0004shadow.png"},
    "Resources/Phoenix/PhoenixATK.png", "Resources/Phoenix/PhoenixFX.wav",
    Velocidad_media, Vida_alta, Fuerza_alta, Cooldown_alto, AttackSpeed_bajo, Rango_medio, TipoAtaque::Area, true
};

inline const Pj_info PjC_Golem = {
    "Golem",
    {"Resources/Golem/SpriteGolemMov_0001.png", "Resources/Golem/SpriteGolemMov_0002.png"},
    {"Resources/Golem/SpriteGolemMov_0001shadow.png", "Resources/Golem/SpriteGolemMov_0002shadow.png"},
    {"Resources/Golem/SpriteGolemStill_0001.png", "Resources/Golem/SpriteGolemStill_0002.png"},
    {"Resources/Golem/SpriteGolemStill_0001shadow.png", "Resources/Golem/SpriteGolemStill_0002shadow.png"},
    {"Resources/Golem/SpriteGolemATK_0001.png", "Resources/Golem/SpriteGolemATK_0002.png", "Resources/Golem/SpriteGolemATK_0003.png", "Resources/Golem/SpriteGolemATK_0004.png", "Resources/Golem/SpriteGolemATK_0005.png", "Resources/Golem/SpriteGolemATK_0006.png"},
    {"Resources/Golem/SpriteGolemATK_0001shadow.png", "Resources/Golem/SpriteGolemATK_0002shadow.png", "Resources/Golem/SpriteGolemATK_0003shadow.png", "Resources/Golem/SpriteGolemATK_0004shadow.png", "Resources/Golem/SpriteGolemATK_0005shadow.png", "Resources/Golem/SpriteGolemATK_0006shadow.png"},
    "Resources/Golem/GolemATK.png", "Resources/Golem/GolemFX.wav",
    Velocidad_baja, Vida_alta, Fuerza_alta, Cooldown_alto, AttackSpeed_bajo, Rango_inf, TipoAtaque::Rango, false
};

inline const Pj_info PjC_Djinni = {
    "Djinni",
    {"Resources/Djinni/SpriteDjinniMov_0001.png", "Resources/Djinni/SpriteDjinniMov_0002.png", "Resources/Djinni/SpriteDjinniMov_0003.png"},
    {"Resources/Djinni/SpriteDjinniMov_0001shadow.png", "Resources/Djinni/SpriteDjinniMov_0002shadow.png", "Resources/Djinni/SpriteDjinniMov_0003shadow.png"},
    {"Resources/Djinni/SpriteDjinniStill_0001.png", "Resources/Djinni/SpriteDjinniStill_0002.png", "Resources/Djinni/SpriteDjinniStill_0003.png"},
    {"Resources/Djinni/SpriteDjinniStill_0001shadow.png", "Resources/Djinni/SpriteDjinniStill_0002shadow.png", "Resources/Djinni/SpriteDjinniStill_0003shadow.png"},
    {"Resources/Djinni/SpriteDjinniATK_0001.png", "Resources/Djinni/SpriteDjinniATK_0002.png", "Resources/Djinni/SpriteDjinniATK_0003.png", "Resources/Djinni/SpriteDjinniATK_0004.png"},
    {"Resources/Djinni/SpriteDjinniATK_0001shadow.png", "Resources/Djinni/SpriteDjinniATK_0002shadow.png", "Resources/Djinni/SpriteDjinniATK_0003shadow.png", "Resources/Djinni/SpriteDjinniATK_0004shadow.png"},
    "Resources/Djinni/DjinniATK.png", "Resources/Djinni/DjinniFX.wav",
    Velocidad_media, Vida_alta, Fuerza_media, Cooldown_medio, AttackSpeed_medio, Rango_inf, TipoAtaque::Rango, true
};

inline const Pj_info PjC_Unicorn = {
    "Unicorn",
    {"Resources/Unicorn/SpriteUnicornMov_0001.png", "Resources/Unicorn/SpriteUnicornMov_0002.png"},
    {"Resources/Unicorn/SpriteUnicornMov_0001shadow.png", "Resources/Unicorn/SpriteUnicornMov_0002shadow.png"},
    {"Resources/Unicorn/SpriteUnicornStill_0001.png", "Resources/Unicorn/SpriteUnicornStill_0002.png", "Resources/Unicorn/SpriteUnicornStill_0003.png", "Resources/Unicorn/SpriteUnicornStill_0004.png"},
    {"Resources/Unicorn/SpriteUnicornStill_0001shadow.png", "Resources/Unicorn/SpriteUnicornStill_0002shadow.png", "Resources/Unicorn/SpriteUnicornStill_0003shadow.png", "Resources/Unicorn/SpriteUnicornStill_0004shadow.png"},
    {"Resources/Unicorn/SpriteUnicornATK_0001.png", "Resources/Unicorn/SpriteUnicornATK_0002.png", "Resources/Unicorn/SpriteUnicornATK_0003.png", "Resources/Unicorn/SpriteUnicornATK_0004.png"},
    {"Resources/Unicorn/SpriteUnicornATK_0001shadow.png", "Resources/Unicorn/SpriteUnicornATK_0002shadow.png", "Resources/Unicorn/SpriteUnicornATK_0003shadow.png", "Resources/Unicorn/SpriteUnicornATK_0004shadow.png"},
    "Resources/Unicorn/UnicornATK.png", "Resources/Unicorn/UnicornFX.wav",
    Velocidad_media, Vida_media, Fuerza_media, Cooldown_bajo, AttackSpeed_alto, Rango_inf, TipoAtaque::Rango, false
};

inline const Pj_info PjC_Valkyrie = {
    "Valkyrie",
    {"Resources/Valkyrie/SpriteValkyrieMov_0001.png", "Resources/Valkyrie/SpriteValkyrieMov_0002.png"},
    {"Resources/Valkyrie/SpriteValkyrieMov_0001shadow.png", "Resources/Valkyrie/SpriteValkyrieMov_0002shadow.png"},
    {"Resources/Valkyrie/SpriteValkyrieStill_0001.png", "Resources/Valkyrie/SpriteValkyrieStill_0002.png", "Resources/Valkyrie/SpriteValkyrieStill_0003.png", "Resources/Valkyrie/SpriteValkyrieStill_0004.png"},
    {"Resources/Valkyrie/SpriteValkyrieStill_0001shadow.png", "Resources/Valkyrie/SpriteValkyrieStill_0002shadow.png", "Resources/Valkyrie/SpriteValkyrieStill_0003shadow.png", "Resources/Valkyrie/SpriteValkyrieStill_0004shadow.png"},
    {"Resources/Valkyrie/SpriteValkyrieATK_0001.png", "Resources/Valkyrie/SpriteValkyrieATK_0002.png", "Resources/Valkyrie/SpriteValkyrieATK_0003.png", "Resources/Valkyrie/SpriteValkyrieATK_0004.png"},
    {"Resources/Valkyrie/SpriteValkyrieATK_0001shadow.png", "Resources/Valkyrie/SpriteValkyrieATK_0002shadow.png", "Resources/Valkyrie/SpriteValkyrieATK_0003shadow.png", "Resources/Valkyrie/SpriteValkyrieATK_0004shadow.png"},
    "Resources/Valkyrie/ValkyrieATK.png", "Resources/Valkyrie/ValkyrieFX.wav",
    Velocidad_media, Vida_media, Fuerza_media, Cooldown_medio, AttackSpeed_bajo, Rango_bajo, TipoAtaque::CuerpoACuerpo, true
};

inline const Pj_info PjC_Archer = {
    "Archer",
    {"Resources/Archer/SpriteArcherMov_0001.png", "Resources/Archer/SpriteArcherMov_0002.png", "Resources/Archer/SpriteArcherMov_0003.png"},
    {"Resources/Archer/SpriteArcherMov_0001shadow.png", "Resources/Archer/SpriteArcherMov_0002shadow.png", "Resources/Archer/SpriteArcherMov_0003shadow.png"},
    {"Resources/Archer/SpriteArcherStill_0001.png", "Resources/Archer/SpriteArcherStill_0002.png", "Resources/Archer/SpriteArcherStill_0003.png", "Resources/Archer/SpriteArcherStill_0004.png"},
    {"Resources/Archer/SpriteArcherStill_0001shadow.png", "Resources/Archer/SpriteArcherStill_0002shadow.png", "Resources/Archer/SpriteArcherStill_0003shadow.png", "Resources/Archer/SpriteArcherStill_0004shadow.png"},
    {"Resources/Archer/SpriteArcherATK_0001.png", "Resources/Archer/SpriteArcherATK_0002.png", "Resources/Archer/SpriteArcherATK_0003.png", "Resources/Archer/SpriteArcherATK_0004.png", "Resources/Archer/SpriteArcherATK_0005.png", "Resources/Archer/SpriteArcherATK_0006.png"},
    {"Resources/Archer/SpriteArcherATK_0001shadow.png", "Resources/Archer/SpriteArcherATK_0002shadow.png", "Resources/Archer/SpriteArcherATK_0003shadow.png", "Resources/Archer/SpriteArcherATK_0004shadow.png", "Resources/Archer/SpriteArcherATK_0005shadow.png", "Resources/Archer/SpriteArcherATK_0006shadow.png"},
    "Resources/Archer/ArcherATK.png", "Resources/Archer/ArcherFX.wav",
    Velocidad_media, Vida_baja, Fuerza_baja, Cooldown_medio, AttackSpeed_medio, Rango_bajo, TipoAtaque::Rango, false
};

inline const Pj_info PjC_Knight = {
    "Knight",
    {"Resources/Knight/SpriteKnightMov_0001.png", "Resources/Knight/SpriteKnightMov_0002.png", "Resources/Knight/SpriteKnightMov_0003.png", "Resources/Knight/SpriteKnightMov_0004.png"},
    {"Resources/Knight/SpriteKnightMov_0001shadow.png", "Resources/Knight/SpriteKnightMov_0002shadow.png", "Resources/Knight/SpriteKnightMov_0003shadow.png", "Resources/Knight/SpriteKnightMov_0004shadow.png"},
    {"Resources/Knight/SpriteKnightStill_0001.png", "Resources/Knight/SpriteKnightStill_0002.png"},
    {"Resources/Knight/SpriteKnightStill_0001shadow.png", "Resources/Knight/SpriteKnightStill_0002shadow.png"},
    {"Resources/Knight/SpriteKnightATK_0001.png", "Resources/Knight/SpriteKnightATK_0002.png", "Resources/Knight/SpriteKnightATK_0003.png"},
    {"Resources/Knight/SpriteKnightATK_0001shadow.png", "Resources/Knight/SpriteKnightATK_0002shadow.png", "Resources/Knight/SpriteKnightATK_0003shadow.png"},
    "Resources/Knight/KnightATK.png", "Resources/Knight/KnightFX.wav",
    Velocidad_media, Vida_baja, Fuerza_baja, Cooldown_nulo, AttackSpeed_muyalto, Rango_bajo, TipoAtaque::CuerpoACuerpo, false
};

// === EQUIPO OSCURO ===

inline const Pj_info PjC_Platero = {
    "Platero",
    {"Resources/Platero/SpritePlateroMov_0001.png", "Resources/Platero/SpritePlateroMov_0002.png"},
    {"Resources/Platero/SpritePlateroMov_0001shadow.png", "Resources/Platero/SpritePlateroMov_0002shadow.png"},
    {"Resources/Platero/SpritePlateroStill_0001.png", "Resources/Platero/SpritePlateroStill_0002.png", "Resources/Platero/SpritePlateroStill_0003.png"},
    {"Resources/Platero/SpritePlateroStill_0001shadow.png", "Resources/Platero/SpritePlateroStill_0002shadow.png", "Resources/Platero/SpritePlateroStill_0003shadow.png"},
    {"Resources/Platero/SpritePlateroATK_0001.png", "Resources/Platero/SpritePlateroATK_0002.png", "Resources/Platero/SpritePlateroATK_0003.png", "Resources/Platero/SpritePlateroATK_0004.png", "Resources/Platero/SpritePlateroATK_0005.png"},
    {"Resources/Platero/SpritePlateroATK_0001shadow.png", "Resources/Platero/SpritePlateroATK_0002shadow.png", "Resources/Platero/SpritePlateroATK_0003shadow.png", "Resources/Platero/SpritePlateroATK_0004shadow.png", "Resources/Platero/SpritePlateroATK_0005shadow.png"},
    "Resources/Platero/PlateroATK.png", "Resources/Platero/PlateroFX.wav",
    Velocidad_media, Vida_media, Fuerza_media, Cooldown_medio, AttackSpeed_alto, Rango_inf, TipoAtaque::Rango, true
};

inline const Pj_info PjC_ShapeShifter = {
    "ShapeShifter",
    {"Resources/ShapeShifter/SpriteShapeShifterMov_0001.png", "Resources/ShapeShifter/SpriteShapeShifterMov_0002.png", "Resources/ShapeShifter/SpriteShapeShifterMov_0003.png"},
    {"Resources/ShapeShifter/SpriteShapeShifterMov_0001shadow.png", "Resources/ShapeShifter/SpriteShapeShifterMov_0002shadow.png", "Resources/ShapeShifter/SpriteShapeShifterMov_0003shadow.png"},
    {"Resources/ShapeShifter/SpriteShapeShifterStill_0001.png", "Resources/ShapeShifter/SpriteShapeShifterStill_0002.png", "Resources/ShapeShifter/SpriteShapeShifterStill_0003.png"},
    {"Resources/ShapeShifter/SpriteShapeShifterStill_0001shadow.png", "Resources/ShapeShifter/SpriteShapeShifterStill_0002shadow.png", "Resources/ShapeShifter/SpriteShapeShifterStill_0003shadow.png"},
    {"Resources/ShapeShifter/SpriteShapeShifterATK_0001.png", "Resources/ShapeShifter/SpriteShapeShifterATK_0002.png", "Resources/ShapeShifter/SpriteShapeShifterATK_0003.png", "Resources/ShapeShifter/SpriteShapeShifterATK_0004.png"},
    {"Resources/ShapeShifter/SpriteShapeShifterATK_0001shadow.png", "Resources/ShapeShifter/SpriteShapeShifterATK_0002shadow.png", "Resources/ShapeShifter/SpriteShapeShifterATK_0003shadow.png", "Resources/ShapeShifter/SpriteShapeShifterATK_0004shadow.png"},
    "Resources/Shapeshifter/ShapeshifterATK.png", "Resources/Banshee/BansheeFX.wav",
    Velocidad_media, Vida_alta, Fuerza_alta, Cooldown_medio, AttackSpeed_bajo, Rango_inf, TipoAtaque::CuerpoACuerpo, true
};

inline const Pj_info PjC_Troll = {
    "Troll",
    {"Resources/Troll/SpriteTrollMov_0001.png", "Resources/Troll/SpriteTrollMov_0002.png", "Resources/Troll/SpriteTrollMov_0003.png"},
    {"Resources/Troll/SpriteTrollMov_0001shadow.png", "Resources/Troll/SpriteTrollMov_0002shadow.png", "Resources/Troll/SpriteTrollMov_0003shadow.png"},
    {"Resources/Troll/SpriteTrollStill_0001.png", "Resources/Troll/SpriteTrollStill_0002.png", "Resources/Troll/SpriteTrollStill_0003.png"},
    {"Resources/Troll/SpriteTrollStill_0001shadow.png", "Resources/Troll/SpriteTrollStill_0002shadow.png", "Resources/Troll/SpriteTrollStill_0003shadow.png"},
    {"Resources/Troll/SpriteTrollATK_0001.png", "Resources/Troll/SpriteTrollATK_0002.png", "Resources/Troll/SpriteTrollATK_0003.png", "Resources/Troll/SpriteTrollATK_0004.png"},
    {"Resources/Troll/SpriteTrollATK_0001shadow.png", "Resources/Troll/SpriteTrollATK_0002shadow.png", "Resources/Troll/SpriteTrollATK_0003shadow.png", "Resources/Troll/SpriteTrollATK_0004shadow.png"},
    "Resources/Troll/TrollATK.png", "Resources/Troll/TrollFX.wav",
    Velocidad_baja, Vida_alta, Fuerza_alta, Cooldown_alto, AttackSpeed_bajo, Rango_bajo, TipoAtaque::CuerpoACuerpo, false
};

inline const Pj_info PjC_Dragon = {
    "Dragon",
    {"Resources/Dragon/SpriteDragonMov_0001.png", "Resources/Dragon/SpriteDragonMov_0002.png"},
    {"Resources/Dragon/SpriteDragonMov_0001shadow.png", "Resources/Dragon/SpriteDragonMov_0002shadow.png"},
    {"Resources/Dragon/SpriteDragonStill_0001.png", "Resources/Dragon/SpriteDragonStill_0002.png", "Resources/Dragon/SpriteDragonStill_0003.png", "Resources/Dragon/SpriteDragonStill_0004.png"},
    {"Resources/Dragon/SpriteDragonStill_0001shadow.png", "Resources/Dragon/SpriteDragonStill_0002shadow.png", "Resources/Dragon/SpriteDragonStill_0003shadow.png", "Resources/Dragon/SpriteDragonStill_0004shadow.png"},
    {"Resources/Dragon/SpriteDragonATK_0001.png", "Resources/Dragon/SpriteDragonATK_0002.png", "Resources/Dragon/SpriteDragonATK_0003.png", "Resources/Dragon/SpriteDragonATK_0004.png"},
    {"Resources/Dragon/SpriteDragonATK_0001shadow.png", "Resources/Dragon/SpriteDragonATK_0002shadow.png", "Resources/Dragon/SpriteDragonATK_0003shadow.png", "Resources/Dragon/SpriteDragonATK_0004shadow.png"},
    "Resources/Dragon/DragonATK.png", "Resources/Dragon/DragonFX.wav",
    Velocidad_media, Vida_muyalta, Fuerza_muyalta, Cooldown_alto, AttackSpeed_medio, Rango_inf, TipoAtaque::Rango, true
};

inline const Pj_info PjC_Basilisk = {
    "Basilisk",
    {"Resources/Basilisk/SpriteBasiliskMov_0001.png", "Resources/Basilisk/SpriteBasiliskMov_0002.png"},
    {"Resources/Basilisk/SpriteBasiliskMov_0001shadow.png", "Resources/Basilisk/SpriteBasiliskMov_0002shadow.png"},
    {"Resources/Basilisk/SpriteBasiliskStill_0001.png", "Resources/Basilisk/SpriteBasiliskStill_0002.png", "Resources/Basilisk/SpriteBasiliskStill_0003.png"},
    {"Resources/Basilisk/SpriteBasiliskStill_0001shadow.png", "Resources/Basilisk/SpriteBasiliskStill_0002shadow.png", "Resources/Basilisk/SpriteBasiliskStill_0003shadow.png"},
    {"Resources/Basilisk/SpriteBasiliskATK_0001.png", "Resources/Basilisk/SpriteBasiliskATK_0002.png", "Resources/Basilisk/SpriteBasiliskATK_0003.png", "Resources/Basilisk/SpriteBasiliskATK_0004.png", "Resources/Basilisk/SpriteBasiliskATK_0005.png"},
    {"Resources/Basilisk/SpriteBasiliskATK_0001shadow.png", "Resources/Basilisk/SpriteBasiliskATK_0002shadow.png", "Resources/Basilisk/SpriteBasiliskATK_0003shadow.png", "Resources/Basilisk/SpriteBasiliskATK_0004shadow.png", "Resources/Basilisk/SpriteBasiliskATK_0005shadow.png"},
    "Resources/Basilisk/BasiliskATK.png", "Resources/Basilisk/BasiliskFX.wav",
    Velocidad_media, Vida_baja, Fuerza_alta, Cooldown_bajo, AttackSpeed_alto, Rango_inf, TipoAtaque::Rango, false
};

inline const Pj_info PjC_Banshee = {
    "Banshee",
    {"Resources/Banshee/SpriteBansheeMov_0001.png", "Resources/Banshee/SpriteBansheeMov_0002.png", "Resources/Banshee/SpriteBansheeMov_0003.png", "Resources/Banshee/SpriteBansheeMov_0004.png"},
    {"Resources/Banshee/SpriteBansheeMov_0001shadow.png", "Resources/Banshee/SpriteBansheeMov_0002shadow.png", "Resources/Banshee/SpriteBansheeMov_0003shadow.png", "Resources/Banshee/SpriteBansheeMov_0004shadow.png"},
    {"Resources/Banshee/SpriteBansheeStill_0001.png", "Resources/Banshee/SpriteBansheeStill_0002.png", "Resources/Banshee/SpriteBansheeStill_0003.png", "Resources/Banshee/SpriteBansheeStill_0004.png"},
    {"Resources/Banshee/SpriteBansheeStill_0001shadow.png", "Resources/Banshee/SpriteBansheeStill_0002shadow.png", "Resources/Banshee/SpriteBansheeStill_0003shadow.png", "Resources/Banshee/SpriteBansheeStill_0004shadow.png"},
    {"Resources/Banshee/SpriteBansheeATK_0001.png", "Resources/Banshee/SpriteBansheeATK_0002.png", "Resources/Banshee/SpriteBansheeATK_0003.png"},
    {"Resources/Banshee/SpriteBansheeATK_0001shadow.png", "Resources/Banshee/SpriteBansheeATK_0002shadow.png", "Resources/Banshee/SpriteBansheeATK_0003shadow.png"},
    "Resources/Banshee/BansheeATK.png", "Resources/Banshee/BansheeFX.wav",
    Velocidad_media, Vida_media, Fuerza_media, Cooldown_alto, AttackSpeed_muyalto, Rango_medio, TipoAtaque::CuerpoACuerpo, true
};

inline const Pj_info PjC_Manticore = {
    "Manticore",
    {"Resources/Manticore/SpriteManticoreMov_0001.png", "Resources/Manticore/SpriteManticoreMov_0002.png"},
    {"Resources/Manticore/SpriteManticoreMov_0001shadow.png", "Resources/Manticore/SpriteManticoreMov_0002shadow.png"},
    {"Resources/Manticore/SpriteManticoreStill_0001.png", "Resources/Manticore/SpriteManticoreStill_0002.png"},
    {"Resources/Manticore/SpriteManticoreStill_0001shadow.png", "Resources/Manticore/SpriteManticoreStill_0002shadow.png"},
    {"Resources/Manticore/SpriteManticoreATK_0001.png", "Resources/Manticore/SpriteManticoreATK_0002.png", "Resources/Manticore/SpriteManticoreATK_0003.png", "Resources/Manticore/SpriteManticoreATK_0004.png"},
    {"Resources/Manticore/SpriteManticoreATK_0001shadow.png", "Resources/Manticore/SpriteManticoreATK_0002shadow.png", "Resources/Manticore/SpriteManticoreATK_0003shadow.png", "Resources/Manticore/SpriteManticoreATK_0004shadow.png"},
    "Resources/Manticore/ManticoreATK.png", "Resources/Manticore/ManticoreFX.wav",
    Velocidad_media, Vida_media, Fuerza_baja, Cooldown_medio, AttackSpeed_bajo, Rango_inf, TipoAtaque::Rango, false
};

inline const Pj_info PjC_Goblin = {
    "Goblin",
    {"Resources/Goblin/SpriteGoblinMov_0001.png", "Resources/Goblin/SpriteGoblinMov_0002.png"},
    {"Resources/Goblin/SpriteGoblinMov_0001shadow.png", "Resources/Goblin/SpriteGoblinMov_0002shadow.png"},
    {"Resources/Goblin/SpriteGoblinStill_0001.png", "Resources/Goblin/SpriteGoblinStill_0002.png"},
    {"Resources/Goblin/SpriteGoblinStill_0001shadow.png", "Resources/Goblin/SpriteGoblinStill_0002shadow.png"},
    {"Resources/Goblin/SpriteGoblinATK_0001.png", "Resources/Goblin/SpriteGoblinATK_0002.png", "Resources/Goblin/SpriteGoblinATK_0003.png", "Resources/Goblin/SpriteGoblinATK_0004.png", "Resources/Goblin/SpriteGoblinATK_0005.png", "Resources/Goblin/SpriteGoblinATK_0006.png"},
    {"Resources/Goblin/SpriteGoblinATK_0001shadow.png", "Resources/Goblin/SpriteGoblinATK_0002shadow.png", "Resources/Goblin/SpriteGoblinATK_0003shadow.png", "Resources/Goblin/SpriteGoblinATK_0004shadow.png", "Resources/Goblin/SpriteGoblinATK_0005shadow.png", "Resources/Goblin/SpriteGoblinATK_0006shadow.png"},
    "Resources/Goblin/GoblinATK.png", "Resources/Goblin/GoblinFX.wav",
    Velocidad_media, Vida_baja, Fuerza_baja, Cooldown_nulo, AttackSpeed_muyalto, Rango_bajo, TipoAtaque::CuerpoACuerpo, false
};
