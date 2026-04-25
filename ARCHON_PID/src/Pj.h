#pragma once

inline constexpr float Velocidad_media = 500.0f, Velocidad_baja=200.0f;
inline constexpr float Vida_muyalta=2000.0f, Vida_alta = 1000.0f, Vida_media = 500.0f, Vida_baja = 200.0f;

enum equipo { LUZ,OSCURIDAD };
enum class tipo_pj { MH, Phoenix,Golem,Djinni,Unicorn,Valkyrie,Archer,Knight,Platero,ShapeShifter,Troll,Dragon,Basilisk,Banshee,Manticore,Goblin};

struct Pj
{
    const char* Sprite;
    const char* Ataque;
    float vel;
    float vida;
    bool rango; // Rango (TRUE) o cuerpo a cuerpo (FALSE)
    bool vuela; // Puede volar (TRUE) o no (FALSE)
    bool equipo; //Luz (FALSE) u oscuridad (TRUE)
    tipo_pj ID; //Permite diferenciar los tipos de personajes
    const unsigned int rangoTablero;
};

//Claro
inline constexpr Pj MH = {
    "bin/Resources/Graficos/Sprites/SpriteMH_0001.png",
    "bin/Resources/Graficos/Sprites/SpriteFlecha.png",
    Velocidad_media,
    Vida_media,
    true, // Rango
    true, // Vuela
    LUZ,
    tipo_pj::MH,
    3
};

inline constexpr Pj Phoenix = {
    "bin/Resources/Graficos/Sprites/SpritePhoenix.png",
    "bin/Resources/Graficos/Sprites/SpriteFlecha.png",
    Velocidad_media,
    Vida_alta,
    false, // No rango
    true, // Vuela
    LUZ,
    tipo_pj::Phoenix,
    5
};

inline constexpr Pj Golem = {
    "bin/Resources/Graficos/Sprites/SpriteGolem.png",
    "bin/Resources/Graficos/Sprites/SpriteFlecha.png",
    Velocidad_baja,
    Vida_alta,
    true, // Rango
    false, // No vuela
    LUZ,
    tipo_pj::Golem,
    3
};

inline constexpr Pj Djinni = {
    "bin/Resources/Graficos/Sprites/SpiteDjinni.png",
    "bin/Resources/Graficos/Sprites/SpriteFlecha.png",
    Velocidad_media,
    Vida_alta,
    true, // Rango
    true, // Vuela
    LUZ,
    tipo_pj::Djinni,
    4
};

inline constexpr Pj Unicorn = {
    "bin/Resources/Graficos/Sprites/SpriteUnicorn_0002.png",
    "bin/Resources/Graficos/Sprites/SpriteFlecha.png",
    Velocidad_media,
    Vida_media,
    true, // Rango
    false, // No vuela
    LUZ,
   tipo_pj::Unicorn,
   4
};

inline constexpr Pj Valkyrie = {
    "bin/Resources/Graficos/Sprites/SpriteValkyrie.png",
    "bin/Resources/Graficos/Sprites/SpriteFlecha.png",
    Velocidad_media,
    Vida_media,
    true, // Rango
    true, // Vuela
    LUZ,
   tipo_pj::Valkyrie,
   3
};

inline constexpr Pj Archer = {
    "bin/Resources/Graficos/Sprites/SpriteArcherpxo.png",
    "bin/Resources/Graficos/Sprites/SpriteFlecha.png",
    Velocidad_media,
    Vida_baja,
    true, // Rango
    false, // No vuela
    LUZ,
    tipo_pj::Archer,
    3
};

inline constexpr Pj Knight = {
    "bin/Resources/Graficos/Sprites/SpriteKnight.png",
    "bin/Resources/Graficos/Sprites/SpriteFlecha.png",
    Velocidad_media,
    Vida_baja,
    false, // No rango
    false, // No vuela
    LUZ,
    tipo_pj::Knight,
    3
};


//Oscuro
inline constexpr Pj Platero = {
    "bin/Resources/Graficos/Sprites/SpritePlatero.png",
    "bin/Resources/Graficos/Sprites/SpriteFlecha.png",
    Velocidad_media,
    Vida_media,
    true, // Rango
    true, // Vuela
    OSCURIDAD,
    tipo_pj::Platero,
    3
};

inline constexpr Pj ShapeShifter = {
    "bin/Resources/Graficos/Sprites/SpriteShapeshifter.png",
    "bin/Resources/Graficos/Sprites/SpriteFlecha.png",
    Velocidad_media,
    Vida_alta,
    false, // No rango
    true, // Vuela
    OSCURIDAD,
    tipo_pj::ShapeShifter,
    5
};

inline constexpr Pj Troll = {
    "bin/Resources/Graficos/Sprites/SpriteTroll.png",
    "bin/Resources/Graficos/Sprites/SpriteFlecha.png",
    Velocidad_baja,
    Vida_alta,
    true, // Rango
    false, // No vuela
    OSCURIDAD,
    tipo_pj::Troll,
    3
};

inline constexpr Pj Dragon = {
    "bin/Resources/Graficos/Sprites/SpriteDragon.png",
    "bin/Resources/Graficos/Sprites/SpriteFlecha.png",
    Velocidad_media,
    Vida_muyalta,
    true, // Rango
    true, // Vuela
    OSCURIDAD,
    tipo_pj::Dragon,
    4
};

inline constexpr Pj Basilisk = {
    "bin/Resources/Graficos/Sprites/SpriteBasilisk.png",
    "bin/Resources/Graficos/Sprites/SpriteFlecha.png",
    Velocidad_media,
    Vida_baja,
    true, // Rango
    false, // No vuela
    OSCURIDAD,
    tipo_pj::Basilisk,
    3
};

inline constexpr Pj Banshee = {
    "bin/Resources/Graficos/Sprites/SpriteBanshee.png",
    "bin/Resources/Graficos/Sprites/SpriteFlecha.png",
    Velocidad_media,
    Vida_media,
    true, // Rango
    true, // Vuela
    OSCURIDAD,
    tipo_pj::Banshee,
    3
};

inline constexpr Pj Manticore = {
    "bin/Resources/Graficos/Sprites/SpriteManticore.png",
    "bin/Resources/Graficos/Sprites/SpriteFlecha.png",
    Velocidad_media,
    Vida_media,
    true, // Rango
    false, // No vuela
    OSCURIDAD,
    tipo_pj::Manticore,
    3
};

inline constexpr Pj Goblin = {
    "bin/Resources/Graficos/Sprites/Spritegoblin.png",
    "bin/Resources/Graficos/Sprites/SpriteFlecha.png",
    Velocidad_media,
    Vida_baja,
    false, // No rango
    false, // No vuela
    OSCURIDAD,
    tipo_pj::Goblin,
    3
};
