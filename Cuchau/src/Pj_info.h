#pragma once
#include <string_view>
#include <vector>

//  Pj_info.h — Definiciones de los 16 personajes jugables
//
//  Contiene las constantes de balance del juego (velocidad, vida, fuerza, etc.)
//  y la estructura Pj_info con todas las propiedades de cada personaje:
//  sprites, sombras, sonidos, estadisticas y flags de comportamiento.
//
//  Equipos:
//    Claro (8): MH, Phoenix, Golem, Djinni, Unicorn, Valkyrie, Archer, Knight
//    Oscuro (8): Platero, ShapeShifter, Troll, Dragon, Basilisk, Banshee, Manticore, Goblin

// --- Constantes de balance del juego ---

// Velocidades de movimiento (en pixeles/segundo originales, se escalan con SPEED_SCALE)
inline constexpr float Velocidad_media = 500.0f, Velocidad_baja = 200.0f;

// Puntos de vida
inline constexpr float Vida_muyalta = 2000.0f, Vida_alta = 1000.0f, Vida_media = 500.0f, Vida_baja = 200.0f;

// Fuerza de ataque (dano por impacto de disparo)
inline constexpr float Fuerza_muyalta = 80.0f, Fuerza_alta = 50.0f, Fuerza_media = 10.0f, Fuerza_baja = 5.0f;

// Tiempo de espera entre disparos (en segundos)
inline constexpr float Cooldown_alto = 5.0f, Cooldown_medio = 3.0f, Cooldown_bajo = 1.0f, Cooldown_nulo = 0.0f;

// Velocidad de los proyectiles (en pixeles/segundo originales, se escalan con SPEED_SCALE)
inline constexpr float AttackSpeed_muyalto = 1000.0f, AttackSpeed_alto = 800.0f, AttackSpeed_medio = 500.0f, AttackSpeed_bajo = 200.0f;

// --- Estructura de controles del teclado ---
struct cntrl
{
    int up, down, left, right;  // Teclas de movimiento asignadas al personaje
};

// --- Estructura con todas las propiedades de un personaje ---
struct Pj_info
{
    std::string_view nombre;             // Nombre mostrado en el HUD y menu
    std::vector<std::string_view> Sprites;           // Sprite frame para la animación
    std::vector<std::string_view> Sprites_shadow;           // Sprite frame para la animación de la sombra
    std::string_view Ataque;             // Sprite del proyectil que lanza
    std::string_view Efecto_ataque;      // Archivo de sonido al disparar
    float vel;                      // Velocidad de movimiento
    float vida;                     // Puntos de vida iniciales
    float fuerza;                   // Dano por impacto
    float cooldown;                 // Tiempo entre disparos (segundos)
    float attack_speed;             // Velocidad del proyectil
    int   frameCount;               // Numero de frames de animacion
    float frameSpeed;               // Tiempo por frame de animacion (segundos)
    bool rango;                     // TRUE = ataque a distancia, FALSE = cuerpo a cuerpo
    bool vuela;                     // TRUE = puede volar, FALSE = terrestre
};

// ============================================================================
//  EQUIPO CLARO — 8 personajes
// ============================================================================

// MH — Mago principal. Velocidad media, vida media, fuerza alta. Vuela y ataca a rango.
inline const Pj_info MH = {
    "MH",

    {"bin/Resources/MH/SpriteMH_0001.png",
    "bin/Resources/MH/SpriteMH_0006.png",
    "bin/Resources/MH/SpriteMH_0001.png"},

    {"bin/Resources/MH/SpriteMH_shadow.png",
    "bin/Resources/MH/SpriteMH_shadow.png",
    "bin/Resources/MH/SpriteMH_shadow.png"},

    "bin/Resources/MH/MHATK.png",
    "bin/Resources/MH/RaspberryPIMH.wav",
    Velocidad_media,
    Vida_media,
    Fuerza_alta,
    Cooldown_medio,
    AttackSpeed_medio,
    3,
    0.1f,
    true, // Rango
    true // Vuela
};

