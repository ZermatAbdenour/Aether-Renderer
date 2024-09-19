#include "Mesh.h"

Mesh::Mesh(std::vector<float> vertices, std::vector<unsigned int> indices)
{
	Vertices = vertices;
	Indices = indices;
}