#pragma once
#include "raylib.h"

//  obstaculo.h — Obstaculo 3D con billboard, sombra y caja de colision
//
//  Cada obstaculo se renderiza como un billboard (sprite que mira a la camara)
//  con una sombra proyectada en el suelo. Tiene una caja de colision en el
//  plano XZ para empujar a los personajes que choquen contra el.

class obstaculo
{
    // Rendering
    Texture2D sprite{};          // Textura principal del obstaculo (billboard)
    Texture2D spriteShadow{};    // Textura de la sombra

    Mesh shadowMesh{};           // Malla plana para proyectar la sombra en el suelo
    Model shadow{};              // Modelo de la sombra (malla + textura)

    Vector3 pos{};               // Posicion 3D del billboard
    Vector3 shadowpos{};         // Posicion de la sombra (ligeramente sobre el suelo, desplazada en Z)

    float size{};                // Tamano del billboard en unidades 3D

    // Colision en el plano XZ
    float colCX{}, colCZ{};     // Centro de la caja de colision (X, Z)
    float colHW{}, colHL{};     // Mitad del ancho y largo de la caja de colision
    bool  solido{ true };       // Si es true, empuja a los personajes al colisionar

    // Clase amiga de interacciones
	friend class Interacciones;

    // Dibuja la sombra en el suelo con modo BLEND_MULTIPLIED
    void drawshadow() const;

public:

    // Constructor por defecto (necesario para arrays)
    obstaculo() = default;

    // Constructor: carga texturas, genera malla de sombra, configura colision
    //   textura: ruta de la imagen del billboard
    //   sombra:  ruta de la imagen de la sombra
    //   posicion: posicion 3D del obstaculo
    //   tam:     tamano del billboard
    //   colW/colL: ancho y largo de la caja de colision (0 = auto 40% del tamaño)
    //   sol:     si es solido (bloquea movimiento)
    obstaculo(const char* textura, const char* sombra, Vector3 posicion,
        float tam, float colW = 0, float colL = 0, bool sol = true) :
        pos(posicion), size(tam), solido(sol),
        // La sombra se coloca justo sobre el suelo (y=0.01), desplazada en Z
        shadowpos(posicion.x, 0.01f, posicion.z - tam / 1.5f)
    {
        // Cargar texturas del sprite y su sombra
        sprite = LoadTexture(textura);
        spriteShadow = LoadTexture(sombra);

        // Generar malla plana para la sombra, escalada segun el aspect ratio del sprite
        float aspect = (float)sprite.width / (float)sprite.height;
        shadowMesh = GenMeshPlane(tam * aspect, 1.5f * tam, 1, 1);
        shadow = LoadModelFromMesh(shadowMesh);
        shadow.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = spriteShadow;

        // Configurar caja de colision centrada en la posicion del obstaculo
        colCX = posicion.x;
        colCZ = posicion.z;
        colHW = colW * 1.5;  // Mitad del ancho
        colHL = colL / 2.5f;  // Mitad del largo
    }

    // Libera texturas y modelo de sombra de la memoria
    ~obstaculo() {
        UnloadModel(shadow);
        UnloadTexture(sprite);
        UnloadTexture(spriteShadow);
    }

	// Dibuja el obstaculo: primero la sombra (BLEND_MULTIPLIED) y luego el billboard
    void Draw(Camera camera) const;
    
    // Getters para la colision
    float getColCX() const { return colCX; }
    float getColCZ() const { return colCZ; }
    float getColHW() const { return colHW; }
    float getColHL() const { return colHL; }
    bool  isSolido() const { return solido; }

};