// Phoenix — Ave fenix. Vida alta, fuerza alta, pero cooldown y velocidad de ataque bajos.
inline const Pj_info Phoenix = {
    "Phoenix",

    {"bin/Resources/Phoenix/SpritePhoenix.png",
    "bin/Resources/MH/SpriteMH_0002.png",
    "bin/Resources/MH/SpriteMH_0003.png"},

    {"bin/Resources/MH/SpriteMH_shadow.png",
    "bin/Resources/MH/SpriteMH_shadow.png",
    "bin/Resources/MH/SpriteMH_shadow.png"},

    "bin/Resources/Phoenix/PhoenixATK.png",
    "bin/Resources/Phoenix/PhoenixFX.wav",
    Velocidad_media,
    Vida_alta,
    Fuerza_alta,
    Cooldown_alto,
    AttackSpeed_bajo,
    3,
    0.1f,
    false, // No rango
    true // Vuela
};

// Golem — Tanque lento. Velocidad baja, vida alta, fuerza alta, cooldown alto.
inline const Pj_info Golem = {
    "Golem",

    {"bin/Resources/Golem/SpriteGolem.png",
    "bin/Resources/MH/SpriteMH_0002.png",
    "bin/Resources/MH/SpriteMH_0003.png"},

    {"bin/Resources/MH/SpriteMH_shadow.png",
    "bin/Resources/MH/SpriteMH_shadow.png",
    "bin/Resources/MH/SpriteMH_shadow.png"},

    "bin/Resources/Archer/SpriteFlecha.png",
    "bin/Resources/Golem/GolemFX.wav",
    Velocidad_baja,
    Vida_alta,
    Fuerza_alta,
    Cooldown_alto,
    AttackSpeed_bajo,
    3,
    0.1f,
    true, // Rango
    false // No vuela
};

// Djinni — Genio volador. Estadisticas equilibradas, fuerza media.
inline const Pj_info Djinni = {
    "Djinni",

    {"bin/Resources/Djinni/SpiteDjinni.png",
    "bin/Resources/MH/SpriteMH_0002.png",
    "bin/Resources/MH/SpriteMH_0003.png"},

    {"bin/Resources/MH/SpriteMH_0001.png",
    "bin/Resources/MH/SpriteMH_0006.png",
    "bin/Resources/MH/SpriteMH_0001.png"},

    "bin/Resources/Djinni/DjinniATK.png",
    "bin/Resources/Djinni/DjinniFX.wav",
    Velocidad_media,
    Vida_alta,
    Fuerza_media,
    Cooldown_medio,
    AttackSpeed_medio,
    3,
    0.1f,
    true, // Rango
    true // Vuela
};

// Unicorn — Unicornio. Vida media, cooldown bajo (dispara rapido), velocidad de ataque alta.
inline const Pj_info Unicorn = {
    "Unicorn",

    {"bin/Resources/Unicorn/SpriteUnicorn_0002.png",
    "bin/Resources/MH/SpriteMH_0002.png",
    "bin/Resources/MH/SpriteMH_0003.png"},

    {"bin/Resources/MH/SpriteMH_0001.png",
    "bin/Resources/MH/SpriteMH_0006.png",
    "bin/Resources/MH/SpriteMH_0001.png"},

    "bin/Resources/Unicorn/UnicornATK.png",
    "bin/Resources/Valkyrie/magicspearfx.wav",
    Velocidad_media,
    Vida_media,
    Fuerza_media,
    Cooldown_bajo,
    AttackSpeed_alto,
    3,
    0.1f,
    true, // Rango
    false // No vuela
};

// Valkyrie — Valkiria voladora. Estadisticas medias, ataque a rango.
inline const Pj_info Valkyrie = {
    "Valkyrie",

    {"bin/Resources/Valkyrie/SpriteValkyrie.png",
    "bin/Resources/MH/SpriteMH_0002.png",
    "bin/Resources/MH/SpriteMH_0003.png"},

    {"bin/Resources/MH/SpriteMH_0001.png",
    "bin/Resources/MH/SpriteMH_0006.png",
    "bin/Resources/MH/SpriteMH_0001.png"},

    "bin/Resources/Valkyrie/ValkyrieATK.png",
    "bin/Resources/Valkyrie/ValkyrieFX.wav",
    Velocidad_media,
    Vida_media,
    Fuerza_media,
    Cooldown_medio,
    AttackSpeed_bajo,
    3,
    0.1f,
    true, // Rango
    true // Vuela
};

