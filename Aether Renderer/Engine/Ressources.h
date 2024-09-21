#pragma once
#include "Mesh.h"
#include "Shader.h"
class Ressources {
public:
	class Primitives {
	public:
		static Mesh* Quad;
	};
	
	class Shaders {
	public:
		static Shader* Default;
	};
};