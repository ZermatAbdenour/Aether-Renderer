#pragma once
#include "Mesh.h"
#include "Ressources.h"
#include "Image.h"

class MeshRenderer {
public:
	MeshRenderer() = default;
	Mesh* mesh = Ressources::Primitives::Quad;
	Image* diffuse = nullptr;
	glm::vec4 baseColor = glm::vec4(1);
	Image* normalMap = nullptr;
	Image* metalicMap = nullptr;
	Image* roughnessMap = nullptr;
	Image* aoMap = nullptr;
	Image* emissiveMap = nullptr;

	/// <summary>
	/// AO , Roughness , and metalic are packed in a single texture
	/// </summary>
	bool packedAoRM;
	//PBR Values
	float metallic = 1;
	float roughness = 0.5f;
	float ao = 0;
};