// Archer — Arquero. Vida y fuerza bajas, pero disparo a velocidad media.
inline const Pj_info Archer = {
    "Archer",

    {"bin/Resources/Archer/SpriteArcherpxo.png",
    "bin/Resources/MH/SpriteMH_0002.png",
    "bin/Resources/MH/SpriteMH_0003.png"},

    {"bin/Resources/MH/SpriteMH_0001.png",
    "bin/Resources/MH/SpriteMH_0006.png",
    "bin/Resources/MH/SpriteMH_0001.png"},

    "bin/Resources/Archer/ArcherATK.png",
    "bin/Resources/Archer/ArcherFX.wav",
    Velocidad_media,
    Vida_baja,
    Fuerza_baja,
    Cooldown_medio,
    AttackSpeed_medio,
    3,
    0.1f,
    true, // Rango
    false // No vuela
};

// Knight — Caballero cuerpo a cuerpo. Vida baja, fuerza baja, pero sin cooldown y proyectil muy rapido.
inline const Pj_info Knight = {
    "Knight",

    {"bin/Resources/Knight/SpriteKnight.png",
    "bin/Resources/MH/SpriteMH_0002.png",
    "bin/Resources/MH/SpriteMH_0003.png"},

    {"bin/Resources/MH/SpriteMH_0001.png",
    "bin/Resources/MH/SpriteMH_0006.png",
    "bin/Resources/MH/SpriteMH_0001.png"},

    "bin/Resources/Knight/KnightATK.png",
    "bin/Resources/Knight/KnightFX.wav",
    Velocidad_media,
    Vida_baja,
    Fuerza_baja,
    Cooldown_nulo,
    AttackSpeed_muyalto,
    3,
    0.1f,
    false, // No rango
    false // No vuela
};


// ============================================================================
//  EQUIPO OSCURO — 8 personajes
// ============================================================================

// Platero — Mago oscuro. Velocidad media, ataque a rango con proyectil rapido.
inline const Pj_info Platero = {
    "Platero",

    {"bin/Resources/Platero/SpritePlatero.png",
    "bin/Resources/MH/SpriteMH_0002.png",
    "bin/Resources/MH/SpriteMH_0003.png"},

    {"bin/Resources/MH/SpriteMH_0001.png",
    "bin/Resources/MH/SpriteMH_0006.png",
    "bin/Resources/MH/SpriteMH_0001.png"},

    "bin/Resources/Platero/PlateroATK.png",
    "bin/Resources/Platero/CorrectoPlatero.wav",
    Velocidad_media,
    Vida_media,
    Fuerza_media,
    Cooldown_medio,
    AttackSpeed_alto,
    3,
    0.1f,
    true, // Rango
    true // Vuela
};

// ShapeShifter — Cambiaformas. Vida alta, fuerza alta, cuerpo a cuerpo.
inline const Pj_info ShapeShifter = {
    "ShapeShifter",

    {"bin/Resources/Shapeshifter/SpriteShapeshifter.png",
    "bin/Resources/MH/SpriteMH_0002.png",
    "bin/Resources/MH/SpriteMH_0003.png"},

    {"bin/Resources/MH/SpriteMH_0001.png",
    "bin/Resources/MH/SpriteMH_0006.png",
    "bin/Resources/MH/SpriteMH_0001.png"},

    "bin/Resources/Shapeshifter/ShapeshifterATK.png",
    "bin/Resources/Banshee/scream.wav",
    Velocidad_media,
    Vida_alta,
    Fuerza_alta,
    Cooldown_medio,
    AttackSpeed_bajo,
    3,
    0.1f,
    false, // No rango
    true // Vuela
};

// Troll — Tanque lento del equipo oscuro. Similar al Golem.
inline const Pj_info Troll = {
    "Troll",

    {"bin/Resources/Troll/SpriteTroll.png",
    "bin/Resources/MH/SpriteMH_0002.png",
    "bin/Resources/MH/SpriteMH_0003.png"},

    {"bin/Resources/MH/SpriteMH_0001.png",
    "bin/Resources/MH/SpriteMH_0006.png",
    "bin/Resources/MH/SpriteMH_0001.png"},

    "bin/Resources/Archer/SpriteFlecha.png",
    "bin/Resources/Troll/trollFX.wav",
    Velocidad_baja,
    Vida_alta,
    Fuerza_alta,
    Cooldown_alto,
    AttackSpeed_bajo,
    3,
    0.1f,
    true, // Rango
    false // No vuela
};

