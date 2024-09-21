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
	std::vector<float>{
		1.0f,1.0f,
		1.0f,0.0f,
		0.0f,0.0f,
		0.0f,1.0f
	}
	,
	//Indices
	std::vector<unsigned int>{
		0, 1, 3,  
		1, 2, 3  
	}
};

Shader* Ressources::Shaders::Default = new Shader("VertexShader.vert", "FragmentShader.frag");