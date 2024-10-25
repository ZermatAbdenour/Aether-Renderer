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

	//PBR Values
	float metallic = 1;
	float roughness = 0.5f;
	float ao = 0;
};