#pragma once
#include "Mesh.h"
#include "Shader.h"
static class Ressources {
public:
	static class Primitives {
	public:
		static Mesh* Quad;
	};
	
	static class Shaders {
	public:
		static Shader* Default;
	};
};