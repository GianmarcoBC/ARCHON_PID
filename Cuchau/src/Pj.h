#pragma once
#include "raylib.h"
class Pj
{
public:
	friend class arena;
	const float charSize = 4.0;
	const float speed = 10.0f;

	Texture2D spriteChar{};
	Texture2D spriteShadow{};
	Mesh shadowMesh;
	Model shadow;

	Pj(const char* principal, const char* sombra)
	{
		spriteChar = LoadTexture(principal);
		spriteShadow = LoadTexture(sombra);

		shadowMesh = GenMeshPlane(charSize, charSize, 1, 1); //dependencia del personaje -> Incluir en la clase
		shadow = LoadModelFromMesh(shadowMesh);
		shadow.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = spriteShadow;
	}

	Pj() = default;
};

