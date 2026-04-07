#pragma once

//Constantes de características de los personajes
inline constexpr float Velocidad_media = 500.0f, Velocidad_baja = 200.0f;
inline constexpr float Vida_muyalta = 2000.0f, Vida_alta = 1000.0f, Vida_media = 500.0f, Vida_baja = 200.0f;
inline constexpr float Fuerza_muyalta = 80.0f, Fuerza_alta = 50.0f, Fuerza_media = 10.0f, Fuerza_baja = 5.0f;

//Controles de los personajes
struct cntrl
{
    int up, down, left, right;
};

//Estructura para almacenar las características de cada personaje
struct Pj
{
    const char* nombre;
    const char* Sprite_1;
    const char* Sprite_2;
    const char* Sprite_3;
    const char* Ataque;
    float vel;
    float vida;
    float fuerza;
    int   frameCount;
    float frameSpeed;
    bool rango; // Rango (TRUE) o cuerpo a cuerpo (FALSE)
    bool vuela; // Puede volar (TRUE) o no (FALSE)
};

//Claro
inline constexpr Pj MH = {
    "MH",
    "bin/Resources/Graficos/Sprites/SpriteMH_0001.png",
    "bin/Resources/Graficos/Sprites/SpriteMH_0006.png",
    "bin/Resources/Graficos/Sprites/SpriteMH_0001.png",
    "bin/Resources/Graficos/Sprites/SpriteFlecha.png",
    Velocidad_media,
    Vida_media,
    Fuerza_alta,
    3, 
    0.1f,
    true, // Rango
    true // Vuela
};

inline constexpr Pj Phoenix = {
    "Phoenix",
    "bin/Resources/Graficos/Sprites/SpritePhoenix.png",
    "bin/Resources/Graficos/Sprites/SpriteMH_0002.png",
    "bin/Resources/Graficos/Sprites/SpriteMH_0003.png",
    "bin/Resources/Graficos/Sprites/SpriteFlecha.png",
    Velocidad_media,
    Vida_alta,
    Fuerza_alta,
    3,
    0.1f,
    false, // No rango
    true // Vuela
};

inline constexpr Pj Golem = {
    "Golem",
    "bin/Resources/Graficos/Sprites/SpriteGolem.png",
    "bin/Resources/Graficos/Sprites/SpriteMH_0002.png",
    "bin/Resources/Graficos/Sprites/SpriteMH_0003.png",
    "bin/Resources/Graficos/Sprites/SpriteFlecha.png",
    Velocidad_baja,
    Vida_alta,
    Fuerza_alta,
    3,
    0.1f,
    true, // Rango
    false // No vuela
};

inline constexpr Pj Djinni = {
    "Djinni",
    "bin/Resources/Graficos/Sprites/SpiteDjinni.png",
    "bin/Resources/Graficos/Sprites/SpriteMH_0002.png",
    "bin/Resources/Graficos/Sprites/SpriteMH_0003.png",
    "bin/Resources/Graficos/Sprites/SpriteFlecha.png",
    Velocidad_media,
    Vida_alta,
    Fuerza_media,
    3,
    0.1f,
    true, // Rango
    true // Vuela
};

inline constexpr Pj Unicorn = {
    "Unicorn",
    "bin/Resources/Graficos/Sprites/SpriteUnicorn_0002.png",
    "bin/Resources/Graficos/Sprites/SpriteMH_0002.png",
    "bin/Resources/Graficos/Sprites/SpriteMH_0003.png",
    "bin/Resources/Graficos/Sprites/SpriteFlecha.png",
    Velocidad_media,
    Vida_media,
    Fuerza_media,
    3,
    0.1f,
    true, // Rango
    false // No vuela
};

inline constexpr Pj Valkyrie = {
    "Valkyrie",
    "bin/Resources/Graficos/Sprites/SpriteValkyrie.png",
    "bin/Resources/Graficos/Sprites/SpriteMH_0002.png",
    "bin/Resources/Graficos/Sprites/SpriteMH_0003.png",
    "bin/Resources/Graficos/Sprites/SpriteFlecha.png",
    Velocidad_media,
    Vida_media,
    Fuerza_media,
    3,
    0.1f,
    true, // Rango
    true // Vuela
};

inline constexpr Pj Archer = {
    "Archer",
    "bin/Resources/Graficos/Sprites/SpriteArcherpxo.png",
    "bin/Resources/Graficos/Sprites/SpriteMH_0002.png",
    "bin/Resources/Graficos/Sprites/SpriteMH_0003.png",
    "bin/Resources/Graficos/Sprites/SpriteFlecha.png",
    Velocidad_media,
    Vida_baja,
    Fuerza_baja,
    3,
    0.1f,
    true, // Rango
    false // No vuela
};

