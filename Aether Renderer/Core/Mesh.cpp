#include "Mesh.h"

Mesh::Mesh(float vertices[], float indices[])
{
	//Generate the Vertex Buffer Object and The Index Buffer Object
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);
	
	//Generate the Vertex array object
	glGenVertexArrays(1, &VAO);

	glBindVertexArray(VAO);
	
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
	
	glVertexAttribPointer(0,3,GL_FLOAT,false,3*sizeof(float),(void*)0);
	glEnableVertexAttribArray(0);
	//unbind the vertex array
	glBindVertexArray(0);
}

void Mesh::Bind()
{
	glBindVertexArray(VAO);
}

void Mesh::Unbind()
{
	glBindVertexArray(0);
}