#pragma once
#include <glm/glm.hpp>
#include <vector>

class Mesh {
public:
    struct Vertex {
        glm::vec3 position;
        glm::vec3 normal;
        glm::vec2 uv;
        glm::vec3 tangent;
        glm::vec3 biTangent;
    };
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
public:
    Mesh() = default;
    Mesh(std::vector<Vertex> vertexData, std::vector<unsigned int> indicesData);
};