inline constexpr Pj Knight = {
    "Knight",
    "bin/Resources/Graficos/Sprites/SpriteKnight.png",
    "bin/Resources/Graficos/Sprites/SpriteMH_0002.png",
    "bin/Resources/Graficos/Sprites/SpriteMH_0003.png",
    "bin/Resources/Graficos/Sprites/SpriteFlecha.png",
    Velocidad_media,
    Vida_baja,
    Fuerza_baja,
    3,
    0.1f,
    false, // No rango
    false // No vuela
};


//Oscuro
inline constexpr Pj Platero = {
    "Platero",
    "bin/Resources/Graficos/Sprites/SpritePlatero.png",
    "bin/Resources/Graficos/Sprites/SpriteMH_0002.png",
    "bin/Resources/Graficos/Sprites/SpriteMH_0003.png",
    "bin/Resources/Graficos/Sprites/SpriteFlecha.png",
    Velocidad_media,
    Vida_media,
    Fuerza_media,
    3,
    0.1f,
    true, // Rango
    true // Vuela
};

inline constexpr Pj ShapeShifter = {
    "ShapeShifter",
    "bin/Resources/Graficos/Sprites/SpriteShapeshifter.png",
    "bin/Resources/Graficos/Sprites/SpriteMH_0002.png",
    "bin/Resources/Graficos/Sprites/SpriteMH_0003.png",
    "bin/Resources/Graficos/Sprites/SpriteFlecha.png",
    Velocidad_media,
    Vida_alta,
    Fuerza_alta,
    3,
    0.1f,
    false, // No rango
    true // Vuela
};

inline constexpr Pj Troll = {
    "Troll",
    "bin/Resources/Graficos/Sprites/SpriteTroll.png",
    "bin/Resources/Graficos/Sprites/SpriteMH_0002.png",
    "bin/Resources/Graficos/Sprites/SpriteMH_0003.png",
    "bin/Resources/Graficos/Sprites/SpriteFlecha.png",
    Velocidad_baja,
    Vida_alta,
    Fuerza_alta,
    3,
    0.1f,
    true, // Rango
    false // No vuela
};

inline constexpr Pj Dragon = {
    "Dragon",
    "bin/Resources/Graficos/Sprites/SpriteDragon.png",
    "bin/Resources/Graficos/Sprites/SpriteMH_0002.png",
    "bin/Resources/Graficos/Sprites/SpriteMH_0003.png",
    "bin/Resources/Graficos/Sprites/SpriteFlecha.png",
    Velocidad_media,
    Vida_muyalta,
    Fuerza_muyalta,
    3,
    0.1f,
    true, // Rango
    true // Vuela
};

inline constexpr Pj Basilisk = {
    "Basilisk",
    "bin/Resources/Graficos/Sprites/SpriteBasilisk.png",
    "bin/Resources/Graficos/Sprites/SpriteMH_0002.png",
    "bin/Resources/Graficos/Sprites/SpriteMH_0003.png",
    "bin/Resources/Graficos/Sprites/SpriteFlecha.png",
    Velocidad_media,
    Vida_baja,
    Fuerza_alta,
    3,
    0.1f,
    true, // Rango
    false // No vuela
};

inline constexpr Pj Banshee = {
    "Banshee",
    "bin/Resources/Graficos/Sprites/SpriteBanshee.png",
    "bin/Resources/Graficos/Sprites/SpriteMH_0002.png",
    "bin/Resources/Graficos/Sprites/SpriteMH_0003.png",
    "bin/Resources/Graficos/Sprites/SpriteFlecha.png",
    Velocidad_media,
    Vida_media,
    Fuerza_media,
    3,
    0.1f,
    true, // Rango
    true // Vuela
};

inline constexpr Pj Manticore = {
    "Manticore",
    "bin/Resources/Graficos/Sprites/SpriteManticore.png",
    "bin/Resources/Graficos/Sprites/SpriteMH_0002.png",
    "bin/Resources/Graficos/Sprites/SpriteMH_0003.png",
    "bin/Resources/Graficos/Sprites/SpriteFlecha.png",
    Velocidad_media,
    Vida_media,
    Fuerza_baja,
    3,
    0.1f,
    true, // Rango
    false // No vuela
};

inline constexpr Pj Goblin = {
    "Goblin",
    "bin/Resources/Graficos/Sprites/Spritegoblin.png",
    "bin/Resources/Graficos/Sprites/SpriteMH_0002.png",
    "bin/Resources/Graficos/Sprites/SpriteMH_0003.png",
    "bin/Resources/Graficos/Sprites/SpriteFlecha.png",
    Velocidad_media,
    Vida_baja,
    Fuerza_baja,
    3,
    0.1f,
    false, // No rango
    false // No vuela
};
