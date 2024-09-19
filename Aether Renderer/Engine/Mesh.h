#pragma once
#include <glm/glm.hpp>
#include <glad/glad.h>
#include <vector>

class Mesh {
public:
    std::vector<float> Vertices;
    std::vector<float> Indices;
public:
    Mesh() = default;
    Mesh(std::vector<float> vertices, std::vector<float> indices);
};