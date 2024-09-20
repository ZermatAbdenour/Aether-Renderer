#pragma once
#include <glm/glm.hpp>
#include <iostream>
#include <vector>
#include "MeshRenderer.h"

class Entity
{
public:
	const char* Name = "Entity";
	Entity* Parent;

	glm::vec3 LocalPosition = glm::vec3(0);
	glm::vec3 EulerAngles = glm::vec3(0);
	glm::vec3 Scale = glm::vec3(1);
	glm::mat4 Model;
	MeshRenderer EntityRenderer;

	//Scene Heirarchy
	std::vector<std::shared_ptr<Entity>> Childs;

public:
	Entity();
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
	/// <summary>
	/// debug: print the Entity hierarchy
	/// </summary>
	void PrintEntityHierarchy(int depth);
	
};