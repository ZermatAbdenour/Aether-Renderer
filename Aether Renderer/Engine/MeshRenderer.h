#pragma once
#include "Mesh.h"
#include "Ressources.h"
#include "Image.h"

class MeshRenderer {
public:
	MeshRenderer() = default;
	Mesh* mesh = Ressources::Primitives::Quad;
	Image* diffuse = nullptr;
	Image* normalMap = nullptr;
	Image* specularMap = nullptr;
};