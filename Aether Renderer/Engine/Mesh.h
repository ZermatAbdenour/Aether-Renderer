#pragma once
#include <glm/glm.hpp>
#include <glad/glad.h>
#include <vector>

class Mesh {
public:
    std::vector<float> Vertices;
    std::vector<float> Uvs;
    std::vector<unsigned int> Indices;
public:
    Mesh() = default;
    Mesh(std::vector<float> vertices, std::vector<float> uvs, std::vector<unsigned int> indices);
    std::vector<float> GetVertexData();
};