#pragma once
class Pj
{
public:
	friend class arena;
	const float charSize = 4.0;
	const float speed = 10.0f;

	Texture2D spriteChar;
	Texture2D spriteShadow;
	Mesh shadowMesh;
	Model shadow;

	Pj(const char* principal, const char* sombra)
	{
		Texture2D spriteChar = LoadTexture(principal);
		Texture2D spriteShadow = LoadTexture(sombra);

		Mesh shadowMesh = GenMeshPlane(charSize, charSize, 1, 1); //dependencia del personaje -> Incluir en la clase
		Model shadowModel = LoadModelFromMesh(shadowMesh);
		shadowModel.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = spriteShadow;
	}
};

