#pragma once
#include <glm/glm.hpp>
#include <iostream>
#include <vector>
#include "MeshRenderer.h"

class Entity
{
public:
	const char* Name = "Entity";
	Entity* parent;

	glm::vec3 localPosition = glm::vec3(0);
	glm::vec3 eulerAngles = glm::vec3(0);
	glm::vec3 scale = glm::vec3(1);
	glm::mat4 model;
	MeshRenderer* meshRenderer = nullptr;

	//Scene Heirarchy
	std::vector<std::shared_ptr<Entity>> childs;

public:
	Entity();
	/// <summary>
	/// when creating Entities sometimes we check things before creating a mesh renderer .
	/// Setting the createRenderer to false will not create a mesh renderer at the start of the entity.
	/// </summary
	Entity(bool createRenderer);
	virtual ~Entity();
	/// <summary>
	/// Add a child to the Childs list of the entities
	/// </summary>
	/// <param name="child"></param>
	void AddChild(std::shared_ptr<Entity> child);
	/// <summary>
	/// Remove a child from the list of childs
	/// </summary>
	/// <param name="child"></param>
	void RemoveChild(std::shared_ptr<Entity> child);
	/// <summary>
	/// Create and add a Child to the list of entities
	/// </summary>
	/// <returns></returns>
	std::shared_ptr<Entity> AddChild(const char* name = "Entity");
	void CalculateModel();
	glm::vec3 GetForward();
	glm::vec3 GetUp();
	glm::vec3 GetRight();
	/// <summary>
	/// Create the Renderer
	/// </summary>
	void CreateMeshRenderer();
	/// <summary>
	/// debug: print the Entity hierarchy
	/// </summary>
	void PrintEntityHierarchy(int depth);

	
};