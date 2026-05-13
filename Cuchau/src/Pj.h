#pragma once
#include "raylib.h"
#include "Pj_info.h"


class Pj
{
//public:
	friend class arena;
	const float charSize = 4.0;
	const float speed = 10.0f;

	// Parámetros del personaje
	Pj_info Player{};

	// Vida máxima del personaje, se inicializa con el valor de vida del Pj
	float max_vida{};
	cntrl Controles{};

	//posicion y direccion del personaje
	Vector3 pos{};

	Texture2D Frames[3]; //array 3 texturas
	Texture2D Frames_shadow[3]; // Array de 3 texturas para sombras

	Mesh shadowMesh[3];
	Model shadow[3];

public:
	void drawshadow(Vector3 shadowpos);
	void UnloadPj();

	Pj(Pj_info p, cntrl c, Vector3 po, bool ip)
	{
		Player = p;
		max_vida = p.vida;
		Controles = c;

		Frames[0] = LoadTexture(Player.Sprite_1);
		Frames[1] = LoadTexture(Player.Sprite_2);
		Frames[2] = LoadTexture(Player.Sprite_3);

		Frames_shadow[0] = LoadTexture(Player.Sprite_1_shadow);
		Frames_shadow[1] = LoadTexture(Player.Sprite_2_shadow);
		Frames_shadow[2] = LoadTexture(Player.Sprite_3_shadow);

		for (int i = 0; i < 3; i++)
		{
			shadowMesh[i] = GenMeshPlane(charSize, charSize, 1, 1);
			shadow[i] = LoadModelFromMesh(shadowMesh[i]);
			shadow[i].materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = Frames_shadow[i];
		}

		pos = po; // Posición inicial del personaje
	}

	Pj() = default;
};