#include "Ressources.h"
#include "../Utilities/FileUtil.hpp"

Mesh* Ressources::Primitives::Quad = new Mesh(
std::vector<Mesh::Vertex> {
//positions           
Mesh::Vertex(glm::vec3(-0.5f, -0.5f, 0.0f), glm::vec2(0.0f, 0.0f)),
	Mesh::Vertex(glm::vec3(-0.5f, 0.5f, 0.0f), glm::vec2(0.0f, 1.0f)),
	Mesh::Vertex(glm::vec3(0.5f, 0.5f, 0.0f), glm::vec2(1.0f, 1.0f)),
	Mesh::Vertex(glm::vec3(0.5f, -0.5f, 0.0f), glm::vec2(1.0f, 0.0f))

},
std::vector<unsigned int>{
	0, 2, 1,
	0, 3, 2
}
);


Shader* Ressources::Shaders::Default = new Shader("VertexShader.vert", "FragmentShader.frag");