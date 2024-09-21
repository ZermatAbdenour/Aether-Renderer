#include "Mesh.h"


Mesh::Mesh(std::vector<Vertex> vertexData, std::vector<unsigned int> indicesData)
{
	vertices = vertexData;
	indices = indicesData;
}
