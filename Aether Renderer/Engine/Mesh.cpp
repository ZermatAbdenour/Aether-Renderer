#include "Mesh.h"

Mesh::Mesh(std::vector<float> vertices, std::vector<float> uvs, std::vector<unsigned int> indices)
{
	Vertices = vertices;
	Indices = indices;
	Uvs = uvs;
}

std::vector<float> Mesh::GetVertexData()
{
    std::vector<float> vertexData;

    for (size_t i = 0; i < Vertices.size(); i += 3) {
        if (i + 2 < Vertices.size()) { 
            // Add position
            vertexData.push_back(Vertices[i]);     // x
            vertexData.push_back(Vertices[i + 1]); // y
            vertexData.push_back(Vertices[i + 2]); // z

            //Add Uvs
            size_t uvIndex = (i / 3) * 2;
            if (uvIndex < Uvs.size()) {
                vertexData.push_back(Uvs[uvIndex]);     // u
                vertexData.push_back(Uvs[uvIndex + 1]); // v
            }
        }
    }

    return vertexData;
}