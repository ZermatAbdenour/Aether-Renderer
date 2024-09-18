#pragma once
#include <glm/glm.hpp>
#include <iostream>
#include <vector>
class Entity
{
public:
	const char* Name = "Entity";
	glm::vec3 Position;
	glm::vec3 EulerAngles;

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
	/// <summary>
	/// debug: print the Entity hierarchy
	/// </summary>
	void PrintEntityHierarchy(int depth);
};