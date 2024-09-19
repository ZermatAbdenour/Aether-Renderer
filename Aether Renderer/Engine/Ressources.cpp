#include "Ressources.h"
#include "../Utilities/FileUtil.hpp"

Mesh* Ressources::Primitives::Quad = new Mesh{
	//Vertices
	std::vector<float>{ 
		 0.5f,  0.5f, 0.0f,  // top right
		 0.5f, -0.5f, 0.0f,  // bottom right
		-0.5f, -0.5f, 0.0f,  // bottom left
		-0.5f,  0.5f, 0.0f   // top left 

	}
	,
	//Indices
	std::vector<unsigned int>{
		0, 1, 3,  // first Triangle
		1, 2, 3   // second Triangle
	}
};

Shader* Ressources::Shaders::Default = new Shader("VertexShader.vert", "FragmentShader.frag");