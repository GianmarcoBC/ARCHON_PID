#pragma once

inline constexpr float Velocidad_media = 500.0f, Velocidad_baja=200.0f;
inline constexpr float Vida_muyalta=2000.0f, Vida_alta = 1000.0f, Vida_media = 500.0f, Vida_baja = 200.0f;


struct Pj
{
    const char* Sprite;
    const char* Ataque;
    float vel;
    float vida;
    bool rango; // Rango (TRUE) o cuerpo a cuerpo (FALSE)
    bool vuela; // Puede volar (TRUE) o no (FALSE)
};

//Claro
inline constexpr Pj MH = {
    "bin/Resources/Graficos/Sprites/SpriteMH_0001.png",
    "bin/Resources/Graficos/Sprites/SpriteFlecha.png",
    Velocidad_media,
    Vida_media,
    true, // Rango
    true // Vuela
};

inline constexpr Pj Phoenix = {
    "bin/Resources/Graficos/Sprites/SpritePhoenix.png",
    "bin/Resources/Graficos/Sprites/SpriteFlecha.png",
    Velocidad_media,
    Vida_alta,
    false, // No rango
    true // Vuela
};

inline constexpr Pj Golem = {
    "bin/Resources/Graficos/Sprites/SpriteGolem.png",
    "bin/Resources/Graficos/Sprites/SpriteFlecha.png",
    Velocidad_baja,
    Vida_alta,
    true, // Rango
    false // No vuela
};

inline constexpr Pj Djinni = {
    "bin/Resources/Graficos/Sprites/SpiteDjinni.png",
    "bin/Resources/Graficos/Sprites/SpriteFlecha.png",
    Velocidad_media,
    Vida_alta,
    true, // Rango
    true // Vuela
};

inline constexpr Pj Unicorn = {
    "bin/Resources/Graficos/Sprites/SpriteUnicorn_0002.png",
    "bin/Resources/Graficos/Sprites/SpriteFlecha.png",
    Velocidad_media,
    Vida_media,
    true, // Rango
    false // No vuela
};

inline constexpr Pj Valkyrie = {
    "bin/Resources/Graficos/Sprites/SpriteValkyrie.png",
    "bin/Resources/Graficos/Sprites/SpriteFlecha.png",
    Velocidad_media,
    Vida_media,
    true, // Rango
    true // Vuela
};

inline constexpr Pj Archer = {
    "bin/Resources/Graficos/Sprites/SpriteArcherpxo.png",
    "bin/Resources/Graficos/Sprites/SpriteFlecha.png",
    Velocidad_media,
    Vida_baja,
    true, // Rango
    false // No vuela
};

inline constexpr Pj Knight = {
    "bin/Resources/Graficos/Sprites/SpriteKnight.png",
    "bin/Resources/Graficos/Sprites/SpriteFlecha.png",
    Velocidad_media,
    Vida_baja,
    false, // No rango
    false // No vuela
};


//Oscuro
inline constexpr Pj Platero = {
    "bin/Resources/Graficos/Sprites/SpritePlatero.png",
    "bin/Resources/Graficos/Sprites/SpriteFlecha.png",
    Velocidad_media,
    Vida_media,
    true, // Rango
    true // Vuela
};

inline constexpr Pj ShapeShifter = {
    "bin/Resources/Graficos/Sprites/SpriteShapeshifter.png",
    "bin/Resources/Graficos/Sprites/SpriteFlecha.png",
    Velocidad_media,
    Vida_alta,
    false, // No rango
    true // Vuela
};

inline constexpr Pj Troll = {
    "bin/Resources/Graficos/Sprites/SpriteTroll.png",
    "bin/Resources/Graficos/Sprites/SpriteFlecha.png",
    Velocidad_baja,
    Vida_alta,
    true, // Rango
    false // No vuela
};

inline constexpr Pj Dragon = {
    "bin/Resources/Graficos/Sprites/SpriteDragon.png",
    "bin/Resources/Graficos/Sprites/SpriteFlecha.png",
    Velocidad_media,
    Vida_muyalta,
    true, // Rango
    true // Vuela
};

inline constexpr Pj Basilisk = {
    "bin/Resources/Graficos/Sprites/SpriteBasilisk.png",
    "bin/Resources/Graficos/Sprites/SpriteFlecha.png",
    Velocidad_media,
    Vida_baja,
    true, // Rango
    false // No vuela
};

inline constexpr Pj Banshee = {
    "bin/Resources/Graficos/Sprites/SpriteBanshee.png",
    "bin/Resources/Graficos/Sprites/SpriteFlecha.png",
    Velocidad_media,
    Vida_media,
    true, // Rango
    true // Vuela
};

inline constexpr Pj Manticore = {
    "bin/Resources/Graficos/Sprites/SpriteManticore.png",
    "bin/Resources/Graficos/Sprites/SpriteFlecha.png",
    Velocidad_media,
    Vida_media,
    true, // Rango
    false // No vuela
};

inline constexpr Pj Goblin = {
    "bin/Resources/Graficos/Sprites/Spritegoblin.png",
    "bin/Resources/Graficos/Sprites/SpriteFlecha.png",
    Velocidad_media,
    Vida_baja,
    false, // No rango
    false // No vuela
};