// Dragon — El mas poderoso. Vida muy alta, fuerza muy alta, pero cooldown alto.
inline const Pj_info Dragon = {
    "Dragon",

    {"bin/Resources/Dragon/SpriteDragon.png",
    "bin/Resources/MH/SpriteMH_0002.png",
    "bin/Resources/MH/SpriteMH_0003.png"},

    {"bin/Resources/MH/SpriteMH_0001.png",
    "bin/Resources/MH/SpriteMH_0006.png",
    "bin/Resources/MH/SpriteMH_0001.png"},

    "bin/Resources/Dragon/DragonATK.png",
    "bin/Resources/Dragon/DragonFX.wav",
    Velocidad_media,
    Vida_muyalta,
    Fuerza_muyalta,
    Cooldown_alto,
    AttackSpeed_medio,
    3,
    0.1f,
    true, // Rango
    true // Vuela
};

// Basilisk — Basilisco. Vida baja pero fuerza alta, cooldown bajo y proyectil rapido.
inline const Pj_info Basilisk = {
    "Basilisk",

    {"bin/Resources/Basilisk/SpriteBasilisk.png",
    "bin/Resources/MH/SpriteMH_0002.png",
    "bin/Resources/MH/SpriteMH_0003.png"},

    {"bin/Resources/MH/SpriteMH_0001.png",
    "bin/Resources/MH/SpriteMH_0006.png",
    "bin/Resources/MH/SpriteMH_0001.png"},

    "bin/Resources/Basilisk/BasiliskATK.png",
    "bin/Resources/Basilisk/BasiliskFX.wav",
    Velocidad_media,
    Vida_baja,
    Fuerza_alta,
    Cooldown_bajo,
    AttackSpeed_alto,
    3,
    0.1f,
    true, // Rango
    false // No vuela
};

// Banshee — Fantasma aullador. Vida media, cooldown alto pero proyectil muy rapido.
inline const Pj_info Banshee = {
    "Banshee",

    {"bin/Resources/Banshee/SpriteBanshee.png",
    "bin/Resources/MH/SpriteMH_0002.png",
    "bin/Resources/MH/SpriteMH_0003.png"},

    {"bin/Resources/MH/SpriteMH_0001.png",
    "bin/Resources/MH/SpriteMH_0006.png",
    "bin/Resources/MH/SpriteMH_0001.png"},

    "bin/Resources/Banshee/BansheeATK.png",
    "bin/Resources/Banshee/BansheeFX.wav",
    Velocidad_media,
    Vida_media,
    Fuerza_media,
    Cooldown_alto,
    AttackSpeed_muyalto,
    3,
    0.1f,
    true, // Rango
    true // Vuela
};

// Manticore — Manticora. Vida media, fuerza baja, proyectil lento.
inline const Pj_info Manticore = {
    "Manticore",

    {"bin/Resources/Manticore/SpriteManticore.png",
    "bin/Resources/MH/SpriteMH_0002.png",
    "bin/Resources/MH/SpriteMH_0003.png"},

    {"bin/Resources/MH/SpriteMH_0001.png",
    "bin/Resources/MH/SpriteMH_0006.png",
    "bin/Resources/MH/SpriteMH_0001.png"},

    "bin/Resources/Manticore/ManticoreATK.png",
    "bin/Resources/Djinni/vientofeericofx.wav",
    Velocidad_media,
    Vida_media,
    Fuerza_baja,
    Cooldown_medio,
    AttackSpeed_bajo,
    3,
    0.1f,
    true, // Rango
    false // No vuela
};

// Goblin — Duende cuerpo a cuerpo. Vida y fuerza bajas, pero sin cooldown y proyectil ultra rapido.
inline const Pj_info Goblin = {
    "Goblin",

    {"bin/Resources/Goblin/Spritegoblin.png",
    "bin/Resources/MH/SpriteMH_0002.png",
    "bin/Resources/MH/SpriteMH_0003.png"},

    {"bin/Resources/MH/SpriteMH_0001.png",
    "bin/Resources/MH/SpriteMH_0006.png",
    "bin/Resources/MH/SpriteMH_0001.png"},

    "bin/Resources/Goblin/GoblinATK.png",
    "bin/Resources/Goblin/GoblinFX.wav",
    Velocidad_media,
    Vida_baja,
    Fuerza_baja,
    Cooldown_nulo,
    AttackSpeed_muyalto,
    3,
    0.1f,
    false, // No rango
    false // No vuela
};
