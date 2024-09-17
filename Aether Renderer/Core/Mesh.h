#pragma once
#include <glm/glm.hpp>
#include <glad/glad.h>
float vertices[] = {
    -0.5f, -0.5f, 0.0f,
     0.5f, -0.5f, 0.0f,
     0.0f,  0.5f, 0.0f
};

class Mesh {
public:
    unsigned int VBO, EBO, VAO;
    Mesh(float vertices[], float indices[]);

};