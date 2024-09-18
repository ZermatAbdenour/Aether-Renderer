#pragma once
#include <glm/glm.hpp>
#include <glad/glad.h>

class Mesh {
public:
    Mesh(float vertices[], float indices[]);
    void Bind();

    static void Unbind();

private:
    unsigned int VBO, EBO, VAO;
};