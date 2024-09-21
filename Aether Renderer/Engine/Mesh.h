#pragma once
#include <glm/glm.hpp>
#include <glad/glad.h>
#include <vector>

class Mesh {
public:
    struct Vertex {
        glm::vec3 position;
        glm::vec3 normal;
        glm::vec2 uv;
    };
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
public:
    Mesh() = default;
    Mesh(std::vector<Vertex> vertexData, std::vector<unsigned int> indicesData);
};