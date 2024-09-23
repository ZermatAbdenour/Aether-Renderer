#pragma once
#include "Mesh.h"
#include "Ressources.h"
#include "Image.h"

class MeshRenderer {
public:
	MeshRenderer() = default;
	Mesh* mesh = Ressources::Primitives::Quad;
	Image* image = nullptr;
};