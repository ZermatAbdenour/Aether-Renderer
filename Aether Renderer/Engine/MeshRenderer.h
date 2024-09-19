#pragma once
#include "Mesh.h"
#include "Ressources.h"
class MeshRenderer {
public:
	MeshRenderer() = default;
	Mesh* Mesh = Ressources::Primitives::Quad;
	Shader* Shader = Ressources::Shaders::Default;
};